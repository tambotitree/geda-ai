# Optical DO meter starter fixture

This is an architectural regression fixture, not a finished or validated sensor.
It contains an ESP32 logical interface, the ATTPM20P symbol, an excitation LED,
and a photodiode. U1 port numbers are **logical test ports, not ESP32 GPIO or
package pins**. U2 uses the sourced eight-pin Microchip mapping; no footprint
has been assigned. The LED driver, TIA, ADC, power supply and physical ESP32
module selection remain to be designed. Optical coupling is an annotation,
not an electrical net. Unconnected optical pins are intentional at this stage.

The coating candidate is PtTFPP on porous anodized aluminum, based on:
https://www.mdpi.com/1424-8220/9/6/4151
The paper uses intensity imaging; it does not validate our proposed electronics.

Calibration requirements stored as U1 attributes survive serialization:

- Adjustable LED current and duty cycle, with experimentally established limits.
- Selectable receiver gain, with bandwidth characterized for each setting.
- Oxygen reference measurements and temperature characterization.
- Versioned coefficients and reference conditions in nonvolatile storage.

Do not substitute electronic gain adjustment for oxygen calibration. No gain,
LED current, modulation frequency, or calibration coefficient is validated yet.

Run from the repository root:

    python3 tests/test_do_meter.py

The offline test uses the real libgeda parser/serializer through gaf, the legacy
netlister, and the AI chat payload builder. It checks all eight named TPM nets
before and after a disk round trip, component identities, calibration metadata,
and schematic context supplied to chat. A deliberate one-grid disconnection
must be detected. It makes no network calls and needs no API key.

To open from the repository root:

    sh gnetlist-legacy/tests/do-meter/view.sh

The launcher enters the fixture directory so its gafrc registers source symbols
before the schematic loads. gschem -c expressions run after file loading and
therefore cannot register missing symbols in time for the initial load.

## Flash and signed calibration history requirements

Reserve module flash for active calibration (NVS) and a separate history log.
Select an ESP32 module and partition sizes before deciding whether additional
SPI NOR is needed. Retention duration, record size and write frequency determine
capacity and endurance; do not use a continuously growing NVS key set as a log.
Storage reference: https://docs.espressif.com/projects/esp-idf/en/release-v5.4/esp32/api-reference/storage/index.html

Each proposed record includes schema version, device identity, sequence number,
UTC timestamp and time source/quality, coefficients, calibration reference
conditions, previous-record digest, signing-key identifier and signature.
Define a canonical encoding before signing. Keep the signing key in the TPM;
store the record and signature in flash. A signature authenticates the signed
bytes and key, not measurement accuracy or the truth of a supplied date.

A trusted time source or an RTC with an explicit trust policy remains required.
A hash chain detects interior edits when signatures are verified, but truncation
or rollback also needs a protected checkpoint/counter or external witness.
Power-loss-safe commits, recovery, signature checks and rollback protection are
firmware requirements, not implemented or verified by this schematic regression.
The current test checks that these requirements survive saving/reloading and
reach AI Assist. No TPM hardware or real cryptographic signing is exercised.

## Battery-backed RTC

U3 is a DS3231 (integrated crystal), powered from 3V3, with a primary CR2032
on VBAT. B1 negative is grounded; there is no charging circuit or connection
from VBAT to the main rail. SDA/SCL have initial 4.7k pull-ups to 3V3, and
C1 is 100nF across VCC/GND. Confirm I2C rise times when bus loading is known.
Pins 5–12 are grounded as required by the datasheet. Optional outputs 32kHz,
INT/SQW and RST are unused. U1 logical ports 9/10 represent I2C; GPIOs remain TBD.
Source: https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf

Firmware must initialize UTC, check oscillator-stop flag (OSF), and record time
quality and clock adjustments. Battery backup preserves time; it does not make
the clock tamper-proof or establish trusted UTC. Physical RTC/battery-holder
footprints and battery-life verification remain pending.
