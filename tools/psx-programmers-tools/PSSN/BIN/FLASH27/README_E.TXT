$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
		DTL-H2700 SDevTC Firmware Kit


Rewrites Firmware

  ROM image is written in the DTL-H2700 

    Syntax: flash <I/O address>

    The <I/O address> specifies the I/O address set in the DTL-H2700.

    Example:
        DOS> cd \pssn\bin\flash27
        DOS> flash 1340

    Note:
    -  When operation is abnormal and the DOS prompt cannot be returned to, 
       press F10 F2. 
    -  Make sure that DEXBIOS is in the 'Remove' state as it is executed.


Switches PAL/NTSC screen modes

  When turning on the system, the DTL-H2700 starts up in NTSC screen mode.
  reset27.exe switches PAL screen mode.
  There is no need to set it again later on.

    Syntax: reset27 [-p|-n] [-a<I/O address>]

    -n  Sets the screen mode to NTSC
    -p  Sets the screen mode to PAL

    If specification is omitted, the previous mode is maintained.

    The -a<I/O address> specifies the I/O address set in the DTL-H2700.
    When the I/O address is not set, the address set in the PC environment 
    variable DEX2000 is used.

    Example
        DOS> reset27 -p -a0x1340

    Example
        DOS> set DEX2000=0x1340
        DOS> reset27 -p


h2700.img(SN) ver1.20
-----------------------------------------------------------------------

Copyright (C) 1996-1998 by Sony Computer Entertainment Inc.
		All Rights Reserved.
