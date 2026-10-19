"""Check the data-directory macro without the suite's library dependencies."""
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class DataDirectories(unittest.TestCase):
    def test_install_and_runtime_paths_agree(self):
        version = re.search(r'AC_INIT\(\[geda-ai\], \[([^]]+)\]',
                            (ROOT / 'configure.ac').read_text())[1]
        package = 'geda-ai-' + version
        with tempfile.TemporaryDirectory() as directory:
            work = Path(directory)
            shutil.copy(ROOT / 'm4/geda-data-dirs.m4', work / 'data.m4')
            (work / 'configure.ac').write_text(
                f'AC_INIT([geda-ai], [{version}])\n'
                'AC_CONFIG_SRCDIR([data.m4])\n'
                'AC_CONFIG_HEADERS([config.h])\n'
                'm4_include([data.m4])\nAX_DATA_DIRS\n'
                'AC_CONFIG_FILES([paths.mk])\nAC_OUTPUT\n')
            (work / 'paths.mk.in').write_text(
                'prefix = @prefix@\ndatarootdir = @datarootdir@\n'
                'sysconfdir = @sysconfdir@\n'
                'data = @GEDADATADIR@\nalias = @GEDAIDATADIR@\n'
                'rc = @GEDARCDIR@\n'
                'all:\n\t@echo "$(data)|$(alias)|$(rc)"\n')
            def run(*args, env=None):
                return subprocess.check_output(args, cwd=work, env=env,
                                               stderr=subprocess.STDOUT,
                                               text=True)
            run('autoheader')
            run('autoconf')
            cases = [
                ([], '/usr/local/share/' + package,
                 '/usr/local/share/' + package),
                (['--prefix=/tmp/geda prefix'],
                 '/tmp/geda prefix/share/' + package,
                 '/tmp/geda prefix/share/' + package),
                (['--prefix=/tmp/geda', '--datarootdir=/tmp/shared',
                  '--with-rcdir=/tmp/config'], '/tmp/shared/' + package,
                 '/tmp/config'),
                (['--prefix=/tmp/geda', '--with-rcdir'],
                 '/tmp/geda/share/' + package, '/tmp/geda/etc/' + package),
            ]
            for flags, data, rc in cases:
                with self.subTest(flags=flags):
                    env = dict(os.environ, GEDARCDIR='/stale/release')
                    run('./configure', *flags, env=env)
                    header = (work / 'config.h').read_text()
                    for name, expected in [('GEDADATADIR', data),
                                           ('PKGDATADIR', data),
                                           ('GEDARCDIR', rc)]:
                        self.assertIn(f'#define {name} "{expected}"', header)
                    self.assertEqual(run('make', '-s', '-f', 'paths.mk').strip(),
                                     '|'.join([data, data, rc]))


if __name__ == '__main__':
    unittest.main()
