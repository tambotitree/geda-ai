"""Command line entry point: ``python3 -m gedai.refactor <verb>``.

Verbs
-----
inventory   parse the oracle and write ``.refactor/actions.json``
oracle      verify the inventory parses cleanly and report the digest
gen         regenerate ``gschem/src/actions.{c,h}`` from the oracle
verify      run the acceptance checks and compile gate on what is on disk
run         drive the per-action unit loop against the API (resumable)
status      summarise progress, token use and remaining time
checkpoint  commit the working tree and tag it ``ckpt/N``
rollback    reset the branch to a checkpoint tag
"""

import argparse
import datetime
import sys

from . import (checkpoint, config, generate, inventory, journal, oracle,
               units, validators)


def _entries():
    return inventory.build()


def cmd_inventory(args):
    entries = _entries()
    problems = oracle.validate(oracle.parse_file(config.ORACLE_FILE))
    path = inventory.write(entries)
    for cid, actual, expected in oracle.id_mismatches(entries):
        print(f"note: {cid} uses id {actual!r} (cid would give {expected!r})")
    print(f"{len(entries)} actions -> {path}")
    if problems:
        print("PROBLEMS:")
        for problem in problems:
            print("  " + problem)
        return 1
    return 0


def cmd_oracle(args):
    records = oracle.parse_file(config.ORACLE_FILE)
    problems = oracle.validate(records)
    digest = oracle.body_sha256("".join(r["body"] for r in records))
    print(f"actions: {len(records)}")
    print(f"combined body digest: {digest}")
    print(f"doc comments: {sum(1 for r in records if r['comment'])}")
    if problems:
        for problem in problems:
            print("  " + problem)
        return 1
    print("oracle: clean")
    return 0


def cmd_gen(args):
    entries = _entries()
    source, rewrites = generate.render_source(entries)
    header = generate.render_header(entries)
    if args.dry_run:
        print(f"would write {len(source)} bytes of actions.c, "
              f"{len(header)} bytes of actions.h")
        return 0
    config.ACTIONS_C.write_text(source)
    config.ACTIONS_H.write_text(header)
    for cid, changes in rewrites.items():
        print(f"rewrite: {cid}: {'; '.join(changes)}")
        journal.append("body-rewrite", cid=cid, changes=changes)
    print(f"wrote {config.ACTIONS_C} and {config.ACTIONS_H}")
    return 0


def cmd_verify(args):
    entries = _entries()
    source = config.ACTIONS_C.read_text()
    header = config.ACTIONS_H.read_text()
    problems = []
    problems += oracle.validate(oracle.parse_file(config.ORACLE_FILE))
    problems += validators.check_arity(source, header, len(entries), entries)
    problems += validators.check_bodies(source, entries)
    problems += validators.check_rows(source, entries)
    problems += validators.check_no_forbidden_source(source)

    print(f"oracle actions: {len(entries)}")
    print(f"arity: {'ok' if not validators.check_arity(source, header, len(entries), entries) else 'FAIL'}")
    print(f"bodies: {'ok' if not validators.check_bodies(source, entries) else 'FAIL'}")
    print(f"rows: {'ok' if not validators.check_rows(source, entries) else 'FAIL'}")

    if problems:
        for problem in problems:
            print("  " + problem)
    if not args.no_compile:
        ok, errors = units.compile_actions_c()
        print(f"compile: {'ok' if ok else 'FAIL'}")
        for line in errors:
            print("  " + line)
        if not ok:
            problems.append("actions.c does not compile")
    if problems:
        journal.append("verify-failed", problems=problems[:10])
        return 1
    journal.append("verify-ok", actions=len(entries))
    return 0


