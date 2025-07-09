set proj_name=app
riscv32-unknown-elf-objdump -h -d -t %proj_name%.rv32 > %proj_name%.lst
