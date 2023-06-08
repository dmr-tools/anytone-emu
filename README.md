# AnyTone emulator

This little program emulates several AnyTone devices, such that codeplugs written by the 
manufacturer CPS, can be captured and analyzed. For the maximum convenience, this tool generates
a pair of pseudo-terminals, that can be exposed as COM ports to wine. This then, allows for running 
the manufacturer CPS in wine, eliminating the need for a windows VM. 

## Usage

For example, the call 
```
 > anytone-emu --wine --port=com2 --radio=d878uv --hexdump 
```
will create a COM2 interface in wine and bind to it, to emulate the AT-D878UV device. The 

## Anytone protocol
The AnyTone protocol follows a strict request/response pattern. The host (computer) sends a request 
to the device, which will send a response back. 

