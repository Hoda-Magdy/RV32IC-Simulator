//
// Created by Hoda Hussein on 6/28/2024.
//
/*
	This is just a skeleton. It DOES NOT implement all the requirements.
	It only recognizes the RV32I "ADD", "SUB" and "ADDI" instructions only.
	It prints "Unkown Instruction" for all other instructions!

	References:
	(1) The risc-v ISA Manual ver. 2.1 @ https://riscv.org/specifications/
	(2) https://github.com/michaeljclark/riscv-meta/blob/master/meta/opcodes
*/

#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc;
unsigned char memory[(16+64)*1024];

void emitError(char *s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW){
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void instDecExec(unsigned int instWord) {
    unsigned int rd, rs1, rs2, funct3, funct7, opcode;
    unsigned int I_imm, S_imm, B_imm, U_imm, J_imm;
    unsigned int address;

    unsigned int instPC = pc - 4;

    opcode = instWord & 0x0000007F;
    rd = (instWord >> 7) & 0x0000001F;
    funct3 = (instWord >> 12) & 0x00000007;
    rs1 = (instWord >> 15) & 0x0000001F;
    rs2 = (instWord >> 20) & 0x0000001F;
    funct7 = (instWord >> 25) & 0x0000007F;

    I_imm = ((instWord >> 20) & 0xFFF) | (((instWord >> 31) ? 0xFFFFF000 : 0x0));
    B_imm = ((instWord >> 31) << 12) |  // imm[12]
            ((instWord >> 25) & 0x3F) << 5 |  // imm[10:5]
            ((instWord >> 8) & 0xF) << 1 |  // imm[4:1]
            ((instWord >> 7) & 0x1) << 11 // imm[11]
            | (((instWord >> 31) ? 0xFFFFE000 : 0x0)); // Sign-extend the 13-bit immediate to 32 bits


    printPrefix(instPC, instWord);

    if(opcode == 0x33) { // R Instructions
        switch(funct3) {
            case 0:
                if(funct7 == 0x20) {
                    cout << "\tSUB\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    instPC+=4;
                } else if (funct7 == 0x00) { // Check for ADD
                    cout << "\tADD\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    instPC+=4;

                } else {
                    cout << "\tUnknown R Instruction \n";
                }
                break;
            case 1:
                cout << "\tSLL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                instPC+=4;
                break;

            case 2:
                cout << "\tSLT\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                instPC+=4;
                break;

            case 3:
                cout << "\tSLTU\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                instPC+=4;
                break;

            case 4:
                cout << "\tXOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                instPC+=4;
                break;

            case 5:
                if(funct7 == 0x20) {
                    cout << "\tSRA\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    instPC+=4;
                } else if (funct7 == 0x00) { // Check for ADD
                    cout << "\tSRL\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                    instPC+=4;
                } else {
                    cout << "\tUnknown R Instruction \n";
                }
                break;
            case 6:
                cout << "\tOR\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                instPC+=4;
                break;
            case 7:
                cout << "\tAND\tx" << rd << ", x" << rs1 << ", x" << rs2 << "\n";
                instPC+=4;
                break;
            default:
                cout << "\tUnknown R Instruction \n";
        }
    } else if(opcode == 0x13) { // I instructions
        switch(funct3) {
            case 0:
                cout << "\tADDI\tx" << rd << ", x" << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
                break;
            default:
                cout << "\tUnknown I Instruction \n";
        }
    } else if(opcode == 0x63) { //B type
        switch(funct3) {
            case 0:
                cout << "\tBEQ\tx" << rs1 << ", x" << rs2 << ", " << instPC+(int)B_imm << "\n";
                instPC+=4;
                break;
            case 1:
                cout << "\tBNE\tx" << rs1 << ", x" << rs2 << ", " << instPC+(int)B_imm << "\n";
                instPC+=4;
                break;
            case 2:
                cout << "\tBLT\tx" << rs1 << ", x" << rs2 << ", " << instPC+(int)B_imm << "\n";
                instPC+=4;
                break;
            case 3:
                cout << "\tBGE\tx" << rs1 << ", x" << rs2 << ", " << instPC+(int)B_imm << "\n";
                instPC+=4;
                break;
            case 4:
                cout << "\tBLTU\tx" << rs1 << ", x" << rs2 << ", " << instPC+(int)B_imm << "\n";
                instPC+=4;
                break;
            case 5:
                cout << "\tBGEU\tx" << rs1 << ", x" << rs2 << ", " << instPC+(int)B_imm << "\n";
                instPC+=4;
                break;
    }}
    else {
        cout << "\tUnknown Instruction \n";
    }
}


int main(int argc, char *argv[]){

	unsigned int instWord=0;
	ifstream inFile;
	ofstream outFile;

	if(argc<1) emitError("use: rvcdiss <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if(inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg (0, inFile.beg);
		if(!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");

		while(true){
				instWord = 	(unsigned char)memory[pc] |
							(((unsigned char)memory[pc+1])<<8) |
							(((unsigned char)memory[pc+2])<<16) |
							(((unsigned char)memory[pc+3])<<24);
				pc += 4;
				// remove the following line once you have a complete simulator
			if(pc==256) break;			// stop when PC reached address 32
				instDecExec(instWord);
		}
	} else emitError("Cannot access input file\n");
}