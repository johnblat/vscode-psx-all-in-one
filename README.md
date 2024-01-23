# PSX Self-Contained Development Environment for VS Code on Windows

## What is this?
I created this git repo to help others get up and running with PSX development. It contains everything you need to get started, from the build tools to the PSX emulator itself. It also contains VS Code configuration to use the included tools.

I wanted to create a repo where one could just clone a repository, press `F5` and have code build and running on an emulator. Then they might think "This ain't so bad to get started." 

After that, maybe they can check out the code and start tinkering. 

Maybe they'll even make a game. 

Maybe they'll even make a game that's fun to play. 

Maybe they'll share it with others. 

Maybe they'll have a good time. 

Maybe they'll even make a game that's fun to play and they'll share it with others and they'll have a good time and they'll make a friend. 

Maybe they'll even make a friend and they'll be happy. 

Maybe they'll be so happy they live a long and fulfilling life. 

Maybe they'll even make a game that's so much fun to play and they'll share it with others and they'll have a good time and they'll make a friend and they'll be happy and they'll live a long and fulfilling life and they'll die peacefully in their sleep. 

Maybe they'll be remembered fondly by their loved ones. 

Not such a bad life.

So yeah, that's what this is.

Now, I'm not saying that this repo will help you do all that. I'm just saying that it might.

I'm also not saying that this repo is the best way to get started. It probably isn't. I'm just saying that it's a way to get started.


## Get Started

You can read this list out of order, but I recommend you read it in order.

1. Install [VS Code](https://code.visualstudio.com/) if you don't have it installed on your comptuer already. If you do skip to step 2. 
1. Install the following extensions:
    - [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
    - [Native Debug](https://marketplace.visualstudio.com/items?itemName=webfreak.debug)
1. Run `pscx-redux.exe` located in the `.tools\pscx-redux` folder. 
    - Ensure that the settings for running the gdb server are set correctly:
        - Dynarec turned off
        - GDB Server and Debugging turned on
    - Alternatively run the VS Code task `Launch pscx-redux.exe` which will do exactly what it says.
1. Run `(gdb) Launch GDB (Redux)` in the Debug tab.


## What's Included
- `.\tools\pscx-redux` - PSX Emulator
    - Contains the pscx-redux emulator 
- `.\tools\gdb-multiarch-12.1` - GDB Debugger
    - Contains a gdb client that can connect to the pscx-redux emulator gdb server
- `.\tools\GnuWin32\bin\make.exe` - Make
    - Contains a Windows make port that can be used to build the code with the Makefile provided
- `.\tools\mips` - Contains the mips compiler
    - Used to compile the project for targeting PSX
- `.\vscode`
    - contains launch and task configurations for VS Code on Windows

## Debugging Bugginess
- If you remove all breakpoints while running the code, and then add a breakpoint, the code will not stop at the breakpoint. 
    - You must pause the debugger and then add a breakpoint. Then you can continue debugging.