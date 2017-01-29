## Mod Installation
Extract the game files from a Metroid Prime NTSC 0-00 ISO using Dolphin or other tools such as Gamecube ISO Tool.

#### Windows
Double click WallRemover.exe

#### Linux/Mac
Compile and run WallRemover in a terminal

(Macs may need to use `clang` instead of `g++`)

Example:
```bash
$ g++ --std=c++14 main.cpp area_collision.cpp utility.cpp -O2 -o WallRemover
$ ./WallRemover
```

See also: [Compiling](#compiling)

#### Run the tool
Enter the directory you extracted the game files to when prompted.

For example `C:\Metroid Prime [GM8E01]\root`

Wait for the tool to complete.

Done! You can now recompile the ISO using Gamecube ISO tool or run the extracted folder in Dolphin.

## Compiling<a name="compiling"></a>
This tool doesn't have any dependencies outside of the C++ standard library.
The repository includes a VS2015 community project file to work with.

Building with GCC/Clang on Linux/Mac is a one-liner: `g++ --std=c++14 main.cpp area_collision.cpp utility.cpp -O2 -o WallRemover`
