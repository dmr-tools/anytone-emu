# AnyTone emulator

This little program emulates several AnyTone devices, such that codeplugs written by the 
manufacturer CPS, can be captured and analyzed. For the maximum convenience, this tool generates
a pair of pseudo-terminals, that can be exposed as COM ports to wine. This then, allows for running
the manufacturer CPS in wine, eliminating the need for a windows VM. 


## Build & install
Being a pure Rust application, the build and install process is straigt forward. First, install 
`cargo`, the Rust library manager. E.g.
```
> sudo apt-get install cargo 
```

Then checkout the sources and enter the directory. Then simply call
```
anytone-emu> cargo run -- --radio=dmr6x2uv2 
```


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

To expose this PTY to applicaions running under wine, run
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


## Anytone protocol
The AnyTone protocol follows a strict request/response pattern. The host (computer) sends a request 
to the device, which will send a response back. 


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
