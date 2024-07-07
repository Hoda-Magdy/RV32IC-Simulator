#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>

using namespace std;

unsigned int pc = 0;
unsigned int regArray[32] = {0};
unsigned char memory[(16+64)*1024];

void emitError(char *s)
{
    cout << s;
    exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW){
    cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}

void handleEcall(unsigned int &instPC) {
   // unsigned int a7 = regArray[17]; // Read system call number from register a7
    unsigned int strAddress = regArray[10];
    unsigned char character = memory[strAddress];

    switch (regArray[17]) {
        case 1: // Print integer
            cout << "\tECALL: Print integer\n";
            cout << "Integer to print: " << regArray[10] << endl;
            break;
        case 4: // Print string
            cout << "\tECALL: Print string\n";
            while (character != '\0') {
                cout << character;
                strAddress++;
                character = memory[strAddress];
            }
            cout << endl;
            break;
        case 5: // Read integer
        {
            cout << "\tECALL: Read integer\n";
            cout << "Enter an integer: ";
            cin >> regArray[10];
            cout << "Integer read: " << regArray[10] << endl;
        }
            break;
        case 10: // Exit
            cout << "\tECALL: Exit\n";
            instPC = 0xFFFFFFFF; // Set the program counter to signal end of execution
            break;
        default:
            cout << "\tECALL: Unknown system call\n";
            break;
    }
}

void S_Instructions(unsigned int funct3, unsigned int rs1, unsigned int rs2, unsigned int S_imm, unsigned int &instPC) {
    unsigned int address;
    address = regArray[rs1] + (int)S_imm;

    switch(funct3) {
        case 0:
            cout << "\tSB\tx" <<dec<<rs2 << ", " << (int)S_imm << "(x" <<dec<< rs1 << ")" << "\n";
            memory[address] = (unsigned char)(regArray[rs2] & 0xFF); // Store byte
            instPC += 4;
            break;
        case 1:
            cout << "\tSH\tx" <<dec<< rs2 << ", " << (int)S_imm << "(x" <<dec<< rs1 << ")" << "\n";
            memory[address] = (unsigned char)(regArray[rs2] & 0xFF); // Store halfword (2 bytes)
            memory[address+1] = (unsigned char)((regArray[rs2] >> 8) & 0xFF);
            instPC += 4;
            break;
        case 2:
            cout << "\tSW\tx" <<dec<< rs2 << ", " << (int)S_imm << "(x" <<dec<< rs1 << ")" << "\n";
            memory[address] = (unsigned char)(regArray[rs2] & 0xFF); // Store word (4 bytes)
            memory[address+1] = (unsigned char)((regArray[rs2] >> 8) & 0xFF);
            memory[address+2] = (unsigned char)((regArray[rs2] >> 16) & 0xFF);
            memory[address+3] = (unsigned char)((regArray[rs2] >> 24) & 0xFF);
            instPC += 4;
            break;
        default:
            cout << "\tUnknown S Instruction \n";
    }
}

void J_instructions(unsigned int rd, unsigned int J_imm, unsigned int &instPC){
    cout << "\tJAL\tx" <<dec<< rd << ", " << 2*(int)J_imm << "\n";
    regArray[rd] = instPC + 4; // Store return address
    instPC += 2 * (int)J_imm;
}

void B_instructions(unsigned int funct3, unsigned int rs1, unsigned int rs2, unsigned int B_imm, unsigned int &instPC){
    bool takeBranch = false;
    switch(funct3) {
        case 0:
            cout << "\tBEQ\tx" <<dec<< rs1 << ", x" <<dec<< rs2 << ", " << instPC + (int)B_imm << "\n";
            if (regArray[rs1] == regArray[rs2]) takeBranch = true;
            break;
        case 1:
            cout << "\tBNE\tx" <<dec<< rs1 << ", x" <<dec<< rs2 << ", " << instPC + (int)B_imm << "\n";
            if (regArray[rs1] != regArray[rs2]) takeBranch = true;
            break;
        case 4:
            cout << "\tBLT\tx" <<dec<< rs1 << ", x" <<dec<< rs2 << ", " << instPC + (int)B_imm << "\n";
            if ((int)regArray[rs1] < (int)regArray[rs2]) takeBranch = true;
            break;
        case 5:
            cout << "\tBGE\tx" <<dec<< rs1 << ", x" <<dec<< rs2 << ", " << instPC + (int)B_imm << "\n";
            if ((int)regArray[rs1] >= (int)regArray[rs2]) takeBranch = true;
            break;
        case 6:
            cout << "\tBLTU\tx" <<dec<< rs1 << ", x" <<dec<< rs2 << ", " << instPC + (int)B_imm << "\n";
            if (regArray[rs1] < regArray[rs2]) takeBranch = true;
            break;
        case 7:
            cout << "\tBGEU\tx" <<dec<< rs1 << ", x" <<dec<< rs2 << ", " << instPC + (int)B_imm << "\n";
            if (regArray[rs1] >= regArray[rs2]) takeBranch = true;
            break;
        default:
            cout << "\tUnknown B Instruction \n";
    }
    if (takeBranch)
        instPC += (int)B_imm;
    else
        instPC += 4;
}

