"""Check gschlas embedding/unembedding with isolated runtime configuration."""
from pathlib import Path
import json
import os
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
BUILD = Path(os.environ.get('GEDA_TEST_BUILD_ROOT', ROOT))


class Gschlas(unittest.TestCase):
    def test_embed_unembed_round_trip(self):
        with tempfile.TemporaryDirectory(prefix='geda-gschlas-') as directory:
            work = Path(directory)
            rc = work/'config'
            rc.mkdir()
            shutil.copyfile(BUILD/'utils/lib/system-gschlasrc', rc/'system-gschlasrc')
            (work/'gafrc').write_text(
                '(component-library '+json.dumps(str(ROOT/'symbols/analog'))+')\n')
            schematic = work/'resistor.sch'
            schematic.write_text('v 20261019 2\nC 1000 1000 1 0 0 resistor-1.sym\n{\n'
                                 'T 1000 1500 5 10 1 1 0 0 1\nrefdes=R1\n'
                                 'T 1000 1700 5 10 1 1 0 0 1\nvalue=22k\n}\n')
            env = dict(os.environ, GUILE_AUTO_COMPILE='0', GEDALOG=str(work),
                       GEDADATA=str(ROOT/'libgeda'), GEDADATARC=str(rc),
                       XDG_CONFIG_HOME=str(rc), XDG_CACHE_HOME=str(work/'cache'))
            for flag, embedded in [('-e', True), ('-u', False)]:
                result = subprocess.run([str(BUILD/'utils/gschlas/gschlas'), flag,
                                         str(schematic)], cwd=work, env=env,
                                        capture_output=True, text=True, timeout=20)
                self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
                for error in ('CRITICAL', 'not found', 'Unbound variable'):
                    self.assertNotIn(error, result.stderr)
                data = schematic.read_text()
                self.assertEqual('EMBEDDEDresistor-1.sym' in data, embedded)
                self.assertIn('refdes=R1', data)
                self.assertIn('value=22k', data)


if __name__ == '__main__':
    unittest.main()
