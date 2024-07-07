.data
string: .asciiz "Hello, RISC-V!\n"

.text
.globl _start

_start:
    auipc sp, 0x14      # Setup the stack pointer
    mv sp, sp           # No-op (optional)

    jal ra, main        # Jump to main

loop:
    li a7, 10           # System call code for print_int
    ecall               # Perform system call
    j loop              # Loop indefinitely

printInt:
    addi sp, sp, -32    # Allocate stack space
    sw s0, 28(sp)       # Save s0
    addi s0, sp, 32     # Set up new frame pointer
    sw a0, -20(s0)      # Save a0
    li a7, 1            # System call code for print_int
    mv a0, a5           # Move a5 to a0 (argument for print_int)
    ecall               # Perform system call
    nop                 # No operation (delay slot)
    lw s0, 28(sp)       # Restore s0
    addi sp, sp, 32     # Deallocate stack space
    ret                 # Return

printString:
    addi sp, sp, -32    # Allocate stack space
    sw s0, 28(sp)       # Save s0
    addi s0, sp, 32     # Set up new frame pointer
    sw a0, -20(s0)      # Save a0
    li a7, 4            # System call code for print_string
    mv a0, a5           # Move a5 to a0 (argument for print_string)
    ecall               # Perform system call
    nop                 # No operation (delay slot)
    lw s0, 28(sp)       # Restore s0
    addi sp, sp, 32     # Deallocate stack space
    ret                 # Return

sum:
    addi sp, sp, -32    # Allocate stack space
    sw ra, 28(sp)       # Save ra
    sw s0, 24(sp)       # Save s0
    addi s0, sp, 32     # Set up new frame pointer
    sw a0, -20(s0)      # Save a0 (argument)
    lw a5, -20(s0)      # Load a0 (argument) into a5
    beqz a5, sum_end    # Branch if a5 is zero to sum_end
    lw a5, -20(s0)      # Reload a5 (loop counter)
    addi a5, a5, -1     # Decrement a5
    mv a0, a5           # Move a5 to a0 (next argument)
    jal ra, sum         # Recursive call to sum
    mv a4, a0           # Move return value to a4
    lw a5, -20(s0)      # Reload a5 (original argument)
    add a0, a4, a5      # Calculate sum
sum_end:
    lw ra, 28(sp)       # Restore ra
    lw s0, 24(sp)       # Restore s0
    addi sp, sp, 32     # Deallocate stack space
    ret                 # Return

main:
    li a5, 5            # Set initial value for sum
    mv a0, a5           # Move a5 to a0 (argument for sum)
    jal ra, sum         # Call sum function
    mv a0, a0           # Move sum result to a0
    jal ra, printInt    # Call printInt to print the sum
    li a0, 1            # Exit program code
    li a7, 10           # System call code for exit
    ecall               # Perform system call
