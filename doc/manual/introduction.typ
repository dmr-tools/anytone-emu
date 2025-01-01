= Introduction
#figure(
  image("fig/gui-annotated-dump.png", width: 75%),
  caption: [Annotated hex-dump of the channel bank of a captured codeplug from the #box[DMR-6x2UV] 
    CPS. ]
)



== Setup and getting familiar
The emulator runs under Linux and Windows, all of the CPSs however, only run under windows. Therefore, it is very likely, that you will use `anytone-emu` under windows. I do provide a small installer for windows for the sake of simplicity. Just download and install the latest version. 

Some of these CPSs also run under wine --- a compatibility layer between windows applications and Linux. You may also try to use wine on a Linux machine for the reverse engineering. Below, I describe each setup process separately.


=== Obtain and extract codeplug descriptions
Clone or download the current codeplug descriptions from @codeplugs. They contain the actual device and codeplug patterns, that are already reverse engineered. You will help in the effort to keep them up to date or extend them.

Each codeplug is represented by a huge XML file containing the entire description of a single codeplug format for a specific device and firmware revision. 


=== Windows Setup
The windows setup is quiet simple:
  - Download and install the latest release from @anytone-emu-releases 
  - Install the `com0com` driver from @com0com. This creates a pair of COM ports, that are connected to each other. This way, the CPS will talk to the emulation. The CPS will open one of the two ports, while the emulation will open the other.
  - Install the manufacturer CPS. A driver is not needed.
  - Start `anytone-emu-gui`
    - Select the catalog file from the codeplug descriptions you downloaded before. 
    - Alternatively, you may select `build-in` codeplugs. They are not that up to date an you cannot edit them. They are, however, an easy way to start up and have a look at the emulator.
    - Then select a device and firmware version you want to reverse engineer. If your device or firmware version is not present, consider adding it or aks for help at the codeplug issues @codeplugs-issues. 
    - Select `Serial Port` as interface and one of the COM ports, you created with the com0com drivers. Remember which port you selected (e.g., always the first). You will have to select the other one later.
    - Hit 'Ok'  


=== Linux Setup 
The following steps describe, how a reverse engineering setup is obtained to emulate devices to CPSs running under wine. This includes the setup of an emulated COM port in wine. 

  - Get the sources from @anytone-emu and build them using `cmake`. To build it, you need `cmake`, `Qt6Core`, `Qt6Widgets`, `Qt6SerialPort` as well as `rsvg-convert`.
  - Start `anytone-emu-gui` and select the device and firmware version you want to reverse engineer. 
  - Select `pseudo terminal` as interface and copy the `symlink path` below. It will point to the pty generated to emulate the serial port. It should be something like `$(HOME)/.local/share/anytone-emu/anytoneport`.
  - Run `regedit` under wine, e.g., run `wine regedit` in a terminal.
    - Edit the key `HKEY_LOCAL_MACHINE\Software\Wine\Ports` and add a string value with the name `COM1` (or another number if already assigned) and enter the absolute path to the symlink as the value. 
    - E.g., name `COM1` and value `/home/hannes/.local/share/anytone-emu/anytoneport`.
  - Quit `regedit` and also restart the wineserver with `wineserver -k`.
  - Install the CPS using wine and start it. You should be able to select the COM port, you just created.


== First steps
Before diving into the reverse engineering of the codeplugs, take some time and get familiar with the graphical interface of `anytone-emu-gui`. 

This application combines the device emulation, codeplug analysis and documentation tool into one unified application. So, there is a lot to discover.

Note, it is helpful to work on a larger monitor with a high resolution. You will need that space.

#figure(
  image("fig/gui-overview.png", width: 75%),
  caption: [
    Overview over the graphical user interface. 
  ]
)

The main window is divided into tree parts. The left panel houses the received binary codeplug images under _Images_. There you find all data received from the CPS by the emulation. Each time, a codeplug is written to the emulation, a new image is created. An image may consists of several memory sections called _Elements_. An element is just a continuous segment of memory the CPS has written to.

