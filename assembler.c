#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *ltrim(char *s) {
	while (*s == ' ' || *s == '\t') s++;
	return s;
}

int getRegister(char *text) {
	if (*text == 'r' || *text=='R') text++;
	return atoi(text);
}

int assembleLine(char *text, unsigned char* bytes) {
	text = ltrim(text);
	char *keyWord = strtok(text," ");
	if (strcmp("add",keyWord) == 0) {
		bytes[0] = 0x10;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}

	if (strcmp("addimmediate",keyWord) == 0) {
		bytes[0] = 0x90;
		bytes[0] |= getRegister(strtok(NULL," "));
		int immediate = getRegister(strtok(NULL," "));
		if (immediate < 0){
			bytes[1] = 0x80;
			immediate*=-1;
		} 
		bytes[1] |= immediate;
		return 2;
	}

	if (strcmp("and",keyWord) == 0) {
		bytes[0] = 0x20;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}

	if (strcmp("branchifequal",keyWord) == 0) {
		bytes[0] = 0xa0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4;
		int offset = getRegister(strtok(NULL," "));
		if (offset < 0){
			bytes[1] |= 0x8;
			offset*=-1;
		} 
		int top4_bits = offset >> 16;
		bytes[1] |= top4_bits & 0x0000000f;
		bytes[2] = (offset >> 8) & 0xff;
		bytes[3] = offset & 0x00FF;
		return 4;
	}

	if (strcmp("branchifless",keyWord) == 0) {
		bytes[0] = 0xb0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4;
		int offset = getRegister(strtok(NULL," "));
		if (offset < 0){
			bytes[1] |= 0x8;
			offset*=-1;
		} 
		int top4_bits = offset >> 16;
		bytes[1] |= top4_bits & 0x0000000f;		
		bytes[2] = (offset >> 8) & 0xff;
		bytes[3] = offset & 0x00FF;
		return 4;
	}

	if (strcmp("divide",keyWord) == 0) {
		bytes[0] = 0x30;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}

	if (strcmp("halt",keyWord) == 0 || strcmp("halt\n", keyWord) == 0) {
		bytes[0] = 0x00;
		bytes[1] = 0x00;
		return 2;
	}

	if (strcmp("interrupt",keyWord) == 0) {
		bytes[0] = 0x80;
		unsigned int interrupt = getRegister(strtok(NULL," "));
		bytes[0] |= interrupt >> 8 & 0x0f;
		bytes[1] = interrupt & 0x0FF;
		return 2;
	}

	if (strcmp("iterateover",keyWord) == 0) {
		bytes[0] = 0xd0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," "));
		int offset = getRegister(strtok(NULL," "));
		bytes[2] = offset >> 8;
		bytes[3] = offset & 0x00FF;
		return 4;
	}

	if (strcmp("jump",keyWord) == 0) {
		bytes[0] = 0xc0;
		int offset = getRegister(strtok(NULL," "));
		int top12_bits = offset >> 16;
		bytes[0] |= top12_bits >> 8 & 0x0f;
		bytes[1] = top12_bits & 0x0FF;
		bytes[2] = offset >> 8 & 0xff;
		bytes[3] = offset & 0x00FF;
		return 4;
	}

	if (strcmp("leftshift",keyWord) == 0) {
		bytes[0] = 0x70;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = 0x00;
		bytes[1] |= getRegister(strtok(NULL," "));
		return 2;
	}

	if (strcmp("load",keyWord) == 0) {
		bytes[0] = 0xe0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL, " "));
		int immediate = getRegister(strtok(NULL," "));
		if (immediate < 0){
			bytes[1] = 0x8;
			immediate*=-1;
		} 
		bytes[1] = bytes[1] << 4 | immediate;
		return 2;
	}

	if (strcmp("multiply",keyWord) == 0) {
		bytes[0] = 0x40;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}

	if (strcmp("or",keyWord) == 0) {
		bytes[0] = 0x60;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}

	if (strcmp("rightshift",keyWord) == 0) {
		bytes[0] = 0x70;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = 0x20;
		bytes[1] |= getRegister(strtok(NULL," "));
		return 2;
	}

	if (strcmp("store",keyWord) == 0) {
		bytes[0] = 0xf0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL, " "));
		int immediate = getRegister(strtok(NULL," "));
		if (immediate < 0){
			bytes[1] = 0x8;
			immediate*=-1;
		} 
		bytes[1] = bytes[1] << 4 | immediate;
		return 2;
	}

	if (strcmp("subtract",keyWord) == 0) {
		bytes[0] = 0x60;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 | getRegister(strtok(NULL," "));
		return 2;
	}

	return 0;
}

int main(int argc, char **argv) {
	FILE *src = fopen(argv[1],"r");
	FILE *dst = fopen(argv[2],"w");
	while (!feof(src)) {
		unsigned char bytes[4];
		memset(bytes, 0, strlen(bytes));
		char line[1000];
		printf ("about to read\n");
		if (NULL != fgets(line, 1000, src)) {
			printf ("read: %s\n",line);
			int byteCount = assembleLine(line,bytes);
			fwrite(bytes,byteCount,1,dst);
		}
	}
	fclose(src);
	fclose(dst);
	return 0;
}
