"""Resumable, checkpointed refactor of gschem's action registry.

The package drives gschem's migration from the DEFINE_ACTION preprocessor
pipeline (actions.decl.h comments in .bak) to a plain C function-pointer
registry.  Every artifact is derived from a deterministic oracle parsed out
of ``gschem/src/actions.c.bak``; the DeepSeek API is used only to re-derive
what the oracle already knows, so drift is detectable rather than invisible.

Run as ``python3 -m gedai.refactor <verb>``.
"""

__all__ = ["config", "oracle", "inventory", "validators", "checkpoint",
           "journal", "deepseek", "prompts"]