def cmd_run(args):
    entries = _entries()
    until = config.parse_until(args.until)
    api_key = config.api_key()
    done = journal.completed_cids() if args.resume else set()
    skipped_rejects = journal.rejected_cids() if (args.resume
                                                  and not args.retry) else set()
    pending = [e for e in entries
               if e["cid"] not in done and e["cid"] not in skipped_rejects]
    if args.only:
        pending = [e for e in pending if e["cid"] in set(args.only)]
    limit = args.max_units or len(pending)
    pending = pending[:limit]

    print(f"pending: {len(pending)} (skipping {len(done)} verified, "
          f"{len(skipped_rejects)} rejected earlier)")
    print(f"hard stop: {until.isoformat()} "
          f"({config.minutes_left(until)} min)")
    if not pending:
        print("nothing to do")
        return 0

    journal.append("run-start", pending=len(pending), until=until.isoformat(),
                   model=config.API_MODEL, resume=bool(args.resume))
    accepted = failed = 0
    spent = 0

    for entry in pending:
        if config.minutes_left(until) <= 0:
            print("hard stop reached; stopping cleanly")
            journal.append("run-stop", reason="deadline", accepted=accepted,
                           failed=failed)
            break
        cid = entry["cid"]
        try:
            result = units.run_unit(entry, api_key=api_key)
        except units.UnitError as exc:
            failed += 1
            print(f"  {cid}: REJECTED ({exc})")
            journal.append("unit-rejected", cid=cid, reason=str(exc))
            continue
        except Exception as exc:  # transport/protocol: stop, do not spin
            failed += 1
            journal.append("run-stop", reason=f"api error: {exc}",
                           accepted=accepted, failed=failed)
            print(f"stopping: {exc}")
            break

        ok, errors = units.compile_actions_c()
        if not ok:
            failed += 1
            print(f"  {cid}: COMPILE FAILED")
            for line in errors:
                print("    " + line)
            journal.append("unit-rejected", cid=cid, reason="compile",
                           errors=errors)
            # Undo the splice so the file returns to the last verified state.
            checkpoint._git("checkout", "--", "gschem/src/actions.c")
            continue

        # The unit is verified; record it in the tree.  A None result means
        # the model reproduced the oracle exactly and the file already
        # matches, which is success, not failure.
        try:
            committed = units.commit_unit(entry, result)
        except units.UnitError as exc:
            failed += 1
            print(f"  {cid}: POST-APPLY FAILED ({exc})")
            journal.append("unit-rejected", cid=cid, reason=str(exc))
            continue

        accepted += 1
        spent += (result["usage"] or {}).get("total_tokens", 0)
        how = f"committed {committed}" if committed else "verified (no diff)"
        print(f"  {cid}: {how} ({result['elapsed_s']}s, "
              f"{spent} tokens so far)")

    checkpoint.write_state(accepted=accepted, failed=failed,
                           tokens=spent, pending=len(pending) - accepted)
    print(f"accepted {accepted}, rejected {failed}, tokens ~{spent}")
    if args.budget and spent > args.budget:
        print("token budget exceeded; stopping")
    journal.append("run-end", accepted=accepted, failed=failed, tokens=spent)
    return 0


def cmd_status(args):
    entries = _entries()
    done = journal.completed_cids()
    usage = journal.usage_totals()
    print(f"actions:   {len(entries)}")
    print(f"verified:  {len(done)}")
    print(f"remaining: {len(entries) - len(done)}")
    print(f"tokens:    {usage['total_tokens']} "
          f"(prompt {usage['prompt_tokens']}, "
          f"completion {usage['completion_tokens']})")
    print(f"drift:     {len(journal.drift())} events")
    print(f"time left: {config.minutes_left()} min until {config.DEFAULT_UNTIL}")
    state = checkpoint.read_state()
    if state:
        print(f"last state: {state}")
    return 0


def cmd_checkpoint(args):
    name, commit = checkpoint.save(args.number)
    print(f"{name} -> {commit[:12]}")
    return 0


def cmd_rollback(args):
    commit = checkpoint.rollback(args.checkpoint)
    print(f"rolled back to {args.checkpoint} ({commit[:12]})")
    return 0


def build_parser():
    parser = argparse.ArgumentParser(
        prog="python3 -m gedai.refactor",
        description="Checkpointed refactor of gschem's action registry.")
    sub = parser.add_subparsers(dest="verb", required=True)

    p = sub.add_parser("inventory", help="parse the oracle to actions.json")
    p.set_defaults(func=cmd_inventory)

    p = sub.add_parser("oracle", help="check the oracle parses cleanly")
    p.set_defaults(func=cmd_oracle)

    p = sub.add_parser("gen", help="regenerate actions.c and actions.h")
    p.add_argument("--dry-run", action="store_true")
    p.set_defaults(func=cmd_gen)

    p = sub.add_parser("verify", help="run acceptance checks and compile")
    p.add_argument("--no-compile", action="store_true")
    p.set_defaults(func=cmd_verify)

    p = sub.add_parser("run", help="drive the per-action model loop")
    p.add_argument("--resume", action="store_true",
                   help="skip actions already verified in the journal")
    p.add_argument("--retry", action="store_true",
                   help="with --resume, also retry actions the model got "
                        "wrong earlier")
    p.add_argument("--only", nargs="+", metavar="CID",
                   help="restrict to these cids")
    p.add_argument("--max-units", type=int,
                   help="stop after this many actions")
    p.add_argument("--until", default=None,
                   help="ISO-8601 hard stop (default 2026-10-18T00:00:00Z)")
    p.add_argument("--budget", type=int, dest="budget",
                   help="stop when this many tokens have been spent")
    p.set_defaults(func=cmd_run)

    p = sub.add_parser("status", help="summarise progress")
    p.set_defaults(func=cmd_status)

    p = sub.add_parser("checkpoint", help="commit and tag ckpt/N")
    p.add_argument("--number", type=int)
    p.set_defaults(func=cmd_checkpoint)

    p = sub.add_parser("rollback", help="reset the branch to a checkpoint")
    p.add_argument("checkpoint")
    p.set_defaults(func=cmd_rollback)
    return parser


def main(argv=None):
    args = build_parser().parse_args(argv)
    try:
        return args.func(args)
    except (config.ConfigError, checkpoint.CheckpointError,
            oracle.OracleError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
