## About
This is a mod that removes wall and ceiling collision from the game. It's less fun than it sounds.

## Usage

### Windows
Double click `prime-wall-remover.exe`

### Linux
Execute `prime-wall-remover-linux` in a terminal

### Mac
I don't have a mac machine to precompile a binary.  
Mac users will need to compile the project temselves using cargo.  
See `Compiling`

### Running the tool
Enter the file path for your metroid Prime ISO when prompted.  
For example `C:\GM8E01.iso`  
Wait for the tool to complete.  
The tool will modify the ISO file in-place, make sure you have a clean copy elsewhere.

## Compiling
This is a standard rust project that can be compiled with `cargo build --release`
