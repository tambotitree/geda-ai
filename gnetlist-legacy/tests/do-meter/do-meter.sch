v 20261019 2
T 400 11000 5 10 1 1 0 0 1
OPTICAL DO METER - architectural test fixture; NOT a completed circuit
T 400 10600 5 10 1 1 0 0 1
ESP32 interface ports are logical: GPIO/module selection pending.
T 400 10200 5 10 1 1 0 0 1
PtTFPP / anodized aluminum; LED wavelength, gain and timing require characterization.
C 1000 4000 1 0 0 esp32-interface.sym
{
T 1000 9400 5 10 1 1 0 0 1
refdes=U1
T 1000 7500 5 10 0 1 0 0 1
device=ESP32_INTERFACE
T 1000 9200 5 10 1 1 0 0 1
value=ESP32 module TBD
T 1000 7900 5 10 0 1 0 0 1
design_status=logical interface only; no physical GPIO mapping
T 1000 8100 5 10 0 1 0 0 1
calibration_method=oxygen reference points plus temperature characterization
T 1000 8300 5 10 0 1 0 0 1
calibration_coefficients=TBD after measurements
T 1000 8500 5 10 0 1 0 0 1
excitation_control=adjustable LED current and duty cycle; limits TBD
T 1000 8700 5 10 0 1 0 0 1
receiver_gain=selectable gain; values and bandwidth TBD
T 1000 8900 5 10 0 1 0 0 1
calibration_storage=flash: active calibration in NVS; signed history in dedicated log partition
T 1000 9100 5 10 0 1 0 0 1
calibration_record=version, device_id, sequence, timestamp_utc, time_source, coefficients, conditions, previous_hash, key_id, signature
T 1000 9300 5 10 0 1 0 0 1
calibration_signing=TPM-held key signs canonical record digest; implementation pending
T 1000 9500 5 10 0 1 0 0 1
calibration_time=DS3231 battery-backed RTC; validate OSF and log UTC source/quality; TPM signs supplied date
T 1000 9700 5 10 0 1 0 0 1
flash_capacity=ESP32 module flash allocation TBD; external flash if retention requires
T 1000 9900 5 10 0 1 0 0 1
calibration_recovery=power-loss-safe commit, signature verification, rollback detection pending
}
C 8000 4000 1 0 0 ATTPM20P-MA1-1.sym
{
T 8000 7900 5 10 1 1 0 0 1
refdes=U2
T 8000 7500 5 10 0 1 0 0 1
device=ATTPM20P
T 8000 7700 5 10 1 1 0 0 1
value=ATTPM20P
T 8000 7900 5 10 0 1 0 0 1
design_status=verified pin map; footprint pending
}
N 3800 4400 4700 4400 4
{
T 3800 4500 5 10 1 1 0 0 1
netname=TPM_CS_N
}
N 8000 6400 7100 6400 4
{
T 7100 6500 5 10 1 1 0 0 1
netname=TPM_CS_N
}
N 3800 4800 4700 4800 4
{
T 3800 4900 5 10 1 1 0 0 1
netname=TPM_MISO
}
N 8000 6000 7100 6000 4
{
T 7100 6100 5 10 1 1 0 0 1
netname=TPM_MISO
}
N 3800 5200 4700 5200 4
{
T 3800 5300 5 10 1 1 0 0 1
netname=TPM_RESET_N
}
N 8000 5600 7100 5600 4
{
T 7100 5700 5 10 1 1 0 0 1
netname=TPM_RESET_N
}
N 3800 5600 4700 5600 4
{
T 3800 5700 5 10 1 1 0 0 1
netname=GND
}
N 8000 5200 7100 5200 4
{
T 7100 5300 5 10 1 1 0 0 1
netname=GND
}
N 3800 6000 4700 6000 4
{
T 3800 6100 5 10 1 1 0 0 1
netname=TPM_MOSI
}
N 11200 5200 12100 5200 4
{
T 11200 5300 5 10 1 1 0 0 1
netname=TPM_MOSI
}
N 3800 6400 4700 6400 4
{
T 3800 6500 5 10 1 1 0 0 1
netname=TPM_SCLK
}
N 11200 5600 12100 5600 4
{
T 11200 5700 5 10 1 1 0 0 1
netname=TPM_SCLK
}
N 3800 6800 4700 6800 4
{
T 3800 6900 5 10 1 1 0 0 1
netname=TPM_IRQ_N
}
N 11200 6000 12100 6000 4
{
T 11200 6100 5 10 1 1 0 0 1
netname=TPM_IRQ_N
}
N 3800 7200 4700 7200 4
{
T 3800 7300 5 10 1 1 0 0 1
netname=3V3
}
N 11200 6400 12100 6400 4
{
T 11200 6500 5 10 1 1 0 0 1
netname=3V3
}
C 2000 1000 1 0 0 led-1.sym
{
T 2000 2500 5 10 1 1 0 0 1
refdes=D1
T 2000 4500 5 10 0 1 0 0 1
device=LED
T 2000 2300 5 10 1 1 0 0 1
value=Excitation wavelength TBD
T 2000 4900 5 10 0 1 0 0 1
design_status=optical block only; driver/current limit pending
}
C 8000 1000 1 0 0 photodiode-1.sym
{
T 8000 2500 5 10 1 1 0 0 1
refdes=D2
T 8000 4500 5 10 0 1 0 0 1
device=PHOTODIODE
T 8000 2300 5 10 1 1 0 0 1
value=Detector/filter TBD
T 8000 4900 5 10 0 1 0 0 1
design_status=optical block only; TIA/ADC pending
}
T 3500 500 5 10 1 1 0 0 1
LED -> PtTFPP coating -> emission filter -> photodiode (OPTICAL PATH)
C 15000 4000 1 0 0 DS3231-1.sym
{
T 15000 8500 5 10 1 1 0 0 1
refdes=U3
T 15000 8000 5 10 0 1 0 0 1
device=DS3231
T 15000 8300 5 10 1 1 0 0 1
value=DS3231
T 15000 8400 5 10 0 1 0 0 1
design_status=RTC schematic pinout verified; footprint pending
T 15000 8600 5 10 0 1 0 0 1
rtc_policy=UTC; reject unset time or OSF=1; log every clock adjustment
}
N 15000 6800 14200 6800 4
{
T 14200 6900 5 10 1 1 0 0 1
netname=3V3
}
N 15000 5600 14200 5600 4
{
T 14200 5700 5 10 1 1 0 0 1
netname=GND
}
N 15000 5200 14200 5200 4
{
T 14200 5300 5 10 1 1 0 0 1
netname=GND
}
N 15000 4800 14200 4800 4
{
T 14200 4900 5 10 1 1 0 0 1
netname=GND
}
N 15000 4400 14200 4400 4
{
T 14200 4500 5 10 1 1 0 0 1
netname=GND
}
N 19000 4400 19800 4400 4
{
T 19000 4500 5 10 1 1 0 0 1
netname=GND
}
N 19000 4800 19800 4800 4
{
T 19000 4900 5 10 1 1 0 0 1
netname=GND
}
N 19000 5200 19800 5200 4
{
T 19000 5300 5 10 1 1 0 0 1
netname=GND
}
N 19000 5600 19800 5600 4
{
T 19000 5700 5 10 1 1 0 0 1
netname=GND
}
N 19000 6000 19800 6000 4
{
T 19000 6100 5 10 1 1 0 0 1
netname=GND
}
N 19000 6400 19800 6400 4
{
T 19000 6500 5 10 1 1 0 0 1
netname=RTC_VBAT
}
N 19000 6800 19800 6800 4
{
T 19000 6900 5 10 1 1 0 0 1
netname=RTC_SDA
}
N 19000 7200 19800 7200 4
{
T 19000 7300 5 10 1 1 0 0 1
netname=RTC_SCL
}
N 3800 7600 4600 7600 4
{
T 3800 7700 5 10 1 1 0 0 1
netname=RTC_SDA
}
N 3800 8000 4600 8000 4
{
T 3800 8100 5 10 1 1 0 0 1
netname=RTC_SCL
}
C 21000 4000 1 0 0 battery-1.sym
{
T 21000 5200 5 10 1 1 0 0 1
refdes=B1
T 21000 8000 5 10 0 1 0 0 1
device=BATTERY
T 21000 5000 5 10 1 1 0 0 1
value=CR2032 3V primary
T 21000 8400 5 10 0 1 0 0 1
battery_policy=nonrechargeable; no charge path
}
N 21000 4200 20200 4200 4
{
T 20200 4300 5 10 1 1 0 0 1
netname=RTC_VBAT
}
N 21700 4200 22500 4200 4
{
T 21700 4300 5 10 1 1 0 0 1
netname=GND
}
C 21000 9000 1 0 0 resistor-1.sym
{
T 21000 9800 5 10 1 1 0 0 1
refdes=R1
T 21000 13000 5 10 0 1 0 0 1
device=RESISTOR
T 21000 9600 5 10 1 1 0 0 1
value=4.7k
T 21000 13400 5 10 0 1 0 0 1
design_status=initial I2C pull-up; verify rise time with bus capacitance
}
N 21000 9100 20200 9100 4
{
T 20200 9200 5 10 1 1 0 0 1
netname=3V3
}
N 21900 9100 22700 9100 4
{
T 21900 9200 5 10 1 1 0 0 1
netname=RTC_SDA
}
C 21000 10400 1 0 0 resistor-1.sym
{
T 21000 11200 5 10 1 1 0 0 1
refdes=R2
T 21000 14400 5 10 0 1 0 0 1
device=RESISTOR
T 21000 11000 5 10 1 1 0 0 1
value=4.7k
T 21000 14800 5 10 0 1 0 0 1
design_status=initial I2C pull-up; verify rise time with bus capacitance
}
N 21000 10500 20200 10500 4
{
T 20200 10600 5 10 1 1 0 0 1
netname=3V3
}
N 21900 10500 22700 10500 4
{
T 21900 10600 5 10 1 1 0 0 1
netname=RTC_SCL
}
C 21000 1500 1 0 0 capacitor-1.sym
{
T 21000 2600 5 10 1 1 0 0 1
refdes=C1
T 21000 5500 5 10 0 1 0 0 1
device=CAPACITOR
T 21000 2400 5 10 1 1 0 0 1
value=100nF
T 21000 5900 5 10 0 1 0 0 1
placement=close to U3 VCC and GND
}
N 21000 1700 20200 1700 4
{
T 20200 1800 5 10 1 1 0 0 1
netname=3V3
}
N 21900 1700 22700 1700 4
{
T 21900 1800 5 10 1 1 0 0 1
netname=GND
}
T 15000 1000 5 10 1 1 0 0 1
DS3231: integrated crystal; 3V3 main supply; primary coin-cell backup.
T 15000 600 5 10 1 1 0 0 1
Pins 5-12 NC must be grounded. Optional 32kHz, INT/SQW, RST unused.
