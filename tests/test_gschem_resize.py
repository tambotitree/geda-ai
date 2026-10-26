#!/usr/bin/env python3
"""Opt-in macOS GUI regression: resize empty and example pages in private processes."""
import argparse
import ast
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys
import tempfile


def main():
    """Build a private GTK probe; launch without touching user settings or designs."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--data', type=Path, help='Override the configured runtime data directory')
    args = parser.parse_args()
    if sys.platform != 'darwin':
        parser.error('This opt-in probe uses macOS DYLD injection')
    root = Path(__file__).resolve().parents[1]
    match = re.search(r'^#define GEDADATADIR (".*")$', (root / 'config.h').read_text(), re.MULTILINE)
    data = args.data or (Path(ast.literal_eval(match[1])) if match else None)
    if data is None or not (data / 'system-gschemrc').is_file():
        parser.error('Install runtime data first or supply --data')
    flags = shlex.split(subprocess.check_output(['pkg-config', '--cflags', '--libs', 'gtk+-3.0'], text=True))
    with tempfile.TemporaryDirectory(prefix='gschem-resize-') as directory:
        work = Path(directory)
        probe = work / 'resize-probe.dylib'
        subprocess.run(['cc', '-std=gnu11', '-Wall', '-Wextra', '-Werror', '-Wpedantic',
                        '-dynamiclib', str(root / 'tests/test_gschem_resize.c'),
                        '-o', str(probe), *flags], check=True)
        env = os.environ.copy()
        env.update(HOME=str(work), XDG_CONFIG_HOME=str(work), XDG_CACHE_HOME=str(work),
                   GEDADATA=str(data), GEDADATARC=str(data), GUILE_AUTO_COMPILE='0',
                   G_DEBUG='fatal-criticals', DYLD_INSERT_LIBRARIES=str(probe),
                   DYLD_LIBRARY_PATH=f'{root}/libgedacairo/.libs:{root}/libgeda/src/.libs')
        for name, files in [('empty', []), ('example', [str(root / 'gschem/examples/cmos-nand.sch')])]:
            result = subprocess.run([str(root / 'gschem/src/.libs/gschem'), *files],
                                    env=env, capture_output=True, text=True, timeout=25)
            if result.returncode or 'PASS: startup' not in result.stdout:
                print(result.stdout + result.stderr, file=sys.stderr)
                raise SystemExit(f'{name}: resize regression failed ({result.returncode})')
            print(name + ': ' + result.stdout.strip())


if __name__ == '__main__':
    main()
