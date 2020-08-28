[![Windows Build Status (appveyor.com)](https://img.shields.io/appveyor/ci/mity/mctrl/master.svg?logo=windows&label=windows%20build)](https://ci.appveyor.com/project/mity/mctrl/branch/master)
[![Coverity Scan Status](https://img.shields.io/coverity/scan/mctrl.svg?label=coverity%20scan)](https://scan.coverity.com/projects/mctrl)

# mCtrl Readme

 * Home website: http://mctrl.org
 * Main code repo: http://github.com/mity/mctrl


## What is mCtrl

mCtrl is a C library providing set of additional user interface controls for
MS Windows, intended to be complementary to the standard Win32API controls from
`USER32.DLL` and `COMCTL32.DLL`.

The API of the library is designed to be similar to the Win32API. I.e. after
a window class of a control is registered with corresponding initialization
function, the control can be normally created with the Win32API's functions
`CreateWindow()` or `CreateWindowEx()` and controlled with `SendMessage()`.


## Getting mCtrl

You can always get the latest version and most actual information on project
home site:

 * http://mctrl.org

There are usually two packages for each release version available:

 * `mCtrl-x.y.z-bin.zip`: pre-built binary package
 * `mCtrl-x.y.z-src.zip`: source package

The pre-built package contains 32-bit as well as 64-bit binaries of `MCTRL.DLL`
and examples, and also documentation for application developers. The source
package is direct export of source tree from version control system repository.

The current code (possibly untested and unstable) can also be cloned from git
repository hosted on github:

 * http://github.com/mity/mctrl


## Using mCtrl

The pre-built release package has the following directory structure:

```
mCtrl-x.y.z/
 |  AUTHORS.md            # List of authors contributing to the project
 |  CONTRIBUTING.md       # Info how to contribute to the project
 |  COPYING.lib           # GNU Lesser General Public License
 |  README.md             # This file
 |
 +- bin/                  # 32-bit binaries
 |   |  mCtrl.dll         # MCTRL.DLL
 |   |  example-*.exe     # Pre-built examples
 |   |
 |   +- debug-gcc/
 |   |      mCtrl.dll     # Debug build of MCTRL.DLL (built with gcc)
 |   |
 |   +- debug-msvc/
 |          mCtrl.dll     # Debug build of MCTRL.DLL (built with Visual Studio)
 |          mCtrl.pdb     # Visual Studio debug info
 |
 +- bin64/                # 64-bit binaries
 |   |  mCtrl.dll         # MCTRL.DLL
 |   |  example-*.exe     # Pre-built examples
 |   |
 |   +- debug-gcc/
 |   |      mCtrl.dll     # Debug build of MCTRL.DLL (built with gcc)
 |   |
 |   +- debug-msvc/
 |          mCtrl.dll     # Debug build of MCTRL.DLL (built with Visual Studio)
 |          mCtrl.pdb     # Visual Studio debug info
 |
 +- doc/                  # Reference manual
 |      *.html
 |
 +- examples/             # Examples
 |      CMakeLists.txt    # CMake recipe for building the examples
 |      *.c; *.h; *.rc    # Source files of the examples
 |
 +- include/
 |   |  mctrl.h           # All-in-one public header (includes all mCtrl/*.h)
 |   |
 |   +- mCtrl/
 |          *.h           # mCtrl public headers
 |
 +- lib/                  # 32-bit import libraries
 |      libmCtrl.dll.a    # Import library for gcc
 |      mCtrl.lib         # Import library for Visual Studio
 |
 +- lib64/                # 64-bit import libraries
        libmCtrl.dll.a    # Import library for gcc
        mCtrl.lib         # Import library for Visual Studio
```

Using mCtrl is as easy as using any other DLL, just tell your compiler and
linker where it can find mCtrl headers and libraries.

Note you should instruct your C/C++ compiler to search for header files in
the `include` directory and use the directory `mCtrl` as part of preprocessor
`#include` directives, e.g.:

```C
#include <mCtrl/dialog.h>
#include <mCtrl/treelist.h>
```


## Building mCtrl from Sources

Disclaimer: If you want to just use `MCTRL.DLL` you should probably stick with
the pre-built package.

To build mCtrl yourself from the source package or cloned git repository, first
of all you need to use [CMake](http://www.cmake.org) 3.1 (or newer) to generate
project files, Makefile or whatever the development tool-chain of your choice
expects.

### Build with Mingw-w64

It's recommended to use out-of-source-tree builds, so create e.g. a directory
`build` in the main mCtrl directory. (If you build in directory located
elsewhere, replace the `..` in the following instructions with the path
pointing to the root mCtrl directory.)

To build with MSYS + [mingw-w64](http://mingw-w64.org) + Make:

```sh
$ mkdir build
$ cd build
$ cmake -G "MSYS Makefiles" ..
$ make
```

To build with MSYS + mingw-w64 + [Ninja](http://martine.github.io/ninja):

```sh
$ mkdir build
$ cd build
$ cmake -G "Ninja" ..
$ ninja
```

To build within [MSYS2](http://msys2.github.io), make sure you have these
MSYS2 packages installed:

 * `make`
 * `mingw-w64-i686-gcc`, `mingw-w64-i686-cmake` (for 32-bit build)
 * `mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-cmake` (for 64-bit build)

Then start MSYS2 shell with `mingw32_shell.bat` or `mingw64_shell.bat`
respectively and follow the same instructions as above for MSYS + 
mingw-w64 + Make.

Note you may need to specify path to `gcc` if you want to use a different gcc
version than the one in your `$PATH`, e.g. if you have multiple mingw-w64
variants installed, one targeting 32-bit and one 64-bit build.

You may do so by setting the variable `CC` prior using CMake. CMake is smart
enough to derive paths to the other tools like a linker or a resource compiler
automatically.

```sh
export CC=/path/to/the/desired/gcc
```

### Build with Microsoft Visual Studio 2017 (or newer)

Visual Studio 2017 and newer supports CMake build system directly:

 1. Start Visual Studio 2017.
 2. In menu File, choose submenu Open and Folder.
 3. In the open dialog, navigate to mCtrl main folder and open it.
 4. In menu CMake, choose Build all.

### Build with Older Version of Microsoft Visual Studio

To build with older Microsoft Visual Studio (versions 2013 and 2015 are known
to work), you have to generate project files manually:

```sh
$ mkdir build
$ cd build
$ cmake -G "Visual Studio 12 2013" ..           # MSVC 2013, 32-bit build
$ cmake -G "Visual Studio 12 2013 Win64" ..     # MSVC 2013, 64-bit build
$ cmake -G "Visual Studio 14 2015" ..           # MSVC 2015, 32-bit build
$ cmake -G "Visual Studio 14 2015 Win64" ..     # MSVC 2015, 64-bit build
```

Then open the generated solution file `build/mCtrl.sln` in Visual Studio and
build the target `ALL_BUILD`.

Unfortunately, for older MSVC versions, CMake does not support generating
projects targeting multiple architectures. Therefore, to build both 32 and
64-bit binaries, you have to generate project files or Makefiles twice and
build them separately, in dedicated directories.

### Other Toolchains

Other CMake generators may or may not work. If they do not, then one or more
`CMakeLists.txt` files within mCtrl directory hierarchy may need some tuning.

Use

```sh
$ cmake --help
```

and refer to CMake documentation to learn more about CMake, its options and
capabilities.

### After the Build

After the building, consider running a mCtrl test-suite to verify correctness
of your build. The test suite, as well as some examples demonstrating mCtrl,
are built as part of the mCtrl build process.


## License

mCtrl itself is covered by the GNU Lesser General Public License 2.1 or
(if you choose so) any later version. Refer to the file `COPYING.lib.md` for
more information about licensing terms.

Some source files and libraries incorporated into mCtrl may have different
(but compatible) licensing terms and some may be put into the public domain:

 * Examples (`examples/*`): Public domain
 * Unit tests (`tests/*.c`): Public domain
 * 3rd party code:
   * Acutest (`lib/acutest/`): MIT license
   * c-reusables (`lib/c-reusables/`): MIT license
   * HSLuv-C (`lib/hsluv-c/`): MIT license


## Reporting Bugs

If you encounter any bug, please be so kind and report it. Unheard bugs cannot
get fixed. You can submit bug reports here:

 * http://github.com/mity/mctrl/issues

Please provide the following information with the bug report:

 * mCtrl version you are using.
 * Whether you use 32-bit or 64-bit build of mCtrl.
 * OS version where you reproduce the issue.
 * As explicit description of the issue as possible, i.e. what behavior
   you expect and what behavior you see.
   (Reports of the kind "it does not work." do not help).
 * If relevant, consider attaching a screenshot.
 * If relevant, some code reproducing the issue can be vital. Ideally in some
   of these forms:
    - a complete source code which can compile as a standalone C or C++ program;
    - a patch or pull request to be applied to one of the examples in mCtrl
      source tree;
    - or (if it is not overly long) as a C function (or few functions) which
      can replace whole function (or few functions) in one of those examples.
