"""Offline DO starter regression: real parser, netlister and chat context."""
from pathlib import Path
import json
import os
import re
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
BUILD = Path(os.environ.get('GEDA_TEST_BUILD_ROOT', ROOT))
FIXTURE = ROOT/'gnetlist-legacy/tests/do-meter'
sys.path.insert(0, str(ROOT))
from gedai.chat_backend import build_payload

EXPECTED = {
    'TPM_CS_N': 1, 'TPM_MISO': 2, 'TPM_RESET_N': 3, 'GND': 4,
    'TPM_MOSI': 5, 'TPM_SCLK': 6, 'TPM_IRQ_N': 7, '3V3': 8,
}


class DoMeter(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='geda-do-meter-')
        self.addCleanup(self.temp.cleanup)
        self.work = Path(self.temp.name)
        self.env = dict(os.environ, GUILE_AUTO_COMPILE='0', GEDALOG=str(self.work),
                        GEDADATA=str(ROOT/'gnetlist-legacy'),
                        GEDADATARC=str(BUILD/'gnetlist-legacy/lib'),
                        XDG_CONFIG_HOME=str(self.work/'config'),
                        XDG_CACHE_HOME=str(self.work/'cache'))
        directories = [ROOT/'libgeda/scheme', BUILD/'libgeda/scheme',
                       ROOT/'gnetlist-legacy/scheme', BUILD/'gnetlist-legacy/scheme']
        rc = ''.join('(scheme-directory '+json.dumps(str(p))+')\n' for p in directories)
        rc += ''.join('(component-library '+json.dumps(str(p))+')\n'
                      for p in [FIXTURE, ROOT/'symbols/micro', ROOT/'symbols/analog', ROOT/'symbols/maxim'])
        (self.work/'gafrc').write_text(rc)

    def command(self, program, *args):
        result = subprocess.run([str(program), *map(str, args)], cwd=self.work,
                                env=self.env, capture_output=True, text=True, timeout=30)
        self.assertEqual(result.returncode, 0, result.stdout+result.stderr)
        for error in ('CRITICAL', 'placeholder/missing component', 'Unbound variable'):
            self.assertNotIn(error, result.stderr)
        return result

    def netlist(self, schematic, name):
        output = self.work/(name+'.net')
        self.command(BUILD/'gnetlist-legacy/src/gnetlist-legacy',
                     '-g', 'geda', '-o', output, schematic)
        return output.read_text()

    def assert_connections(self, netlist):
        nets = {}
        for name, members in re.findall(r'^([\w]+) : (.*)$', netlist, re.M):
            nets[name] = set(re.findall(r'([A-Z]+\d+) (\d+)', members))
        expected = {name: {('U1', str(n)), ('U2', str(n))}
                    for name, n in EXPECTED.items()}
        expected['3V3'].update({('U3', '2'), ('R1', '1'), ('R2', '1'), ('C1', '1')})
        expected['GND'].update({('U3', str(n)) for n in range(5, 14)})
        expected['GND'].update({('B1', '2'), ('C1', '2')})
        expected.update({
            'RTC_SDA': {('U1', '9'), ('U3', '15'), ('R1', '2')},
            'RTC_SCL': {('U1', '10'), ('U3', '16'), ('R2', '2')},
            'RTC_VBAT': {('B1', '1'), ('U3', '14')},
        })
        for name, members in expected.items():
            self.assertEqual(nets.get(name), members, name)
        for refdes, device in [('U1', 'ESP32_INTERFACE'), ('U2', 'ATTPM20P'),
                               ('D1', 'LED'), ('D2', 'PHOTODIODE'), ('U3', 'DS3231'),
                               ('B1', 'BATTERY'), ('R1', 'RESISTOR'),
                               ('R2', 'RESISTOR'), ('C1', 'CAPACITOR')]:
            self.assertIn(refdes+' device='+device, netlist)

    def test_rtc_symbol_pin_contract(self):
        symbol = (ROOT/'symbols/maxim/DS3231-1.sym').read_text()
        pins = re.findall(r'^P [^\n]+\n\{\n(.*?)^\}', symbol, re.M | re.S)
        actual = {}
        for block in pins:
            n = int(re.search(r'^pinnumber=(\d+)$', block, re.M).group(1))
            self.assertNotIn(n, actual)
            actual[n] = re.search(r'^pinlabel=(.+)$', block, re.M).group(1)
        self.assertEqual(actual, {1: '32kHz', 2: 'VCC', 3: 'INT/SQW#',
                                 4: 'RST#', **{n: 'NC_GND' for n in range(5, 13)},
                                 13: 'GND', 14: 'VBAT', 15: 'SDA', 16: 'SCL'})

    def test_tpm_symbol_pin_contract(self):
        symbol = (ROOT/'symbols/micro/ATTPM20P-MA1-1.sym').read_text()
        pins = re.findall(r'^P [^\n]+\n\{\n(.*?)^\}', symbol, re.M | re.S)
        actual = {}
        for block in pins:
            number = re.search(r'^pinnumber=(\d+)$', block, re.M).group(1)
            label = re.search(r'^pinlabel=(.+)$', block, re.M).group(1)
            type_ = re.search(r'^pintype=(.+)$', block, re.M).group(1)
            self.assertNotIn(number, actual)
            actual[number] = (label, type_)
        self.assertEqual(actual, {
            '1': ('SPI_CS#', 'in'), '2': ('MISO', 'out'),
            '3': ('SPI_RST#', 'in'), '4': ('GND', 'pwr'),
            '5': ('MOSI', 'in'), '6': ('SPI_CLK', 'in'),
            '7': ('PIRQ#', 'oc'), '8': ('VCC', 'pwr'),
        })

    def test_connectivity_and_saved_page(self):
        original = FIXTURE/'do-meter.sch'
        self.assert_connections(self.netlist(original, 'original'))
        saved = self.work/'saved.sch'
        expression = '''(use-modules (geda page) (ice-9 textual-ports))
          (define p (string->page "fixture.sch"
            (call-with-input-file %s get-string-all)))
          (call-with-output-file %s (lambda (port) (display (page->string p) port)))
          (close-page! p)''' % (json.dumps(str(original)), json.dumps(str(saved)))
        self.command(BUILD/'gaf/gaf', 'shell', '-c', expression)
        self.assert_connections(self.netlist(saved, 'reloaded'))
        data = saved.read_text()
        for field in ('calibration_method=', 'calibration_coefficients=',
                      'excitation_control=', 'receiver_gain=', 'calibration_storage=',
                      'calibration_record=', 'calibration_signing=', 'calibration_time=',
                      'flash_capacity=', 'calibration_recovery='):
            self.assertIn(field, data)
        payload = build_payload({'provider': 'openrouter', 'model': 'fixture-only',
                                 'question': 'Review calibration requirements.',
                                 'schematic': data})
        self.assertIn(data, payload['messages'][0]['content'])

    def test_detects_disconnected_tpm_pin(self):
        # A one-grid endpoint error must be detectable even though the file parses.
        broken = self.work/'broken.sch'
        data = (FIXTURE/'do-meter.sch').read_text()
        self.assertIn('N 8000 6400 7100 6400 4', data)
        broken.write_text(data.replace('N 8000 6400 7100 6400 4',
                                       'N 7900 6400 7100 6400 4', 1))
        netlist = self.netlist(broken, 'broken')
        with self.assertRaises(AssertionError):
            self.assert_connections(netlist)


if __name__ == '__main__':
    unittest.main()
