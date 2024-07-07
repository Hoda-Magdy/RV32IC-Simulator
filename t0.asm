.data
string: .asciiz "Hello, RISC-V!\n"

.text
.globl _start

_start:
    # Setup the stack pointer (assuming you want to initialize it differently)
    la sp, string

    # Jump to main
    jal ra, main

loop:
    li a7, 10        # System call to print string
    ecall
    j loop

main:
    li ra, 1         # Initialize registers (example values)
    li sp, 2
    li gp, 3
    li tp, 4
    li t0, 5
    li t1, 6

    add t2, ra, sp   # Example arithmetic operations
    add s0, t2, zero
    add s1, s0, t2
    sub a0, s1, t1
    add t1, t1, t1
    and s0, s0, s1
    xor s1, s1, a0
    addi t2, t2, 5
    sub s1, s1, a0

    li a1, 1952      # Example branch instructions
    slli a1, a1, 5
    li a2, 1952

test_branch:
    blt ra, sp, L1   # Branch if less than
    j FAIL

L1:
    bge t1, t0, L2   # Branch if greater than or equal
    j FAIL

L2:
    beq a1, a2, PASS # Branch if equal

FAIL:
    li t6, -1        # Failure condition
    j finish

PASS:
    li t6, 0         # Success condition

finish:
    li a7, 10        # Exit program
    ecall