void R_instructions(unsigned int rd, unsigned int funct3, unsigned int rs1, unsigned int rs2, unsigned int funct7, unsigned int &instPC){
    switch(funct3) {
        case 0:
            if(funct7 == 0x20) {
                cout << "\tSUB\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
                regArray[rd] = regArray[rs1] - regArray[rs2];
            } else if (funct7 == 0x00) { // Check for ADD
                cout << "\tADD\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
                regArray[rd] = regArray[rs1] + regArray[rs2];
            } else {
                cout << "\tUnknown R Instruction \n";
            }
            break;
        case 1:
            cout << "\tSLL\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
            regArray[rd] = regArray[rs1] << (regArray[rs2] & 0x1F);
            break;
        case 2:
            cout << "\tSLT\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
            regArray[rd] = ((int)regArray[rs1] < (int)regArray[rs2]) ? 1 : 0;
            break;
        case 3:
            cout << "\tSLTU\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
            regArray[rd] = (regArray[rs1] < regArray[rs2]) ? 1 : 0;
            break;
        case 4:
            cout << "\tXOR\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
            regArray[rd] = regArray[rs1] ^ regArray[rs2];
            break;
        case 5:
            if(funct7 == 0x20) {
                cout << "\tSRA\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
                regArray[rd] = (int)regArray[rs1] >> (regArray[rs2] & 0x1F);
            } else if (funct7 == 0x00) { // Check for SRL
                cout << "\tSRL\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
                regArray[rd] = regArray[rs1] >> (regArray[rs2] & 0x1F);
            } else {
                cout << "\tUnknown R Instruction \n";
            }
            break;
        case 6:
            cout << "\tOR\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
            regArray[rd] = regArray[rs1] | regArray[rs2];
            break;
        case 7:
            cout << "\tAND\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", x" <<dec<< rs2 << "\n";
            regArray[rd] = regArray[rs1] & regArray[rs2];
            break;
        default:
            cout << "\tUnknown R Instruction \n";
    }
    instPC += 4;
}

void I_instructions(unsigned int rd, unsigned int funct3, unsigned int rs1, unsigned int I_imm, unsigned int &instPC){
    if (rd == 0 && rs1 == 0 && I_imm == 0 && funct3 == 0) {
        // NOP instruction
        cout << "\tNOP\n";
        instPC += 4;
        return;
    }
    switch(funct3) {
        case 0:
            cout << "\tADDI\tx" << dec << rd << ", x" << dec << rs1 << ", " << (int)I_imm << "\n";
            regArray[rd] = regArray[rs1] + (int)I_imm;
            break;
        case 2:
            cout << "\tSLTI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << (int)I_imm << "\n";
            regArray[rd] = ((int)regArray[rs1] < (int)I_imm) ? 1 : 0;
            break;
        case 3:
            cout << "\tSLTIU\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << (unsigned int)I_imm << "\n";
            regArray[rd] = (regArray[rs1] < (unsigned int)I_imm) ? 1 : 0;
            break;
        case 4:
            cout << "\tXORI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << (int)I_imm << "\n";
            regArray[rd] = regArray[rs1] ^ (int)I_imm;
            break;
        case 6:
            cout << "\tORI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << (int)I_imm << "\n";
            regArray[rd] = regArray[rs1] | (int)I_imm;
            break;
        case 7:
            cout << "\tANDI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << (int)I_imm << "\n";
            regArray[rd] = regArray[rs1] & (int)I_imm;
            break;
        case 1:
            cout << "\tSLLI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << ((unsigned int)I_imm & 0x1F) << "\n";
            regArray[rd] = regArray[rs1] << ((unsigned int)I_imm & 0x1F);
            break;
        case 5:
            if ((I_imm >> 10) & 0x1) { // Check if SRAI
                cout << "\tSRAI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << ((unsigned int)I_imm & 0x1F) << "\n";
                regArray[rd] = (int)regArray[rs1] >> ((unsigned int)I_imm & 0x1F);
            } else {
                cout << "\tSRLI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << ((unsigned int)I_imm & 0x1F) << "\n";
                regArray[rd] = regArray[rs1] >> ((unsigned int)I_imm & 0x1F);
            }
            break;
        default:
            cout << "\tUnknown I Instruction \n";
    }
    instPC += 4;
}
void IL_instructions(unsigned int rd,unsigned int funct3, unsigned int rs1, unsigned int I_imm, unsigned int &instPC) {
    unsigned int address;
    address = regArray[rs1] + (int)I_imm;

    switch(funct3) {
        case 0:
            cout << "\tLB\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";
            regArray[rd] = (int8_t)memory[address]; // Load byte
            instPC += 4;
            break;
        case 1:
            cout << "\tLH\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";

            regArray[rd] = (int16_t)(memory[address] | (memory[address+1] << 8)); // Load halfword
            instPC += 4;
            break;
        case 2:
            cout << "\tLW\tx" <<dec<< rd << ", " << (int)I_imm << "(x"<<dec << rs1 << ")" << "\n";
            regArray[rd] = memory[address] | (memory[address+1] << 8) | (memory[address+2] << 16) | (memory[address+3] << 24); // Load word
            instPC += 4;
            break;
        case 4:
            cout << "\tLBU\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";
            regArray[rd] = memory[address]; // Load unsigned byte
            instPC += 4;
            break;
        case 5:
            cout << "\tLHU\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";
            regArray[rd] = memory[address] | (memory[address+1] << 8); // Load unsigned halfword
            instPC += 4;
            break;
        default:
            cout << "\tUnknown Load Instruction \n";
    }
}