Also on the left panel, you find the codeplug structure. This structure holds the entire description of the codeplug and can be very complex. This structure is a tree, where the root is the codeplug, containing everything. 

The parent-child relationship can be read as _consists of_ or _contains_. E.g., A codeplug _contains_ channel banks, _consisting of_ single channel bank elements, _consisting of_ channels, _containing_ ...

These structures are called _pattern_. They describe, how the codeplug is assembled. Each pattern may have an absolute or relative address and may have a fixed size.

If an address or size is implicit --- that is, it cannot be changed directly --- it is shown in the side panel in muted colors. You may notice, that addresses and sizes are shown in weird formats. So we should discuss them first.

=== Addressed and Sizes 
Both formats follow the same structure: `BYTE:BIT`, where the bit-part and column is optional. The byte part is expressed in hexadecimal format. You may use a `h` prefix to signify that. E.g., a size of 12 bytes and 3 bits can be expressed as `ch:3`. For sizes, this format is straight forward. 

For addresses, there is a caveat. Consider the address of the first bit right after the first byte. The address would be `1h:7`. The first bit after the first byte is most significant bit of the second byte. Hence byte address `1h` and the most significant bit is bit 7. 

This appears to be difficult to gasp and counter intuitive, but these are the canonical conventions to address bits and bytes. But do not fear confusion, `anytone-emu` will give you a map.

Now, let us continue the tour. The right half of the window is split into two parts. The lower one only houses the log. It can be ignored and hidden by hitting the scroll-button in the toolbar.

The main part, the upper half will house the differences between codeplugs and the aforementioned element maps. 

To see something, fire up the CPS and write some codeplug to the emulator. If everything works, you should find a first image in the image list. Expand it, select an element and hit the _Show hexdump_ toolbutton #box(image("fig/button-show-hexdump.svg")) on top of the list. A new tab with the hexdump of that element should open.

=== Hex dumps
As usual, each line of the hexdump contains up to 16 bytes. On the left, the ASCII representation of printable characters is shown. The address of the line is shown on the left. 

#figure(
  image("fig/gui-hexdump-element.png", width: 75%),
  caption: [Received image and hexdump of the first element of that image. Obviously, it is related to the channel settings.]
)

From that first element of the first image, we learned, that (at least some part) of the channels are encoded at a memory section starting at address `800000h`. We may now proceed to reverse engineer the encoding of a first channel feature. 

Switch back to the CPS, open the first channel and change the RX frequency but remember, what that frequency was before. Here I change it from 435.525 MHz to 432.12345 MHz. We again write that modified codeplug to the emulation and obtain a second image.

If we now select the two images and hit the _show difference_ button #box(image("fig/button-show-diff-single.svg")), We once again obtain a hexdump. This time, however, showing only lines containing a difference between the two codeplugs. 


=== Hex differences
A hex difference is quiet similar to the hex dump but compares two 16-byte lines side-by-side.

#figure(
  image("fig/gui-hexdiff-element.png", width: 75%),
  caption: [
    Hexdiff between two codeplugs highlighting the changes between the two. 
  ], 
) <fig-hexdiff-example>

The left column is always the _old_ codeplug. E.g., the one selected first,while the second column is the _new_ one. The bytes of the _old_ codeplug that have changed are marked red, while the ones changed in the new codeplug are marked green. Unchanged bytes are not highlighted. 

The very first byte did not change, but the next three and later on, four more. We now need to remember, what we have changed. We changed the RX frequency from 435.525 MHz to 432.12345 MHz. If we compare these frequencies with the fist four bytes, we discover, that the frequency digits are represented in hex within the codeplug. Having 4 bytes to store theses digits, we only get 8 digits. Hence, the frequency is stored in multiples of 10Hz. You may have noticed, that this is the allowed resolution in the CPS. 

