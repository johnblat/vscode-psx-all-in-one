
# PSX Modding
All media and content **besides an official region-specific PSX CD-ROM** running on PSX hardware will be reffered to as **PSX Alternative Media (PAM)** for any particular PSX model. Any storage medium other than a CD-ROM will be reffered to as **PSX Alternative Storage (PAS)**.

## Why Mod?
Modding allows running a few different types of **PAM**:
- To run homebrew content
- To run personal backups of original PSX games. A backup means a copy of a game that you own. A PSX can not run unoriginal copies of games.
- To run official discs from other regions.

Modding allows loading **PAM** from a few different types of **PAS**:
- SD cards containing PAM loaded into a memory card
- PAM sent to the PSX from a PC via a serial cable
    - This is the most common method for development
    - This is especially useful for debugging and testing

## Types of Modding

There are two main types of mods: a hard mod, and a soft mod. 
- A **hard mod** is a physical modification to the PSX hardware. This involves installing a modchip, which is a small circuit board that is soldered to the PSX motherboard. This modchip allows the PSX to run homebrew discs.
- A **soft mod** is a software modification to the PSX, or using a modified peripheral such as a memory card or a game shark. 

## Soft Mods
- Disc Swap Trick

## Serial Communication
- You must have Unirom

### Memory Card
- https://github.com/ShendoXT/memcarduino

## Misc
- PSX can not run CD-RW discs, only CD-R/CD-ROM, so ensure that you burn release builds of you project to a CD-R disc
- During development, you'll probably want to use a DIY serial cable to transfer your project to the PSX. This is because burning a CD-R every time you want to test your project will take a long time. 