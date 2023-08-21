# AnyTone emulator

This little program emulates several AnyTone devices, such that codeplugs written by the 
manufacturer CPS, can be captured and analyzed. For the maximum convenience, this tool generates
a pair of pseudo-terminals, that can be exposed as COM ports to wine. This allows for running
the manufacturer CPS in wine, eliminating the need for a windows VM. 


## Build & install
This 

## Usage under wine

For example, the call 
```
anytone-emu> cargo run -- --radio=dmr6x2uv
```
or equivalently, the call showing all default-values
```
anytone-emu> cargo run -- --interface=wine --radio=dmr6x2uv --output=hex --prefix=codeplug_ 
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
wineserver -k
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


## Command line options
  - `--help` -- Obvious help message.
  - `--version` -- Show version number
  - `--interface=IF` -- Specifies the interface to the CPS. This is either `wine` or `comX`. The 
    former will create a PTY pair and link `$HOME/.local/share/anytone-emu/anytoneport` to it. This
    then allows to set this PTY as a COM port in the wine `regedit`. If `anytone-emu` is run under 
    windows, a null-modem emulator (e.g., [com2com](https://com0com.sourceforge.net/)) can be used
    to connect the CPS to the emulator. In this case, the null-modem emulator will create two 
    virtual COM ports. One is passed to `anytone-emu` as `comX` the other is set in the CPS.
  - `--device=DEVICE_ID` -- Specifies which radio to emulate. Must be one of `d868uve`, `d878uv`, 
    `d878uv2`, `d578uv`, `d578uv2`, `dmr6x2uv` or `dmr6x2uv2`.
  - `--output=PATTERN` 

    
## License
anytone-emu  Copyright (C) 2023  Hannes Matuschek

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
