@echo off
set /p root="Enter extracted Metroid Prime root directory: "
for /f "delims=" %%i in ('cd') do set mydir=%%i
echo %root%
echo "Unpacking paks"
for /l %%i in (1, 1, 8) do (
	%mydir%\PakTool.exe -d "%root%\Metroid%%i.pak"
	%mydir%\PakTool.exe -x "%root%\Metroid%%i.pak" -e
)
echo "Modding MREAs"
forfiles /p "%root%" /s /m *.MREA /c "cmd /c %mydir%\WallRemover.exe @path"
echo "Repacking paks"
for /l %%i in (1, 1, 8) do (
	%mydir%\PakTool.exe -r MP1 "%root%\Metroid%%i-pak" "%root%\Metroid%%i.pak" "%root%\Metroid%%i-pak.txt" -e
	rd /s /q "%root%\Metroid%%i-pak"
	del /q "%root%\Metroid%%i-pak.txt"
)
