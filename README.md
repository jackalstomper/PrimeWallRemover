## Usage
WallRemover <MREA File Path>

Mostly designed to be used in conjunction with a batch script and PakTool
to convert the entire games collision models at once.

## Mod Installation
Extract the game files from a Metroid Prime NTSC 0-00 ISO using Dolphin or other tools such as Gamecube ISO Tool.

Double click `remove_walls.bat` to run it.

Enter the directory you extracted the game files to.

For example `C:\Metroid Prime [GM8E01]\root`

Wait for the tool to complete.

Done! You can now recompile the ISO using Gamecube ISO tool or run the extracted folder in Dolphin.

## Compiling
This tool doesn't have any dependencies outside of the C++ standard library.
The repository includes a VS2015 community project file to work with.

Building with GCC/Clang is a one-liner: `g++ --std=c++14 main.cpp area_collision.cpp utility.cpp -O2 -o WallRemover`
