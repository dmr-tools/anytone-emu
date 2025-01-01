= Motivation
AnyTone radios are popular and well supported by the manufacturer. That is, there are frequent firmware updates for theses devices. Also there are many clones of these devices by other manufacturers with their own variants of the firmware. 

Unfortunately no one is giving a toss on backward compatibility between devices. This can be seen as many firmware updates require a backup of the codeplug that must be programmed back to the device after the update. Together this is a nightmare for me to keep up to date with these codeplug formats as they change frequently and completely undocumented. 

This issue might be manageable, if the codeplug would be small. But it isn't. AnyTone devices have an absurd number of settings. So reverse engineering the changes to the codeplug format by any random minor update of the firmware takes ages. 

Fortunately, AnyTone decided to interface their radios using USB CDC-ACM, also known as *serial-over-usb*. Using a so-called virtual null-modem, it is possible to emulate these devices and trick the manufacturer CPS to happily write to the emulation instead of an actual device. This way, it is possible to capture what the CPS writes to the radio and to analyze the written codeplug.

But even with an emulation, reverse engineering the codeplug from scratch every time, the manufacturer releases a new firmware version, is way to cumbersome. It would be great, if there is a tool that is able to 

 + emulate any AnyTone device and clones
 + receive and decode codeplugs
 + mark any incompatibilities between the known codeplug format and the received one
 + provide means to update the codeplug description/documentation

These are the objectives of this project. The `anytone-emu` application tries exactly that. Provide an emulation and helpful tools, that ease the reverse engineering of updated or new codeplug formats.
