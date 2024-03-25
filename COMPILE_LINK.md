## PSX Executable Compilation and Linking 
- PSX executables are statically linked, as opposed to dynamically linked. Click [here](https://www.linkedin.com/pulse/differences-between-static-dynamic-libraries-juan-david-tuta-botero/) for a more information on the difference between the two linking methods. Static linking is very common on embedded systems, as there may not be a loader program to load shared libraries at runtime, as is the case on destkop operating systems like Windows, Linux, and MacOS.
- Historically PSX compilation was done with the official PSX copmiler `ccpsx`, however modern tools like `mipsel-none-elf-gcc` or `mipsel-linux-gnu-gcc` can be used to compile PSX executables on modern workstations. The PSX uses a MIPS R3000 CPU so a compiler that can target MIPS is needed.
- Makefile
- mipsel gcc compiler
### Compiler Flags
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
    - **-ffunction-sections**
        > -ffunction-sections generates a separate ELF section for each function in the source file. The unused section elimination feature of the linker can then remove unused functions at link time.
        - [Doc](https://developer.arm.com/documentation/101754/0622/armclang-Reference/armclang-Command-line-Options/-ffunction-sections---fno-function-sections?lang=en)
    - **-mno-gpopt** 
        > Do not use GP-relative accesses for symbols that are known to be in a small data section; see -G, -mlocal-sdata and -mextern-sdata. -mgpopt is the default for all configurations.
        - [Doc](https://gcc.gnu.org/onlinedocs/gcc/MIPS-Options.html)
        - More information about the `$gp` register can be found [here](https://training.mips.com/basic_mips/PDF/Assemble_Language.pdf) 
    - **-fomit-frame-pointer**
        > Don't keep the frame pointer in a register for functions that don't need one. This avoids the instructions to save, set up and restore frame pointers; it also makes an extra register available in many functions. It also makes debugging impossible on some machines.
        - [Doc](https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Optimize-Options.html)
    - **-fno-builtin** 
        > Don’t recognize built-in functions that do not begin with ‘__builtin_’ as prefix. See Other built-in functions provided by GCC, for details of the functions affected, including those which are not built-in functions when -ansi or -std options for strict ISO C conformance are used because they do not have an ISO standard meaning.
        - [Doc](https://gcc.gnu.org/onlinedocs/gcc/C-Dialect-Options.html)
    - **-fno-strict-aliasing** 
        > Causes the compiler to avoid assumptions regarding non-aliasing of objects of different types
        - [Doc](https://gcc.gnu.org/onlinedocs/gcc-5.5.0/gnat_ugn/Alphabetical-List-of-All-Switches.html)
    - **-Wno-attributes**
        > Do not warn if an unexpected __attribute__ is used, such as unrecognized attributes, function attributes applied to variables, etc. This will not stop errors for incorrect use of supported attributes.
        - [Doc](https://gcc.gnu.org/onlinedocs/gcc-4.4.2/gcc/Warning-Options.html)

### Libraries

In the `Makefile`, library binaries get linked like so:
```bash
    LDFLAGS += -Wl,--start-group
    LDFLAGS += -lapi
    LDFLAGS += -lc
    LDFLAGS += -lc2
    LDFLAGS += -lcard
    LDFLAGS += -lcomb
    LDFLAGS += -lds
    LDFLAGS += -letc
    LDFLAGS += -lgpu
    LDFLAGS += -lgs
    LDFLAGS += -lgte
    LDFLAGS += -lgpu
    LDFLAGS += -lgun
    LDFLAGS += -lhmd
    LDFLAGS += -lmath
    LDFLAGS += -lmcrd
    LDFLAGS += -lmcx
    LDFLAGS += -lpad
    LDFLAGS += -lpress
    LDFLAGS += -lsio
    LDFLAGS += -lsnd
    LDFLAGS += -lspu
    LDFLAGS += -ltap
    LDFLAGS += -lcd # only needed for cd-rom access
    LDFLAGS += -Wl,--end-group
```
These libraries can be found in the `thirdparty/nugget/psyq/lib` directory. They are `.a` (archive) objects, which implies a static library, as opposed to a shared/dynamic library. A shared library would have the `.so` (shared object) extension. For Windows Libraries, these would be equivalent to `.lib` and `.dll` (dynamically linked library) files, respectively.

A description of these libraries can be found in the PSX Documentation found in `docs\LibRef47.pdf` and `docs\LibOver47.pdf`.

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
            - 1K of Scratchpad (D-Cache used as Fast RAM). [Ref](https://www.chibialiens.com/mips/psx.php)
- `default.ld`
    - sets the heap base to the bss end.
    - the heap will be used when dynamic allocations are made with `malloc` functions
    - the bss is the uninitialized data segment memory
