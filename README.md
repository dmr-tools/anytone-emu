# AnyTone emulator

This little program emulates several AnyTone devices, such that codeplugs written by the 
manufacturer CPS, can be captured and analyzed. For the maximum convenience, this tool either 
generates a pair of pseudo-terminals (under Linux), that can be exposed as COM ports to wine. This 
allows for running the manufacturer CPS in wine, eliminating the need for a Windows VM. 
Alternatively, it opens a serial interface (one of a pair of so-called virtual null-modem cables) 
under windows and emulates an AnyTone device there. 


## Build & install
This tool is build with Qt 6 using QtSerialPort to talk to the serial port on Windows and Linux. You 
will need these development packages to build anytone-emu. The build tool is cmake, so that one is 
needed too. To prepare icons, `rsvg-convert` is needed. 


## Codeplug catalogs 
Devices, firmware and actually the codeplug structure is described in a collection of huge XML 
files, the codeplug catalog. You can clone that catalog from https://github.com/dmr-tools/codeplugs.
You need the path to the `catalog.xml` to run the emulation, it contains all devices and firmware 
definitions, that can be emulated.

To list all devices, defined in the catalog, simply run 
```
 > anytone-emu PATH_TO_CATALOG/catalog.xml 
```
The device to emulate is specified as the second argument. I.e.,
```
 > anytone-emu PATH_TO_CATALOG/catalog.xml d878uv [...] 
```
By default, the latest firmware (found in the catalog) will be emulated. You can specify the 
firmware explicitly. I.e., 
```
 > anytone-emu PATH_TO_CATALOG/catalog.xml d878uv --firmware=3.08 [...]
```
To get a list of all firmware versions defined for this device, run
```
 > anytone-emu PATH_TO_CATALOG/catalog.xml d878uv --firmware=? [...]
```


## Usage under wine

For example, the call 
```
 > anytone-emu PATH_TO_CATALOG/catalog.xml dmr6x2uv pty 
```
will create a pseudo-terminal and connect the emulation of a BTECH DMR-6X2UV to that. This PTY is 
then linked to `~/.local/share/anytone-emu/anytoneport`. From there, your CPS can access it through
wine.

To expose this PTY to applications running under wine, run
```
 > wine regedit
```
And add a string-value to the key `HKEY_LOCAL_MACHINE\Software\Wine\Ports`. The name is then (e.g.)
`COM1` while the value should point to the linked PTY. `$HOME/.local/share/anytone-emu/anytoneport`,
where `$HOME` should the absolute path to you home directory.

Then, you may need to restart the wineserver with 
```
 > wineserver -k
```

Then start the CPS application using wine and select the configured COM port as the interface to 
the device within the CPS.

### CPSs under wine
There are several CPSs, which do not appear to run easily under wine. This table collects which 
CPSs appear to work with wine.

 | CPS                  | Version | Wine 6     | Wine 8     | Remark                           |
 | ---------------------| ------- | ---------- | ---------- | -------------------------------- |
 | AnyTone D868UVE      | 1.40    | not tested | yes        |                                  |
 | AnyTone D878UV       | 3.02n   | no         | crashes    |                                  |
 | AnyTone D878UV II    | 3.02n   | not tested | crashes    |                                  |
 | AnyTone D578UV (II)  | 1.18    | not tested | crashes    |                                  |
 | BTECH DMR-6X2UV      | 2.04    | not tested | yes        |                                  |
 | BTECH DMR-6X2UV Pro  | 1.01k   | yes        | not tested |                                  |
 | Alinco DJ-MD5        | 1.13e   | not tested | yes        |                                  |

## Command line options
```
  > anytone-emu [OPTIONS] catalog [device [interface]]
```
#### Options
  - `--help` -- Obvious help message.
  - `--version` -- Show version number
  - `--loglevel=LEVEL` -- Specifies the log-level. Must be one of `debug`, `info`, `warning`, 
    `error` or `fatal`.
  - `--dump` -- Just dumps the received codeplug as a hex-dump. Either to stdout or to a file, 
    specified by `--output`.
  - `--diff=MODE` -- Dumps only the difference between codeplugs. The `MODE` specifies the codeplugs 
    to be compared. The `first` mode compares each received codeplug with the first one, while 
    `previous` (default) compares the last two received codeplugs.
  - `--output=FORMAT` -- Specifies the sprintf-like file pattern to save received codeplugs to. 
    For every received codeplug, a counter gets increased. E.g., `codeplug_%02d.hex` will produce 
    the file-sequence `codeplug_00.hex`, `codeplug_01.hex`, ...
  - `--firmware=VERSION|?`  Specifies the firmware version to emulate. This usually has no effect 
    on the emulation. If no firmware is specified, the latest found is used. To get a list of all 
    version defined, pass `?`.

The command also takes up to 3 positional arguments. I.e.,
  - `catalog`  -- Specifies the catalog file to use. See http://github.com/dmr-tools/codeplugs/.
  - `device` -- Specifies the device to emulate, e.g. 'd868uv', 'd868uve', 'd878uv', 'd878uv2', 
    'd578uv', 'd578uv2', 'dmr6x2uv', 'dmr6x2uv2', 'djmd5' or 'djmd5x'. For a full list of the 
    devices of the catalog, omit this argument.
  - `interface` -- Specifies the interface to the CPS. This can either be `pty` or the name of a 
    serial interface. If `pty` is set, the emulator will generate a new pseudo terminal. This can 
    then be used to emulate a COM port under wine. Under windows, a serial port of a virtual 
    null-modem must be chosen. The second port of that null-modem is then selected in the CPS.

    
## License
anytone-emu  Copyright (C) 2023 -- 2026  Hannes Matuschek

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
