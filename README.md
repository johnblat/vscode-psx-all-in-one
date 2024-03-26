# PSX Project Building, Running, and Debugging
---
:exclamation: :exclamation: Please **submit and issue** for any questions/clarifications on the contents, or **create a PR** with any changes that will make the guide easier to follow. It is especially valuable to have someone completely new to PSX development raise any issues. :exclamation: :exclamation:

## Table of Contents
1. [Modern PSX Development Overview](#modern-psx-development-overview)
1. [Running a disk image in the PCSX-Redux Emulator](#running-a-disk-image-in-the-pcsx-redux-emulator)
1. [Quick Start Guide](#quick-start-guide)
1. [Compilation and Linking](#compilation-and-linking)
1. [Disk Image Creation](#disk-image-creation)
1. [PCSX-Redux Debugging](#pcsx-redux-debugging)
1. [Soft Modding](#soft-modding)










---
## Modern PSX Development Overview

The year is 2024, and PSX development has never been easier and more.... modern (:question: :grey_question: :question:)

You will use a combination of new tools, old tools, a lot of documentation, and a converted version of the official sony PSX software development kit called **PsyQ**.


> :exclamation: This guide and repo was written with Windows as the development environment in mind. If you use Linux or MacOS and use this guide, please make changes to it and submit a PR providing updates for those Operating Systems. :exclamation:

 

### Terms I will use in this guide
- __PSX Alternative Media (PAM)__: Any media that is not an official region-specific software on a PSX CD-ROM. This includes **homebrew content**, personal **backups** of original PSX games, and official disks from **other regions**. 
- **PSX Alternative Storage (PAS)**: Any storage medium that is not a CD-ROM. This includes SD cards, and a PC connected to the PSX via a serial cable.
- **Homebrew Media**: Media made by non-official developers for the PSX. This includes games, demos, and other software.
- **Backups**: A copy of a game that you own. Typically, a copy burned onto a CD-R disk.

### Software
These tools are included in this repository. If you wish to clone or download this repository, you will have everything you need to get started. I will also provide download links if you want to download them yourself. 
- **VS Code**
    - As far as I know, a lot of prominent PSX Homebrew developers use VS Code as their text editor of choice. I've provided VS Code debug and task configurations in this repository that will help you build and debug your PSX programs. 
- **PCSX-Redux Emulator**:
    - A PSX emulator that can run PAM, PAS.
    - [Download page](https://github.com/grumpycoders/pcsx-redux?tab=readme-ov-file#where)
- **GnuWin32/bin/make.exe**:
    - A Windows make port that can be used to build the code with the Makefile provided.
    - [Download page](https://gnuwin32.sourceforge.net/packages/make.htm). Install "Complete package, except sources" if you don't know which one to download
    - Alternatively, and probably the recommended approach is to use this installation method using the [`pcsx-redux` provided mips installation script](https://github.com/ABelliqueux/nolibgs_hello_worlds?tab=readme-ov-file#windows)
- **mipsel-none-elf-gcc-11.2.0.exe**:
    - A mips compiler that can be used to compile the project for targeting PSX.
    - [Download page](https://static.grumpycoder.net/pixel/mips/)
- **gdb-multiarch**:
    - A gdb client that can connect to the pscx-redux emulator gdb server.
    - [Download page](https://static.grumpycoder.net/pixel/gdb-multiarch-windows/)
    - Same as the mips compiler, it can alternatively use the [`pcsx-redux` provided mips installation script](https://github.com/ABelliqueux/nolibgs_hello_worlds?tab=readme-ov-file#windows)
-  **mkpsxiso-2.04-win64.exe**:
    - A tool that can be used to create an ISO from the compiled project and any assets.
    - [Download Page](https://github.com/Lameguy64/mkpsxiso/releases/tag/v2.04)
- **Memory Card Annihilator**:
    - A tool that can be used to format a memory card for use with FreePSXBoot.
    - [Download Page](https://www.psx-place.com/threads/memory-card-annihilator-v2-0a-a-new-version-after-more-than-11-years.36277/)
    - [Other Download Page](https://gbatemp.net/download/memory-card-annihilator.35971/)
- **FreePSXBoot**: 
    - A software that gets loaded onto a memory card, and allows the PSX to run PAM and PAS.
    - [Download Page](https://github.com/brad-lin/FreePSXBoot/releases/tag/v2.1)

- **Tim Edit**:
    - A tool that can be used to convert TIM image files to a format that can be used with the PSX. Not the original PSX tool.
    - [Download Page](https://github.com/Lameguy64/TIMedit/releases/tag/0.10a)
     
- **TimTool**:
     - Software for the original PSX development that can be used to create TIM image files (PSX image file format) and layout.
     - [Download Page](https://psx.arthus.net/sdk/Psy-Q/)


### Equipment

- **PlayStation Console**
    - To run your program on it :p
    - Will need to be either hard or soft modded to run your program. This guide will focus on soft modding, as that is the method I have done, but I'll provide links to hard modding resources.
- **Blank CD-R disks**
    > :pencil2: The PSX can not run CD-RW disks. Only CD-R will work.
- **PSX Memory Card with FreePSXBoot + Unirom8**
    - Will be used for launching your program!
- **USB stick with Memory Card Annihilator and FreePSXBoot**
    - Will be used to format a PSX memory card and load FreePSXBoot onto it.
- **PS2 Memory Card with FreeMcBoot**
    - Will be used to load the software in the USB stick mentioned above.
- **Playstation 2 Console**
    - Used to format a PSX memory card and install FreePSXBoot on it. 
    - A PS2 is used because it is a device that contains both a PSX memory card slot and a USB port. If you don't use a PS2, you would need a PSX memory card reader/writer that could be used with your PC. This would involve buying a specialized device, or DIYing one yourself.
- **disk Reader/Writer**
    - Used to burn CD-R disks with your program on it.
    - Can be used to make a copy of a PSX game you own to test that it works on a modded PSX.
> :pencil2: Creating or obtaining the FreeMcBoot memory card, the USB stick contents, and the FreePSXBoot memory card will be covered later

### Playstation Software Development Kit (PsyQ)
The headers and library binaries are under `thirdparty\nugget\psyq\` in this repository. This is a semi-official SDK for PSX development. It has been converted from the original PsyQ SDK to work with modern PCs and toolchains.

### Put some tools on your PATH
Some tools in the repository might be better put on your path for launching via the command line without typing the entire/relative path form this project's root. 
- `tools\GnuWin32\bin`
    - Contains `make`
    - Sometimes you'll want to build without using VS Code
- `tools\pcsx-redux\`
    - Sometimes you'll need to run the emulator with command line arguments such as `-pcdrv` and `-pcdrvbase`, which will be covered later
- `tools\mkpsxiso-2.04-win64\bin`
    - Sometimes you'll want to create a disk image without using the Makefile (which contains rules for building to a disk image)
    - This allows you to dump contents of a disk image for debugging purposes if there are issues running it


> :pencil2: You can put the other tools on your path too, but these are the ones I found I use on the command line.

### References
- [Github of PsyQ examples](https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main)
    - Contains examples of how to use the PsyQ SDK to create PSX programs.
    - I will reference this guide for learning various things that will help you get a running program on the PSX.
- [Lameguy's PlayStation Programming Series](http://lameguy64.net/tutorials/pstutorials/)
    - An unfinished/work-in-profress series of tutorials on PSX development.








---
## Running a disk image in the PCSX-Redux Emulator

If you are unfamiliar with using `pcsx-redux`, or emulators in general, it's helpful to first run a disk image in the emulator to get a feel for how it works. 

1. Create a backup of a PSX game you own with a disk reader, or obtain a disk image of a PSX game through some other legal means
    > :skull: Remember kid, pirating games is **illegal**. Don't even dream about illegally downloading a game to play in a PSX emulator. Only use games you own. :skull:
    > 
    > :oncoming_police_car: If you can do the crime, you can do the time. :oncoming_police_car:
1. Launch `pscx-redux`
1. Go to `File/Open Disk Image` and then select the `.iso`, `.bin`, or `.cue` file of the game you want to run
    - You can also load the disk image from the command line with the `-iso` argument. Example: `pscx-redux -cdrom "path/to/game.iso"`
    >  :pencil2: disk images in the ISO-9660 format will either be stored in an `.iso` file or a `.bin` file. The `.bin` file will have a `.cue` file that contains information about the `.bin` file. The `.cue` file is used to load the `.bin` file into the emulator.
1. Go to `Emulation/Start` to start the game, or press `F5`.
    - You can also run the disk image from the command line with the `-run` argument. Example: `pscx-redux -iso "path/to/game.iso" -run`


    

    > :pencil2: You can view the entire list of PCSX-Redux CLI Flags [here](https://pcsx-redux.consoledev.net/cli_flags/)







---
## Quick Start Guide

### Building and Running a PSX Program
1. In the project directory, run `make`
    - This will compile the project and create a `.ps-exe` file that can be ran in the emulator
1. Launch `pcsx-redux`
1. Go to `File/Load Binary` and select the `.ps-exe` file that was created in the previous step
1. Go to `Emulation/Start` to start the program, or press `F5`


`pcsx-redux` on the command line: `pcsx-redux -exe "path/to/program.ps-exe" -run`
### Debugging in VS Code
1. Launch `pscx-redux`
1. Go to `Configuration/Emulation`
    - Check
        - Enable Debugger
        - Enable GDB Server
    - Uncheck
        - Dynarec CPU
1. Reboot `pscx-redux` if you needed to uncheck the Dynarec CPU

`pcsx-redux` on the command line: `pcsx-redux -gdbserver -debugger -exe "path/to/program.ps-exe" -run`

### Using the Makefile
- If you'd like to copy and paste this Makefile into your own project, you can add more sources to the `SRCS` variable.

The above will get you started with building, running, and debugging programs. If you are interested in knowing more about the details in the process, read on...














---
## Compilation and Linking

The PSX has a MIPS R3000 CPU, therefore the code must be cross-compiled with a MIPS compiler. Classic PSX executable (ps-exe) compilation was done with the official `ccpsx` compiler, however, other compilers like `mipsel-none-elf-gcc` or `mipsel-linux-gnu-gcc` can be used to compile PSX executables on modern 64-bit workstations. In addition, a custom converted version of the official Sony PSX SDK called **PsyQ** is used to provide the necessary libraries that can be used by modern compilers.

PSX executables are statically linked, as opposed to dynamically linked. Click [here](https://www.linkedin.com/pulse/differences-between-static-dynamic-libraries-juan-david-tuta-botero/) for a more information on the difference between the two linking methods. Static linking is very common on embedded systems, as there may not be a loader program to load shared libraries at runtime, as is the case on destkop operating systems like Windows, Linux, and MacOS.

### Compiler Options used in the Makefile
- Unless otherwise specified, the docs for options are [here](https://gcc.gnu.org/onlinedocs/gcc/MIPS-Options.html)
- flags:
    - **-march=mps1**:
        - Generate code that runs on [MIPS I R3000 architecture](https://en.wikipedia.org/wiki/MIPS_architecture#MIPS_I)
    - **-mabi=32**: 
        - Generate code for 32-bit [ABI](https://en.wikipedia.org/wiki/Application_binary_interface)
    - **-EL**:
        > Generate [little-endian](https://en.wikipedia.org/wiki/Endianness) code. This is the default for ‘mips*el-*-*’ configurations.
    - **-fno-pic**: 
        > Disables the generation of [position independent](https://en.wikipedia.org/wiki/Position-independent_code) code with relative address references, which are independent of the location where your program is loaded.
        - The reason for this when compiling to the PSX is that the PSX has a fixed memory map, and the code will always be loaded at the same address. Position Independet code is typically used when the code can be loaded at any address in memory, typically when the code is a shared library. PSX executables don't contain shared libraries, so this flag is used.
        - :exclamation: not found in the docs above
    - **-mno-shared**: 
        - Do not generate code that is fully position-independent, and that can therefore not be linked into shared libraries. This option only affects -mabicalls/-mno-abicalls.
        > All -mabicalls code has traditionally been position-independent, regardless of options like -fPIC and -fpic. However, as an extension, the GNU toolchain allows executables to use absolute accesses for locally-binding symbols. It can also use shorter GP initialization sequences and generate direct calls to locally-defined functions. This mode is selected by -mno-shared.
    - **-mno-abicalls**: 
        > Do not generate code that is suitable for [SVR4](https://en.wikipedia.org/wiki/UNIX_System_V#SVR4)-style dynamic objects. -mabicalls is the default for SVR4-based systems.
    - **-mfp32**: 
        > Assume that floating-point registers are 32 bits wide.
    - **-fno-stack-protector** 
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Optimize-Options.html)
        - Opposite of:
        > -fstack-protector
        >      Emit extra code to check for buffer overflows, such as stack smashing attacks. This is done by adding a guard variable to functions with vulnerable objects. This includes functions that call alloca, and functions with buffers larger than 8 bytes. The guards are initialized when a function is entered and then checked when the function exits. If a guard check fails, an error message is printed and the program exits.
    - **-nostdlib**:
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Link-Options.html)
        >Do not use the standard system startup files or libraries when linking. No startup files and only the libraries you specify will be passed to the linker. The compiler may generate calls to memcmp, memset, memcpy and memmove. These entries are usually resolved by entries in libc. These entry points should be supplied through some other mechanism when this option is specified.
    - **-ffreestanding**
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html)
        > Assert that compilation targets a freestanding environment. This implies -fno-builtin. A freestanding environment is one in which the standard library may not exist, and program startup may not necessarily be at main. The most obvious example is an OS kernel. This is equivalent to -fno-hosted.
    - **-ffunction-sections**
        - [Reference](https://developer.arm.com/documentation/101754/0622/armclang-Reference/armclang-Command-line-Options/-ffunction-sections---fno-function-sections?lang=en)
        > -ffunction-sections generates a separate ELF section for each function in the source file. The unused section elimination feature of the linker can then remove unused functions at link time.
    - **-mno-gpopt** 
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc/MIPS-Options.html)
        > Do not use GP-relative accesses for symbols that are known to be in a small data section; see -G, -mlocal-sdata and -mextern-sdata. -mgpopt is the default for all configurations.
        - More information about the `$gp` register can be found [here](https://training.mips.com/basic_mips/PDF/Assemble_Language.pdf) 
    - **-fomit-frame-pointer**
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Optimize-Options.html)
        > Don't keep the frame pointer in a register for functions that don't need one. This avoids the instructions to save, set up and restore frame pointers; it also makes an extra register available in many functions. It also makes debugging impossible on some machines.
    - **-fno-builtin** 
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html)
        > Don’t recognize built-in functions that do not begin with ‘__builtin_’ as prefix. See Other built-in functions provided by GCC, for details of the functions affected, including those which are not built-in functions when -ansi or -std options for strict ISO C conformance are used because they do not have an ISO standard meaning.
    - **-fno-strict-aliasing** 
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gnat_ugn/Alphabetical-List-of-All-Switches.html)
        > Causes the compiler to avoid assumptions regarding non-aliasing of objects of different types
    - **-Wno-attributes**
        - [Reference](https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Warning-Options.html)
        > Do not warn if an unexpected __attribute__ is used, such as unrecognized attributes, function attributes applied to variables, etc. This will not stop errors for incorrect use of supported attributes.

### Libraries

The following libraries will be linked into the executable if their functionality is needed in the target executable:
- `libapi.a`
- `libc.a`
- `libc2.a`
- `libcard.a`
- `libcomb.a`
- `libds.a`
- `libetc.a`
- `libgpu.a`
- `libgs.a`
- `libgte.a`
- `libgpu.a`
- `libgun.a`
- `libhmd.a`
- `libmath.a`
- `libmcrd.a`
- `libmcx.a`
- `libpad.a`
- `libpress.a`
- `libsio.a`
- `libsnd.a`
- `libspu.a`
- `libtap.a`
- `libcd.a`

These libraries can be found in the `thirdparty/nugget/psyq/lib` directory. They are `.a` (archive) objects, which implies a static library, as opposed to a shared/dynamic library. A shared library would have the `.so` (shared object) extension. These file extensions for the library types are also used on MacOS and Linux. For Windows Libraries, these would be equivalent to `.lib` and `.dll` (dynamically linked library) files, respectively.

A description of these libraries can be found in the PSX Documentation found in `docs/LibRef47.pdf` and `docs/LibOver47.pdf`.

> :pencil2: You may notice a library `libsn.a` under the psyq libraries. This library enables PC at `thirdparty/nugget/common/kernel/pcdrv.h` can be used in it's place.


### Linking Scripts
> The main purpose of the linker script is to describe how the sections in the input files should be mapped into the output file, and to control the memory layout of the output file. However, when necessary, the linker script can also direct the linker to perform many other operations, using the linker commands
[More information about Linker Scripts](https://users.informatik.haw-hamburg.de/~krabat/FH-Labor/gnupro/5_GNUPro_Utilities/c_Using_LD/ldLinker_scripts.html#:~:text=The%20main%20purpose%20of%20the,operations%2C%20using%20the%20linker%20commands.)
- `ps-exe.ld`
    - This script is specifically tailored for creating executables that run on the original PlayStation (PS1) console. The PS1 has specific requirements for the executable format and memory layout, which this script addresses.
    - Defines several memory regions according to the PSX's architecture:
        - loader
            - `0x8000F800`
            - Used for .psx-exe header data
        - ram (rwx)
             - `0x80010000`
        - dcache (data cache)
            - `0x1F800000`
            - 1K of Scratchpad (D-Cache used as Fast RAM). [Reference](https://www.chibialiens.com/mips/psx.php)
- `default.ld`
    - sets the heap base to the bss end.
    - the heap will be used when dynamic allocations are made with `malloc` functions
    - the bss is the uninitialized data segment memory









---
## Disk Image Creation

PSX Disks use the ISO-9660 format, and are typically stored in a `.bin` with an accopmanying [.cue file](https://en.wikipedia.org/wiki/Cue_sheet_(computing)). 

1. Obtain a License File by downloading the a non-nugget version of the PsyQ SDK [here](https://www.psxdev.net/downloads.html) 
    - The license files will be found in the `PSYQ_SDK/psyq/cdgen/LCNSFILE` directory.
    - Make note of where the license files are located, as they will be used when creating Disk Images. I put the license files in a directory called `LCNSFILE` at the root of my project directory.
1. Compile the project to a `.ps-exe` file
1. Create an `isoconfig.xml` ([example](https://github.com/Lameguy64/mkpsxiso/blob/master/examples/example.xml)) file to describe the disk image. This file will be used to create the disk image with the `mkpsxiso` tool.
    - During this step, fill in the sections with the appropriate values for your project. Be sure to include the license file information in the `<license>` tag
1. Run `mkpsxiso.exe -y isoconfig.xml`
    - If there is an issue with the generated disk image not working as intended, you can use the program `dumpsxiso.exe` to dump the contents of the disk image to help debug the issue.
1. Follow the steps in the [Running a disk image in the PCSX-Redux Emulator](#running-a-disk-image-in-the-pcsx-redux-emulator) section to run the disk image in the emulator to ensure it works.

---
## PCSX-Redux Debugging


1. Launch `pscx-redux`
1. Go to `Configuration/Emulation`
    - Check
        - Enable Debugger
        - Enable GDB Server
    - Uncheck
        - Dynarec CPU
1. Reboot `pscx-redux` if you needed to uncheck the Dynarec CPU
    > :pencil2: `pcsx-redux` on the command line: `pcsx-redux -gdbserver -debugger -exe "path/to/program.ps-exe" -run`
1. Launch project in VS Code
1. Install the following VS Code extensions if they are not already installed:
    - [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
    - [Native Debug](https://marketplace.visualstudio.com/items?itemName=webfreak.debug)
1. Use the following files in the `.vscode` directory to debug the program. Replace the `<placeholders>` with actual values 
    - `launch.json`
    ```json
    {
        "version": "0.2.0",
        "configurations": [
        
        {
            "type": "gdb",
            "request": "attach",
            "name": "(gdb) Launch GDB (Redux)",
            "target": "localhost:3333",
            "remote": true,
            "stopAtConnect": true,
            "executable": "./<executable name>.elf",
            "windows": {
                "gdbpath": "${workspaceFolder}/tools/gdb-multiarch-12.1/bin/gdb.exe",
            },
            "cwd": "${workspaceRoot}",
            "autorun": [
                "set substitute-path /project .",
                "set heuristic-fence-post 0",
                "monitor reset shellhalt",
                "load ./<executable name>.elf",
                "tbreak main",
            ],
            "valuesFormatting": "parseText",
            "preLaunchTask": "Build Project with make"
            },
        ]
    }
    ```
    - `tasks.json`
    ```json
    {
        "version": "2.0.0",
        "tasks": [
            {
                "type": "shell",
                "label": "Build Project with make",
                "command": "<path/to/make>",
                "problemMatcher": ["$gcc"],
                "group": {
                "kind": "build",
                "isDefault": true
                },
                "detail": "Task generated by Debugger.",
            
            }
        ]
    }
    ```
### Debugging Bugginess
If you remove all breakpoints while running the code, and then add a breakpoint, the code will not stop at the breakpoint. You must pause the debugger and then add a breakpoint. Then you can continue debugging.

---
## Soft Modding

In order to run homebrew media, or any alternative media on the PSX, the PSX must be modded. There are two ways to mod a PSX: soft modding and hard modding. Soft modding inovles utilizing a software exploit to run alternative media. A hard mod involves physically modifying the PSX to run alternative media, typically with a mod chip.

[This video](youtube.com/watch?v=6stVV01V1Hk) covers soft modding a PSX. It shows how to install FreePSXBoot on a PSX memory card using the PS2 FreeMcBoot memory card and a USB stick with the FreePSXBoot software on it.

> :pencil2: When attempting to download the Memory Card Annihilator software mentioned in the video, an error may occur. In that case, you can download it from this [Alternative Link](https://gbatemp.net/download/memory-card-annihilator.35971/)
    

---
## Running a Burned Disk on a real PSX
---
1. Use a software like [ImgBurn](https://www.imgburn.com/index.php?act=download) to burn the generated disk image file from the [Disk Image Creation](#disk-image-creation) section to a blank CD-R disk using a CD/DVD writer. Select the `<image name>.cue` file.
    - There is also the windows `ISOburn.exe` tool that can be used from the command line to burn the disk image. This might be a good way to automate the process via a script.
1. With a `freePSXboot` Memory card, launch the PSX console
1. Insert the burned disk into the PSX
1. Select the menu option to boot the disk.