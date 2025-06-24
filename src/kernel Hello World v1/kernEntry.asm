;Fait le lien entre le bootloader et le kernel
section .text
    [bits 32]
    [extern main]
    call main
    jmp $