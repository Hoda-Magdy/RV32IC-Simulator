.data
string: .asciiz "Hello, RISC-V!\n"

.text
.globl _start

_start:
    # Load address of string into t0
    lui t0, %hi(string)      # Load upper 20 bits of string address
    addi t0, t0, %lo(string) # Add lower 12 bits of string address

    # Setup the stack pointer
    mv sp, t0

    # Jump to main
    jal zero, main

loop:
    li a7, 10         # System call to print string
    ecall
    j loop

printInt:
    addi sp, sp, -32
    sw s0, 28(sp)
    addi s0, sp, 32
    sw a0, -20(s0)
    li a7, 1          # System call to print integer
    mv a0, a5
    sw a5, -20(s0)
    ecall
    nop
    lw s0, 28(sp)
    addi sp, sp, 32
    ret

printString:
    addi sp, sp, -32
    sw s0, 28(sp)
    addi s0, sp, 32
    sw a0, -20(s0)
    li a7, 4          # System call to print string
    mv a0, a5
    sw a5, -20(s0)
    ecall
    nop
    lw s0, 28(sp)
    addi sp, sp, 32
    ret

dble:
    addi sp, sp, -32
    sw s0, 28(sp)
    addi s0, sp, 32
    sw a0, -20(s0)
    lw a5, -20(s0)
    slli a5, a5, 1
    mv a0, a5
    lw s0, 28(sp)
    addi sp, sp, 32
    ret

main:
    addi sp, sp, -32
    sw ra, 28(sp)
    sw s0, 24(sp)
    addi s0, sp, 32
    la a0, string     # Load address of string
    jal zero, printString
    li a0, 10         # Load integer 10
    jal zero, printInt
    lw ra, 28(sp)
    lw s0, 24(sp)
    addi sp, sp, 32
    ecall             # Exit program