The _readability_ of the frequencies is not a coincidence. Each hex-digit takes exactly 4 bits. Hence every decimal frequency digit is also stored in 4 bits. This is called BCD (binary coded decimals) and is a very inefficient way to store integers. 

We now reverse engineered the first field of a channel. The RX frequency. Having a look at the channel dialog, let us realize, that we still have a long way ahead of us.


== Differential analysis
What you just performed is called a _differential analysis_. You change one aspect in the CPS and observe, what changes in the memory written to the emulator. Usually, only one or a few bytes change and thus it is easy to correlate the changes made to the memory addresses affected. 

Sometime, your changes have side effects too. An example for that is the change of the RX frequency above shown in @fig-hexdiff-example. The first four bytes encode the RX frequency directly. However, the change of the RX frequency also affected bytes `800005h-800008h`. Again 4 bytes. One may speculate, that this memory section is related to the transmit frequency. Although we did not change it, it is not encoded directly as a BCD number. Maybe, it is encoded as an offset from the RX frequency.

To verify this suspicion, we may write three different codeplugs to the emulator. One where the RX and TX frequencies are equal, one where the TX frequency is a fixed amount above the RX frequency and one last codeplug, where the TX frequency is a fixed amount below the RX frequency. One obtains

#table(
  columns: (2fr, 2fr, 3fr),
  table.header(
    [RX Frequency], [TX Frequency], [Offset 0-8]
  ),
  [432.12345], [432.12345], [`43 21 23 45 00 00 00 00 1c`], 
  [432.12345], [433.12345], [`43 21 23 45 00 10 00 00 5c`],
  [432.12345], [431.12345], [`43 21 23 45 00 10 00 00 9c`]
)

In the first case, the offset is 0, and for the other two cases it is $plus.minus 1$Mhz. Obviously, the offset is encoded as 4 BCD digits in the bytes 4-7. The direction of the frequency offset, however, is stored differently. The only byte left changing with the frequency offset is byte 8. The least significant 4 bits are unchanged. Only the most significant bits change. We find 

#table(
  columns: (1fr, 1fr, 1fr),
  table.header(
    [Direction],[Hex value],[Binary value]
  ),
  [None], [`1ch`], [`00011100b`],
  [Positive], [`5ch`], [`01011100b`],
  [Negative], [`9ch`], [`10011100b`]
)

It is obvious, that the lest significant 6 bits remain constant `011100b`. We still don't know their meaning. The two most significant bits, however, are now known. They encode the direction of the transmit frequency offset. 

To summarize we found the following fields within a channel 
#table(
  columns: (1fr, 1fr, 5fr, 5fr),
  table.header(
    [Offset], [Size], [Type], [Description]
  ),
  [`00h`], [`4h`], [Unsigned integer, BCD, big endian], [RX frequency in 10Hz resolution.],
  [`04h`], [`4h`], [Unsigned integer, BCD, big endian], [TX frequency offset in 10Hz resolution.],
  [`08h:6`], [`:2`], [Enumeration], [Offset direction, 0=None, 1=Positive, 
  2=Negative],
  [`08h:0`], [`:6`], [Unknown], [Some data, `1ch`.],
)

With only a few changes in the CPS, we covered some significant portion (`8h:2`) of the channel element (size `40h`). We even managed to resolve some side effects. In my experience, these side effects are relatively rare and therefore, the reverse engineering proceeds quiet smoothly. 

How do we know, that the channel is `40h` bytes in size? There is a simple trick. Once more, have a look at the hexdump of the first element of an image. There, we discovered the channels. We still don't know where a channel starts or ends. However, we see, that the start of one channel name and the start of the next channel name is exactly `40h` bytes apart (4 lines, same column). Consequently, the offset from one channel to the next is `40h`. This does not necessarily means, that each channel is exactly `40h` bytes in size, but it is reasonable to assume it.

== Documenting the codeplug structure
Once we reverse engineered some of the codeplug structure end field encoding, it is time to document what we have found. 