# RV32IC-Simulator
The RV32IC Simulator is a powerful tool, providing an accessible and accurate way to simulate and understand the execution of machine code on RV32IC hardware.
## Overview
We created a simple RISC-V simulator that interprets, outputs and executes RV32I machine code instructions. The simulator is capable of handling R, I, S, B, U, J instruction types as well as system calls (ECALL).

## Table of Contents
-	Building the Simulator
-	How to Use the Simulator
-	Simulator Design
-	Challenges and Limitations

## Building the Simulator
We used a C++ compiler to develop our program. Hoda wrote the code segments for R and B instructions, Farida wrote the code segments for the S and J instructions, and Laila worked on the I and U instructions. All 3 students worked on handling the “ecall” instruction together, as well as compressed instructions.

## How to use the Simulator
Clone the repository. Write an assembly program on RARS or a similar program. Assemble the program and save the machine code created (.bin file). Save this file in the folder of the C++ program. Write “g++ -o rn main.cpp”. After executing this line, write “./rn instructions.bin”, where you replace instructions.bin with the name of the machine code binary file.

## Simulator Design
**Memory and Registers**

The simulator has a memory size of (16+64) * 1024 bytes. It also has an array “regArray[32]” which is used to keep track of the values stored in the registers of RISC-V. A program counter is also used to keep track of the current instruction address.

**Instruction Decoding and Execution**

A function is used to handle the decoding, identification and execution of an instruction word. It first uses shift and & operators to correctly place and separate the segments of an instruction (opcode, rd, rs1, …etc). Then each type of instruction’s immediate is extracted depending on how the immediate is placed in the function and whether the immediate needs sign extending, zero extending, or neither. 

A switch case is used to check the type of instruction depending on the opcode, and based on the opcode it calls the instruction that handles this type of instruction, if it has more than one instruction with the same opcode (e.g.: R instructions)., or directly executes the instruction (e.g.: lui instruction). In the functions that are specified for each function type, switch cases are used again to identify the instruction based on funct3, which was previously extracted from the function word. If funct3 is not enough to identify the given function, an if statement is used to check funct7 (this can be seen in R instructions). When the instruction is identified, it is firstly printed to the screen using the correct instruction syntax. Then the execution occurs by updating the regArray and/or the memory. The program counter is also updated based on the instruction. 

**Error Handling**

The function ‘emitError’ is used to print error messages and exit the simulation in case of any issues. “Unknown instruction is also printed to the screen if it does not match any of the cases given.

## Challenges and Limitations

**Challenges** 

Handling the different types of immediates was a bit challenging. Each instruction format has its unique way of extracting immediate values and registers. Ensuring that these values were correctly sign-extended or zero-extended as per the specification required careful bit manipulation, which was error-prone initially. Also handling the execution of the store and load function was challenging as it included accessing the memory and addresses and updating it. They were also challenging due to handling not only words, but also half words and a byte (e.g. sh/sb). The ecall implementation was also challenging overall. Implementing system calls to handle standard I/O operations required careful handling of registers and memory. Maintaining the correct program counter and ensuring that instructions executed in the correct order while handling jump and branch instructions was bit challenging and had to be handled accurately as mismanagement of the PC would ruin the flow of the execution of the rest of the program.

**Limitations**

The error handling does not recognize all possible error cases and is basic. It could be improved by providing more detailed diagnostics. The memory size is also fixed to (16+64) * 1024 bytes, which may not be sufficient for running large programs with more instructions. The Ecall function is also limited. Only a subset of ecall operations are supported, while the others are not implemented.

