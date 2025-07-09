@echo off
cd /d %~dp0
set proj_name=app
cd ..\..\
for %%a in ("%cd%") do (
echo 1 > "%cd%\Output\obj\projects\%%~nxa\ram.o"
echo 1 > "%cd%\Output\obj\projects\%%~nxa\Output\bin\app.o"
echo 1 > "%cd%\Output\obj\projects\%%~nxa\Output\bin\download.o"
echo 1 > "%cd%\Output\obj\projects\%%~nxa\Output\bin\res.o"
echo 1 > "%cd%\Output\obj\projects\%%~nxa\Output\bin\xcfg.o"
)
cd Output\bin\
@echo on
riscv32-unknown-elf-objdump -h -d -t %proj_name%.rv32 > %proj_name%.lst
