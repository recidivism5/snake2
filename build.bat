@echo off
if not defined DevEnvDir (
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)
cl /nologo /O1 /w /GS- main.c /Fe"snake.exe" /link /nodefaultlib /subsystem:windows kernel32.lib gdi32.lib user32.lib
rm main.obj