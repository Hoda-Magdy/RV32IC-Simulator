#include <iostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>

using namespace std;

unsigned int pc = 0;
unsigned int regArray[32] = {0};
unsigned char memory[(16+64)*1024];

void emitError(const char* message) {
    cerr << message;
}
void printPrefix(unsigned int instA, unsigned int instW){
    cout << "0x" << dec << instA << "\t0x" << dec << instW;
}

void handleEcall(unsigned int &instPC) {
    unsigned int a7 = regArray[17]; // Read system call number from register a7
    unsigned int strAddress = regArray[10];
    unsigned char character = memory[strAddress];

    switch (regArray[17]) {
        case 1: // Print integer
            cout << "\tECALL: Print integer\n";
            cout << "Integer to print: " << dec << regArray[10] << endl;
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
                cout << "Integer read: " << dec << regArray[10] << endl;
            }
            break;
        default:
            cout << "\tECALL: Unknown system call\n";
            break;
    }
}

void S_Instructions(unsigned int funct3, unsigned int rs1, unsigned int rs2, unsigned int S_imm, unsigned int &instPC) {
    unsigned int address;
    switch(funct3) {
        case 0:
            cout << "\tSB\tx" <<dec<<rs2 << ", " << (int)S_imm << "(x" <<dec<< rs1 << ")" << "\n";
            address = regArray[rs1] + (int)S_imm;
            memory[address] = (unsigned char)(regArray[rs2] & 0xFF); // Store byte
            instPC += 4;
            break;
        case 1:
            cout << "\tSH\tx" <<dec<< rs2 << ", " << (int)S_imm << "(x" <<dec<< rs1 << ")" << "\n";
            address = regArray[rs1] + (int)S_imm;
            memory[address] = (unsigned char)(regArray[rs2] & 0xFF); // Store halfword (2 bytes)
            memory[address+1] = (unsigned char)((regArray[rs2] >> 8) & 0xFF);
            instPC += 4;
            break;
        case 2:
            cout << "\tSW\tx" <<dec<< rs2 << ", " << (int)S_imm << "(x" <<dec<< rs1 << ")" << "\n";
            address = regArray[rs1] + (int)S_imm;
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
            cout << "\tADDI\tx" <<dec<< rd << ", x" <<dec<< rs1 << ", " << (int)I_imm << "\n";
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
    switch(funct3) {
        case 0:
            cout << "\tLB\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";
            address = regArray[rs1] + (int)I_imm;
            regArray[rd] = (int8_t)memory[address]; // Load byte
            instPC += 4;
            break;
        case 1:
            cout << "\tLH\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";
            address = regArray[rs1] + (int)I_imm;
            regArray[rd] = (int16_t)(memory[address] | (memory[address+1] << 8)); // Load halfword
            instPC += 4;
            break;
        case 2:
            cout << "\tLW\tx" <<dec<< rd << ", " << (int)I_imm << "(x"<<dec << rs1 << ")" << "\n";
            address = regArray[rs1] + (int)I_imm;
            regArray[rd] = memory[address] | (memory[address+1] << 8) | (memory[address+2] << 16) | (memory[address+3] << 24); // Load word
            instPC += 4;
            break;
        case 4:
            cout << "\tLBU\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";
            address = regArray[rs1] + (int)I_imm;
            regArray[rd] = memory[address]; // Load unsigned byte
            instPC += 4;
            break;
        case 5:
            cout << "\tLHU\tx" <<dec<< rd << ", " << (int)I_imm << "(x" <<dec<< rs1 << ")" << "\n";
            address = regArray[rs1] + (int)I_imm;
            regArray[rd] = memory[address] | (memory[address+1] << 8); // Load unsigned halfword
            instPC += 4;
            break;
        default:
            cout << "\tUnknown Load Instruction \n";
    }
}


void U_Instructions(unsigned int rd, unsigned int U_imm, unsigned int &instPC){
    cout << "\tLUI\tx" << dec << rd << ", " << dec << U_imm << "\n";
    regArray[rd] = U_imm;
    instPC += 4;
}

void decode(unsigned int instA, unsigned int &instPC){
    unsigned int opcode, rd, funct3, rs1, rs2, funct7, I_imm, S_imm, B_imm, U_imm, J_imm;
    opcode = instA & 0x7F;
    rd = (instA >> 7) & 0x1F;
    funct3 = (instA >> 12) & 0x07;
    rs1 = (instA >> 15) & 0x1F;
    rs2 = (instA >> 20) & 0x1F;
    funct7 = (instA >> 25) & 0x7F;
    I_imm = (int)instA >> 20;
    S_imm = ((int)instA >> 25) | ((instA >> 7) & 0x1F);
    B_imm = ((int)instA >> 31 << 11) | ((instA >> 7) & 0x1F) | ((instA >> 25) & 0x3F << 5) | ((instA >> 8) & 0x0F << 1);
    U_imm = instA & 0xFFFFF000;
    J_imm = ((int)instA >> 31 << 19) | ((instA >> 21) & 0x3FF << 1) | ((instA >> 20) & 0x1 << 11) | ((instA >> 12) & 0xFF);

    // Check for compressed instructions
    if ((instA & 0x3) == 0x3) {
        // Compressed instruction detected
        unsigned int funct3_c = (instA >> 13) & 0x7;
        unsigned int rs1_c = (instA >> 7) & 0x7;
        unsigned int rs2_c = (instA >> 2) & 0x7;

        switch (funct3_c) {
            case 0:
                // C.ADDI4SPN
                {
                    unsigned int rd_c = (instA >> 7) & 0x7;
                    unsigned int imm = ((instA >> 2) & 0x1F) | ((instA >> 12) & 0x3F) << 5;
                    I_instructions(rd_c, 0, 2, imm, instPC);
                }
                break;
            case 1:
                // C.LW
                {
                    unsigned int rd_c = (instA >> 7) & 0x7;
                    unsigned int imm = ((instA >> 2) & 0x1F) | ((instA >> 12) & 0x01) << 5;
                    I_instructions(rd_c, 3, rs1_c, imm, instPC);
                }
                break;
            case 2:
                // C.SW
                {
                    unsigned int imm = (((instA >> 7) & 0x1C) | ((instA >> 2) & 0x03E) | ((instA >> 12) & 0x01) << 5);
                    S_Instructions(2, rs1_c, rs2_c, imm, instPC);
                }
                break;
            case 5:
                // C.LI
                {
                    unsigned int rd_c = (instA >> 7) & 0x7;
                    unsigned int imm = ((instA >> 2) & 0x1F) | ((instA >> 12) & 0x01) << 5;
                    I_instructions(rd_c, 0, 0, imm, instPC);
                }
                break;
            case 6:
                // C.ADDI
                {
                    unsigned int rd_c = (instA >> 7) & 0x7;
                    unsigned int imm = ((instA >> 2) & 0x1F) | ((instA >> 12) & 0x01) << 5;
                    I_instructions(rd_c, 0, rs1_c, imm, instPC);
                }
                break;
            case 7:
                // C.ADDIW
                {
                    unsigned int rd_c = (instA >> 7) & 0x7;
                    unsigned int imm = ((instA >> 2) & 0x1F) | ((instA >> 12) & 0x01) << 5;
                    I_instructions(rd_c, 1, rs1_c, imm, instPC);
                }
                break;
            default:
                cout << "\tUnknown compressed instruction \n";
                break;
        }
    } else {
        // Handle standard RV32I instructions
        switch(opcode) {
            case 0x33:
                R_instructions(rd, funct3, rs1, rs2, funct7, instPC);
                break;
            case 0x13:
                I_instructions(rd, funct3, rs1, I_imm, instPC);
                break;
             case 0x03: // I Instructions (Loads)
                 IL_instructions(rd, funct3, rs1, I_imm, instPC);
                 break;

            case 0x23:
                S_Instructions(funct3, rs1, rs2, S_imm, instPC);
                break;
            case 0x63:
                B_instructions(funct3, rs1, rs2, B_imm, instPC);
                break;
            case 0x37:
                U_Instructions(rd, U_imm, instPC);
                break;
            case 0x6F:
                J_instructions(rd, J_imm, instPC);
                break;
            case 0x73:
                handleEcall(instPC);
                break;
            default:
                cout << "\tUnknown Instruction \n";
        }
    }
}

int main(int argc, char *argv[]){
    unsigned int instW, instA;
    ifstream source;

    if(argc < 2)
        emitError("\tUsage: simulator <machine_code_file_name> \n");
    source.open(argv[1], ios::binary | ios::in);
    if(!source)
        emitError("\tCannot open file \n");

    pc = 0x0;
    while(1){
        source.read((char *)&instW, 4);
        if(source.eof()) break;
        instA = (instW << 24) | ((instW & 0xFF00) << 8) |
                ((instW >> 8) & 0xFF00) | (instW >> 24);
        printPrefix(pc, instW);
        decode(instA, pc);
    }

    cout << "Program execution completed" << endl;
    source.close();
    return 0;
}
