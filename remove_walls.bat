@echo off
set /p root="Enter extracted Metroid Prime root directory: "
for /f "delims=" %%i in ('cd') do set mydir=%%i
set remover=%mydir%\WallRemover.exe
echo %root%
forfiles /p "%root%" /s /m *.MREA /c "cmd /c %remover% @path"
