## 02-23-2024 

### Question
Hello! I'm working on PSX development with a setup of:
Windows 10
MIPS GCC (Grumpy Coder's toolchain)
the PsyQ SDK.

My executables currently run on PSCX-Redux, but I plan to transition to actual PSX hardware with a custom USB-to-PSX serial link. I've successfully used objcopy for linking assets into the executable but anticipate needing to simulate CD-ROM access for larger projects. How can I best replicate CD-ROM loading in PSCX-Redux? 

Is creating .iso files with each build the recommended approach? Also, when moving to real hardware, can the serial link be used to stream data as if from a CD-ROM? 

Also, if i have any misunderstandings in my above question, or if y'all have better suggestions, I'd appreciate your guidance 🙂 

### Answer
if your goal of simulating cd-rom access is to exercise the codepath, then yes, you'd need to re-create an iso every time, but then in general, streaming data through the hardware sio is an operation generally known as "pcdrv", which Redux supports natively
typically, if you go down this route, the workflow is to have some way of switching between the codepaths of either going through the cd-rom, or through pcdrv
Redux when run with the vscode extension will map local project files to pcdrv, or you can use the command line to achieve the same
and on real hardware, when using unirom + nops in debug mode, the same will happen: nops will map local files to pcdrv


## 02-28-2024

### Question
When using pcdrv enabled in pcsx-redux, do I still use  libcd and it will emulate cd-rom access, or do i use the libsn library which i saw had fileserver functions? Or is there something else i would use to access files from the pc hard drive?

### Answer

I recommend using https://github.com/grumpycoders/pcsx-redux/blob/main/src/mips/common/kernel/pcdrv.h
Header only, no library dependency

### Question
thanks! i got it workin with that header 🙂
what does the sn stand for btw?

### Answer
https://en.wikipedia.org/wiki/SN_Systems