void instDecExec(unsigned int instWord) {
    unsigned int rd, rs1, rs2, funct3, funct7, opcode, I_imm, S_imm, B_imm, U_imm, J_imm;

    unsigned int instPC = pc;

    opcode = instWord & 0x7F;
    rd= (instWord >> 7) & 0x1F;
    funct3 = (instWord >> 12) & 0x07;
    rs1 = (instWord >> 15) & 0x1F;
    rs2 = (instWord >> 20) & 0x1F;
    funct7 = (instWord >> 25) & 0x7F;
    //int rs1=rs1_h;
    //int rs2=rs2_h;
    //int rd=rd_h;
    I_imm = (instWord >> 20);  // Extract imm[11:0]

    // Sign extend the immediate value
    if (I_imm & 0x800) {
        I_imm |= 0xFFFFF000;}
    S_imm = ((instWord >> 7) & 0x1F) | ((instWord >> 20) & 0xFE0);  // Combine imm parts
    if (S_imm & 0x800)  // Check if the immediate value is negative
        S_imm |= 0xFFFFF000;  // Sign extend
    B_imm = ((instWord >> 8) & 0xF) | (((instWord >> 25) & 0x3F) << 4) | (((instWord >> 7) & 0x1) << 10) |
            (((instWord >> 31) & 0x1) << 11);
    U_imm = instWord & 0xFFFFF000;
    J_imm = ((instWord >> 21) & 0x3FF) | (((instWord >> 20) & 0x1) << 10) | (((instWord >> 12) & 0xFF) << 11) |
            (((instWord >> 31) & 0x1) << 19);
    unsigned int imm;
    cout << "0x" << hex << instPC << "\t0x" << instWord;

    switch (opcode) {
        case 0x33: // R Instructions
            R_instructions(rd, funct3, rs1, rs2, funct7, instPC);
            break;
        case 0x03: // I Instructions (Loads)
            IL_instructions(rd, funct3, rs1, I_imm, instPC);
            break;
        case 0x13: // I Instructions
            I_instructions(rd, funct3, rs1, I_imm, instPC);
            break;
        case 0x23:  // Store
            S_Instructions(funct3, rs1, rs2, S_imm, instPC);
            break;
        case 0x63: // B Instructions
            B_instructions(funct3, rs1, rs2, B_imm, instPC);
            break;
        case 0x37: // U Instructions (LUI)
            cout << "\tLUI\tx" << rd << ", 0x" << (U_imm >> 12) << "\n";
            regArray[rd] = U_imm;
            instPC += 4;
            break;
        case 0x17: // U Instructions (AUIPC)
            cout << "\tAUIPC\tx" << rd << ", 0x" << (U_imm >> 12) << "\n";
            regArray[rd] = instPC + U_imm;
            instPC += 4;
            break;
        case 0x6F: // J Instructions
            J_instructions(rd, J_imm, instPC);
            break;
        case 0x67: //JALR
            cout << "\tJALR\tx" << rd << ", x" << rs1 << ", " << (int)I_imm << "\n";
            regArray[rd] = instPC+4;
            instPC+=I_imm;
        case 0x73: // I Instructions (ECALL/EBREAK)
            handleEcall(instPC);
            break;
        default:
            cout << "\tUnknown Instruction \n";
    }

    pc = instPC;
}

int main(int argc, char *argv[]) {
    unsigned int instWord = 0;
    unsigned int instPC = 0;
    ifstream inFile;
    ofstream outFile;

    if (argc < 2) emitError((char *) "use: rv32i_sim <machine_code_file_name>\n");

    inFile.open(argv[1], ios::in | ios::binary | ios::ate);
    if (inFile.is_open()) {
        int fsize = inFile.tellg();  // Get the file size
        inFile.seekg(0, inFile.beg);

        if (!inFile.read((char *) memory, fsize)) emitError((char *) "Cannot read from input file\n");

        while (instPC < fsize) {  // Loop until instPC exceeds file size
            instWord = (unsigned char) memory[instPC] |
                       (((unsigned char) memory[instPC + 1]) << 8) |
                       (((unsigned char) memory[instPC + 2]) << 16) |
                       (((unsigned char) memory[instPC + 3]) << 24);

            pc = instPC;
            regArray[0]=0;
            instDecExec(instWord);
            instPC += 4;
        }
    } else emitError((char *) "Cannot access input file\n");

    for (int i = 0; i < 32; i++)
        cout << "x" << dec << i << "\t" << "0x" << hex << regArray[i] << "\n";

    return 0;
}