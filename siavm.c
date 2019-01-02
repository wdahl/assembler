#include <stdio.h>
#include <string.h>
#include <stdbool.h>

unsigned char memory[10000];
bool halt = false;

struct CPU{
    int registers[16];
    int PC;
    int OP1;
    int OP2;
    int result;
    unsigned char current_instruction[4];
}cpu;

void printRegesters(){
    int i;
    for(i=0; i<16; i++){
        printf("R%d: %d\n", i+1, cpu.registers[i]);
    }
}

void printMemory(){
    int i;
    for(i=0; i<65; i++){
        printf("$%d: %x\n", i, memory[i]);
    }
}

void add(){
    cpu.result = cpu.OP1 + cpu.OP2;
    cpu.PC += 2;
}

void addimmediate(){
    unsigned char i = cpu.current_instruction[1];
    int s;
    if((i & 0x8) == 0x8){
        i &= 0x7f;
        s = i;
        s *= -1;
    }
    cpu.result = s;
    cpu.PC += 2; 
}

void and(){
    cpu.result = cpu.OP1 & cpu.OP2;
    cpu.PC += 2;
}

void branchifequal(){
    unsigned int top4bits = cpu.current_instruction[1] & 0x0f;
    unsigned int bottom16bits = (cpu.current_instruction[2] << 8) | cpu.current_instruction[3];
    unsigned int offset = top4bits << 16 | bottom16bits;
    if((offset >> 19) == 1){
        offset = offset << 13;
        offset = offset >> 13;
        offset *= -1;
    }
    if(cpu.OP1 == cpu.OP2){
        cpu.PC += offset;
    }
    else{
        cpu.PC += 4;
    }
}

void branchifless(){
    unsigned int top4bits = cpu.current_instruction[1] & 0x0f;
    unsigned int bottom16bits = (cpu.current_instruction[2] << 8) | cpu.current_instruction[3];
    unsigned int offset = top4bits << 16 | bottom16bits;
    if((offset >> 19) == 1){
        offset = offset << 13;
        offset = offset >> 13;
        offset *= -1;
    }
    if(cpu.OP1 < cpu.OP2){
        cpu.PC += offset;
    }
    else{
        cpu.PC += 4;
    }
}

void divide(){
    cpu.result = cpu.OP1 / cpu.OP2;
    cpu.PC += 2;
}

void interrupt(){
    int code = ((cpu.current_instruction[0] & 0x0f) << 8) | cpu.current_instruction[1];
    if(code == 0){
        printRegesters();
    }

    else{
        printMemory();
    }

    cpu.PC += 2;
}

void iterateover(){
    int nextPointerOffset = cpu.current_instruction[1];
    int offset = (cpu.current_instruction[2] << 8) | cpu.current_instruction[3];
    int nextPointer = cpu.OP1 + nextPointerOffset;
    if(memory[nextPointer] == 0){
        cpu.PC += 4;
    }
    else{
        cpu.PC -= offset;
        cpu.result = nextPointer;
    }
}

void jump(){
    int top12bits = ((cpu.current_instruction[0] & 0x0f) << 8) | cpu.current_instruction[1];
    int bottom16bits = (cpu.current_instruction[2] << 8) | cpu.current_instruction[3];
    int offset = (top12bits << 16) | bottom16bits;
    cpu.PC = offset;
}

void shift(){
    int amount = cpu.current_instruction[1] & 0x1f;
    if((cpu.current_instruction[1] & 0x20) == 0){
        cpu.result = cpu.OP1 << amount;
    }

    else{
        cpu.result = cpu.OP1 >> amount;
    }

    cpu.PC += 2;
}

void load(){
    signed char offset = (signed char)cpu.current_instruction[1] & 0x0f;
    if(offset < 0){
        offset = offset << 1;
        offset = offset >> 1;
        offset *= -1;
    }
    cpu.result = memory[cpu.OP2 + offset];
    cpu.PC += 2;
}

void multiply(){
    cpu.result = cpu.OP1 * cpu.OP2;
    cpu.PC += 2;
}

void or(){
    cpu.result = cpu.OP1 | cpu.OP2;
    cpu.PC += 2;
}

void store(){
    cpu.result = cpu.OP1;
    cpu.PC += 2;
}

void subtract(){
    cpu.result = cpu.OP1 - cpu.OP2;
    cpu.PC += 2;
}

void read_file(char *file){
    FILE *fp = fopen(file, "r");
    int i = 0;
    while(!feof(fp)){
        memory[i] = fgetc(fp);
        i++;
    }
}

void fetch(){
    memcpy(cpu.current_instruction, memory + cpu.PC, 4);
}

void dispatch(){
    cpu.OP1 = cpu.registers[(cpu.current_instruction[0] & 0x0f)-1];
    cpu.OP2 = cpu.registers[((cpu.current_instruction[1] >> 4) & 0x0f)-1];
}

void execute(){
    switch((cpu.current_instruction[0] >> 4) & 0x0f){
        case 1:
            add();
            break;
        
        case 9:
            addimmediate();
            break;
        
        case 2:
            and();
            break;

        case 10:
            branchifequal();
            break;

        case 11:
            branchifless();
            break;

        case 3:
            divide();
            break;

        case 8:
            interrupt();
            break;

        case 0:
            halt = 1;
            break;

        case 13:
            iterateover();
            break;
        
        case 12:
            jump();
            break;

        case 7:
            shift();
            break;

        case 14:
            load();
            break;

        case 4:
            multiply();
            break;

        case 6:
            or();
            break;

        case 15:
            store();
            break;

        case 5:
            subtract();
            break;
    }
}

void storeCPU(){
    signed char offset = (signed char)cpu.current_instruction[1] & 0x0f;
    switch((cpu.current_instruction[0] >> 4) & 0x0f){
        case 1:
            cpu.registers[(cpu.current_instruction[1] & 0x0f)-1] = cpu.result;
            break;
        
        case 9:
            cpu.registers[(cpu.current_instruction[0] & 0x0f)-1] = cpu.result;
            break;
        
        case 2:
            cpu.registers[(cpu.current_instruction[1] & 0x0f)-1] = cpu.result;
            break;

        case 3:
            cpu.registers[(cpu.current_instruction[1] & 0x0f)-1] = cpu.result;
            break;

        case 13:
            cpu.registers[(cpu.current_instruction[0] & 0x0f)-1] = cpu.result;
            break;

        case 7:
            cpu.registers[(cpu.current_instruction[0] & 0x0f)-1] = cpu.result;
            break;

        case 14:
            cpu.registers[(cpu.current_instruction[0] & 0x0f)-1] = cpu.result;
            break;

        case 4:
            cpu.registers[(cpu.current_instruction[1] & 0x0f)-1] = cpu.result;
            break;

        case 6:
            cpu.registers[(cpu.current_instruction[1] & 0x0f)-1] = cpu.result;;
            break;

        case 15:
            if(offset < 0){
                offset = offset << 1;
                offset = offset >> 1;
                offset *= -1;
            }

            memory[cpu.OP2 + offset] = cpu.result;
            break;

        case 5:
            cpu.registers[(cpu.current_instruction[1] & 0x0f)-1] = cpu.result;
            break;
    }
}

int main(int argc, char *argv[]){
    cpu.PC = 0;
    read_file(argv[1]);

    while(!halt){
        fetch();
        dispatch();
        execute();
        storeCPU();
    }

    return 0;
}