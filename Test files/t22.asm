.data
string: .asciiz "Hello, RISC-V!\n"

.text
.globl main

loop:
    li a7, 10         # System call to print string
    ecall
    j loop            # Loop indefinitely

printInt:
    addi sp, sp, -32   # Allocate space on stack
    sw s0, 28(sp)      # Save s0
    addi s0, sp, 32    # Set up new frame pointer
    sw a0, -20(s0)     # Save a0
    li a7, 1           # System call to print integer
    mv a0, a5          # Move a5 (saved in printInt) to a0
    ecall              # Execute system call
    nop                # No operation (delay slot)
    lw s0, 28(sp)      # Restore s0
    addi sp, sp, 32    # Deallocate stack space
    ret                # Return

printString:
    addi sp, sp, -32   # Allocate space on stack
    sw s0, 28(sp)      # Save s0
    addi s0, sp, 32    # Set up new frame pointer
    sw a0, -20(s0)     # Save a0
    li a7, 4           # System call to print string
    mv a0, a5          # Move a5 (saved in printString) to a0
    ecall              # Execute system call
    nop                # No operation (delay slot)
    lw s0, 28(sp)      # Restore s0
    addi sp, sp, 32    # Deallocate stack space
    ret                # Return

dble:
    addi sp, sp, -32   # Allocate space on stack
    sw s0, 28(sp)      # Save s0
    addi s0, sp, 32    # Set up new frame pointer
    sw a0, -20(s0)     # Save a0
    lw a5, -20(s0)     # Load saved a0 into a5
    slli a5, a5, 1     # Shift left logical immediate by 1
    mv a0, a5          # Move a5 to a0 (result)
    lw s0, 28(sp)      # Restore s0
    addi sp, sp, 32    # Deallocate stack space
    ret                # Return

main:
    addi sp, sp, -32   # Allocate space on stack
    sw ra, 28(sp)      # Save return address (ra)
    sw s0, 24(sp)      # Save s0
    addi s0, sp, 32    # Set up new frame pointer
    la a0, string      # Load address of string into a0
    jal ra, printString # Call printString
    li a0, 10          # Load integer 10 into a0
    jal ra, printInt   # Call printInt
    lw ra, 28(sp)      # Restore ra
    lw s0, 24(sp)      # Restore s0
    addi sp, sp, 32    # Deallocate stack space
    ret                # Return
