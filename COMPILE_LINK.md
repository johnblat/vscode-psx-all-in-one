## PSX Executable Compilation and Linking
- Makefile
- mipsel gcc compiler
    - docs for options: https://gcc.gnu.org/onlinedocs/gcc/MIPS-Options.html
    - flags:
        - **-march=mps1**: [MIPS I R3000 architecture](https://en.wikipedia.org/wiki/MIPS_architecture#MIPS_I)
        - **-mabi=32**: 32-bit [ABI](https://en.wikipedia.org/wiki/Application_binary_interface)
        - **-EL**: [Little Endian](https://en.wikipedia.org/wiki/Endianness) Byte Order
        - **-fno-pic**: Do not generate [Position Independent Code](https://en.wikipedia.org/wiki/Position-independent_code). The reason for this when compiling to the PSX is that the PSX has a fixed memory map, and the code will always be loaded at the same address. Position Independet code is typically used when the code can be loaded at any address in memory, typically when the code is a shared library. PSX executables don't contain shared libraries, so this flag is used.
            - :exclamation: not found in the docs above
        - **-mno-shared**: 
            - Do not generate code that is fully position-independent, and that can therefore not be linked into shared libraries. This option only affects -mabicalls/-mno-abicalls.
            > All -mabicalls code has traditionally been position-independent, regardless of options like -fPIC and -fpic. However, as an extension, the GNU toolchain allows executables to use absolute accesses for locally-binding symbols. It can also use shorter GP initialization sequences and generate direct calls to locally-defined functions. This mode is selected by -mno-shared.
        - **-mno-abicalls**: 
            > Do not generate code that is suitable for [SVR4](https://en.wikipedia.org/wiki/UNIX_System_V#SVR4)-style dynamic objects. -mabicalls is the default for SVR4-based systems.
        - **-mfp32**: 
            > Assume that floating-point registers are 32 bits wide.
        - **-fno-stack-protector**
            - Opposite of:
            > -fstack-protector
            >      Emit extra code to check for buffer overflows, such as stack smashing attacks. This is done by adding a guard variable to functions with vulnerable objects. This includes functions that call alloca, and functions with buffers larger than 8 bytes. The guards are initialized when a function is entered and then checked when the function exits. If a guard check fails, an error message is printed and the program exits.
            - [Doc](https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Optimize-Options.html)
            - not in mips gcc docs linked
        - **-nostdlib**:
            - not in mips gcc docs linked
            >Do not use the standard system startup files or libraries when linking. No startup files and only the libraries you specify will be passed to the linker. The compiler may generate calls to memcmp, memset, memcpy and memmove. These entries are usually resolved by entries in libc. These entry points should be supplied through some other mechanism when this option is specified.
            - [Doc](https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Link-Options.html)
        - **-ffreestanding**
            > Assert that compilation targets a freestanding environment. This implies -fno-builtin. A freestanding environment is one in which the standard library may not exist, and program startup may not necessarily be at main. The most obvious example is an OS kernel. This is equivalent to -fno-hosted.
            - not in mips gcc docs linked
            - [Doc](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html)
