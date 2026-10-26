"""Convert a real connected schematic using a bundled test-only footprint."""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
BUILD = Path(os.environ.get('GEDA_TEST_BUILD_ROOT', ROOT))


class Conversion(unittest.TestCase):
    def test_connected_conversion(self):
        with tempfile.TemporaryDirectory(prefix='geda-conversion-') as directory:
            work = Path(directory)
            result = subprocess.run(
                ['sh', str(ROOT/'utils/examples/gsch2pcb/run-connected-test.sh'), str(work)],
                cwd=ROOT, capture_output=True, text=True, timeout=30,
                env=dict(os.environ, GEDA_TEST_BUILD_ROOT=str(BUILD)))
            self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
            self.assertNotIn("can't find PCB element", result.stdout+result.stderr)
            self.assertNotIn('CRITICAL', result.stderr)
            net = (work/'connected.net').read_text().split()
            self.assertEqual(set(net), {'LINK', 'R1-2', 'R2-1'})
            board = (work/'connected.pcb').read_text()
            self.assertIn('"R1" "10k"', board)
            self.assertIn('"R2" "1k"', board)
            self.assertEqual(board.count('Element['), 2)
            self.assertGreater((work/'connected.cmd').stat().st_size, 0)


if __name__ == '__main__':
    unittest.main()
