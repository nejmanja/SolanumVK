@REM -V is for vulkan, -o is output
setlocal enabledelayedexpansion

@echo off

set COMPILER="%VULKAN_SDK%\Bin\glslangValidator.exe"

for %%E in (comp vert frag) do (

    for %%f in (..\shaders\*.^%%E) do (
        set "src=%%f"
        call set "out=..\shaders\%%~nxf.spv"
        echo Compiling shader !src! to !out!...
        !COMPILER! -V "%%f" -o "!out!"
    )
)