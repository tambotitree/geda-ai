"""Check command dispatch, isolated config I/O, Guile execution and Cairo export."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
BUILD_ROOT = Path(os.environ.get('GEDA_TEST_BUILD_ROOT', ROOT))


class GafCommands(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory(prefix='geda-gaf-')
        self.addCleanup(self.directory.cleanup)
        self.work = Path(self.directory.name)
        self.environment = dict(os.environ, GUILE_AUTO_COMPILE='0',
                                XDG_CONFIG_HOME=str(self.work/'config'),
                                GEDALOG=str(self.work))

    def run_gaf(self, *arguments):
        return subprocess.run([str(BUILD_ROOT/'gaf/gaf'), '--no-rcfiles', *arguments],
                              env=self.environment, cwd=self.work, text=True,
                              capture_output=True, timeout=20)

    def test_help_and_unknown_option(self):
        for command in ('config', 'shell', 'export'):
            result = self.run_gaf(command, '--help')
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn('Usage:', result.stdout)
        result = self.run_gaf('config', '--not-a-valid-option')
        self.assertEqual(result.returncode, 1, result.stderr)
        self.assertIn('help', result.stderr)

    def test_project_configuration_round_trip(self):
        options = ('config', '--project='+str(self.work))
        result = self.run_gaf(*options, 'test', 'message', 'isolated value')
        self.assertEqual(result.returncode, 0, result.stderr)
        result = self.run_gaf(*options, 'test', 'message')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout.strip(), 'isolated value')

    def test_scheme_execution(self):
        result = self.run_gaf('shell', '-c', '(display (+ 19 23)) (newline)')
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertEqual(result.stdout.strip(), '42')

    def test_symbol_exports(self):
        for format_, signature in [('png', b'\x89PNG\r\n\x1a\n'), ('svg', b'<?xml')]:
            output = self.work/('symbol.'+format_)
            result = self.run_gaf('export', '-f', format_, '-o', str(output),
                                  str(ROOT/'gsymcheck/tests/correct.sym'))
            self.assertEqual(result.returncode, 0, result.stderr)
            data = output.read_bytes()
            self.assertTrue(data.startswith(signature))
            self.assertGreater(len(data), 100)


if __name__ == '__main__':
    unittest.main()
