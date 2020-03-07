## About
This is a mod that removes wall and ceiling collision from the game. It's less fun than it sounds.

## Usage

#### Windows
Double click WallRemover.exe

#### Linux/Mac
Compile and run WallRemover in a terminal

See: [Compiling](#compiling)

#### Instructions
Enter the file path to your Metroid Prime 0-00 NTSC ISO file when prompted.

For example `C:\GM8E01.iso`

The tool will extract the game files from the ISO in your working directory in the folder `discExtract`.

Once done modifying the files, it will compile a new ISO in your working directory called `prime-nowalls.iso`.

Once complete you can run the ISO in Dolphin or in a USB loader like nintendont.

## Compiling<a name="compiling"></a>
The tool requires a C++17 compiler and depends on the [nod](https://github.com/AxioDL/nod) library for interacting with ISO files.

The repository includes this dependency as a submodule.

```plain
git submodule update --init --recursive
```

The tool uses CMAKE to generate build scripts.

### Linux/Mac
The project can be built using cmake + make as typical.

```plain
mkdir build
cd build/
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

mac builds are untested and some conditional compiler checks in `CMakeLists.txt` and `main.cpp` may need to be changed.

### Windows
Opening the repo as a CMAKE project should be all thats needed to compile with Visual Studio
