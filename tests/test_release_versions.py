"""Check configured C headers and generated gschem RC release identity."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]


class ReleaseVersions(unittest.TestCase):
    def test_configured_headers_and_rc_match_release(self):
        header = ROOT / 'version.h'
        rc = ROOT / 'gschem/lib/system-gschemrc'
        if not header.exists() or not rc.exists():
            self.skipTest('run configure and make -C gschem/lib first')
        release = re.search(r'AC_INIT\(\[geda-ai\], \[([^]]+)\]',
                            (ROOT / 'configure.ac').read_text())[1]
        dotted, date = release.rsplit('-', 1)
        self.assertRegex(date, r'^\d{8}$')
        for name in ('config.h', 'version.h'):
            with self.subTest(header=name):
                content = (ROOT / name).read_text()
                self.assertIn(f'#define PACKAGE_DATE_VERSION "{date}"', content)
                self.assertIn(f'#define PACKAGE_DOTTED_VERSION "{dotted}"', content)
        self.assertIn(f'(gschem-version "{date}")', rc.read_text())
        generated = (ROOT / 'gschem/lib/Makefile').read_text()
        self.assertRegex(generated, rf'(?m)^DATE_VERSION = {date}$')
        self.assertRegex(generated, rf'(?m)^PACKAGE_DATE_VERSION = {date}$')


if __name__ == '__main__':
    unittest.main()
