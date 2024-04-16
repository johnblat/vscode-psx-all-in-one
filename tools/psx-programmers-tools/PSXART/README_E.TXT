$PSDocId: Document Release 1.0 for Run-time Library Release 4.4$
                                                                Oct. 1998

                                          PlayStation(R) Programmer Tool
                                             Runtime Library Release 4.4

                         Tool Changes and New Additions, and Known Bugs
                                                       from Release 4.3   
---------------------------------------------------------------------------
                      Copyright(C) 1998 Sony Computer Entertainment Inc.
                                                    All Rights Reserved.
---------------------------------------------------------------------------

The file names for SGI files are written the way they will appear after 
opening /psxart/sgi/graphics/sgiconv.taz.


Known Bugs and Cautions
---------------------------------------------------------------------------
None.


Existing Tools Improved, Changed and Corrected
---------------------------------------------------------------------------
1. mkmime	1.12     MIMe HMD creator
	x86: /psxart/x86/graphics/mkmime.exe
	SGI: /psxart/sgi/graphics/mkmime

	- An abnormal termination with "assertion failed" bug in
	  very rarely, is fixed.

2. xhmd		1.20     HMD Dis-assembler
	x86: /psxart/x86/graphics/xhmd.exe
	SGI: /psxart/sgi/graphics/xhmd

	- Polygon data are allowed to dis-assemble.
	- Multiple labels for an address can be processed

3. mc32     3.2      Movie Converter
	x86: /psxart/x86/graphics/mc32.exe

    - The malfunction that converted data is not properly if it's image 
      width or height is not multiple of 16, has been fixed.

    - The malfunction that on video+sound dialog, if "CD-ROM speed Normal" 
      is selected, application abort occurs, has been fixed.

4. aiff2vag 3.2      Sound data converter
	x86: /psxart/x86/sound/aiff2vag.exe

    - The malfunction that sometimes noise is caused on loop point, has 
      been fixed.

    - The malfunction that error code is returnd even when conversion is 
      successful, has been fixed.

5. smf2seq  2.4      Sequence data coverter
	x86: /psxart/x86/sound/smf2seq.exe

    - The malfunction that error code is returnd even when conversion is 
      success, has been fixed.


