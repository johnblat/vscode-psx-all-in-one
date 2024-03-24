# PlayStation (PSX) Development Getting Started Guide
:exclamation: :exclamation: Please **submit and issue** for any questions/clarifications on the contents, or **create a PR** with any changes that will make the guide easier to follow. It is especially valuable to have someone completely new to PSX development raise any issues. :exclamation: :exclamation:
## Modern PSX Development Overview
The year is 2024, and PSX development has never been easier and more.... modern (:question: :grey_question: :question:)

You will use a combination of new tools, old tools, a lot of documentation, and a converted version of the official sony PSX software development kit called **PsyQ**.

This guide will cover the following "Getting Started" topics:
- Setting up your development environment
- Running a disc image in the PCSX-Redux emulator
- Compiling a PSX executable
- Debugging a PSX executable
- Creating a PSX disc image with your program on it
- Running your program on a PSX console
- Soft modding a PSX console

### Notes
> :exclamation: This guide and repo was written with Windows as the development environment in mind. If you use Linux or MacOS and use this guide, please make changes to it and submit a PR providing updates for those Operating Systems. :exclamation:

> :skull: Remember kid, pirating games is illegal. Don't even think about illegally downloading a ROM to play in a PSX emulator. Only use games you own. :skull:
> 
> :oncoming_police_car: If you can do the crime, you can do the time. :oncoming_police_car: 

### Terms I will use in this guide
- __PSX Alternative Media (PAM)__: Any media that is not an official region-specific software on a PSX CD-ROM. This includes **homebrew content**, personal **backups** of original PSX games, and official discs from **other regions**. 
- **PSX Alternative Storage (PAS)**: Any storage medium that is not a CD-ROM. This includes SD cards, and a PC connected to the PSX via a serial cable.
- **Homebrew Media**: Media made by non-official developers for the PSX. This includes games, demos, and other software.
- **Backups**: A copy of a game that you own. Typically, a copy burned onto a CD-R disc.

### Software
These tools are included in this repository. If you wish to clone or download this repository, you will have everything you need to get started. I will also provide download links if you want to download them yourself. 
- **VS Code**
    - As far as I know, most PSX Homebrew developers use VS Code as their text editor of choice. I've provided VS Code debug and task configurations in this repository that will help you build and debug your PSX programs. 
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
- **Blank CD-R Discs**
    > :pencil2: The PSX can not run CD-RW discs. Only CD-R will work.
- **PSX Memory Card with FreePSXBoot + Unirom8**
    - Will be used for launching your program!
- **USB stick with Memory Card Annihilator and FreePSXBoot**
    - Will be used to format a PSX memory card and load FreePSXBoot onto it.
- **PS2 Memory Card with FreeMcBoot**
    - Will be used to load the software in the USB stick mentioned above.
- **Playstation 2 Console**
    - Used to format a PSX memory card and install FreePSXBoot on it. 
    - A PS2 is used because it is a device that contains both a PSX memory card slot and a USB port. If you don't use a PS2, you would need a PSX memory card reader/writer that could be used with your PC. This would involve buying a specialized device, or DIYing one yourself.
- **Disc Reader/Writer**
    - Used to burn CD-R discs with your program on it.
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
    - Sometimes you'll want to create a disc image without using the Makefile (which contains rules for building to a disc image)
    - This allows you to dump contents of a disc image for debugging purposes if there are issues running it


> :pencil2: You can put the other tools on your path too, but these are the ones I found I use on the command line.

### References
- [Github of PsyQ examples](https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main)
    - Contains examples of how to use the PsyQ SDK to create PSX programs.
    - I will reference this guide for learning various things that will help you get a running program on the PSX.
- [Lameguy's PlayStation Programming Series](http://lameguy64.net/tutorials/pstutorials/)
    - An unfinished/work-in-profress series of tutorials on PSX development.


## Running a Disc Image in the PCSX-Redux Emulator
1. Create a backup of a PSX game you own with a Disc Reader, or obtain a disc image of a PSX game through some other legal means
1. Launch `pscx-redux`
1. Go to `File/Open Disk Image` and then select the `.iso`, `.bin`, or `.cue` file of the game you want to run
    - You can also load the disk image from the command line with the `-iso` argument. Example: `pscx-redux -cdrom "path/to/game.iso"`
    >  :pencil2: Disc images in the ISO-9660 format will either be stored in an `.iso` file or a `.bin` file. The `.bin` file will have a `.cue` file that contains information about the `.bin` file. The `.cue` file is used to load the `.bin` file into the emulator.
1. Go to `Emulation/Start` to start the game, or press `F5`.
    - You can also run the disk image from the command line with the `-run` argument. Example: `pscx-redux -iso "path/to/game.iso" -run`
    

    > :pencil2: You can view the entire list of PCSX-Redux CLI Flags [here](https://pcsx-redux.consoledev.net/cli_flags/)

## Compiling a PSX Executable
