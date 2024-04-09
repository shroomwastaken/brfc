/*
	brainfuck interpreter
	(c) 2024 - shroomwastaken
*/

#include <stdio.h>
#include <stdlib.h>

#define MAX_TAPE_LEN 30000
#define MAX_LOOPS 128

int main(int argc, char* argv[]) {
	// setup
	if (argc != 2) { printf("usage: brfc <file name>\n"); return 1; }
	FILE* file = fopen(argv[1], "rb");
	if (file == NULL) { printf("could not open file!\n"); return 1; }
	fseek(file, 0, SEEK_END);
	unsigned long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* p = malloc(file_size + 1);
	unsigned long size = fread(p, 1, file_size, file);
	p[size] = 0x00; // null terminator

	// interpreting
	unsigned char* tape = calloc(MAX_TAPE_LEN, sizeof(char));
	unsigned long* l_inds = malloc(MAX_LOOPS);
	unsigned long pc = 0;
	int tc = 0;
	unsigned char lc = 0;
	char broken = 0;
	unsigned char open_brackets = 0; // idk why i did this
	while (p[pc]) {
		switch (p[pc]) {
			case '+': tape[tc]++; break;
			case '-': tape[tc]--; break;
			case '>': if (tc + 1 == MAX_TAPE_LEN) tc = -1; tc++; break;
			case '<': if (!tc) tc = MAX_TAPE_LEN; tc--; break;
			case '.': printf("%c", tape[tc]); break;
			case ',': tape[tc] = getchar(); break;
			case '[':
				if (lc + 1 == MAX_LOOPS) {
					printf(">128 loops!\n"); broken = 1; break;
				}
				if (!tape[tc]) {
					open_brackets++;
					while (p[pc] != ']' || open_brackets != 0) {
						pc++;
						if (p[pc] == '[') open_brackets++;
						if (p[pc] == ']') open_brackets--;
						if (pc == file_size + 1) { broken = 1; break; }
					} break;
				} else l_inds[lc++] = pc;
				break;
			case ']':
				if (!lc) { printf("unexpected ]\n"); broken = 1; break; }
				if (tape[tc]) pc = l_inds[lc - 1]; else lc--;
				break;
		}
		pc++; if (broken) break;
	}

	printf("\n"); // flush stdout
	free(tape); free(p); free(l_inds); fclose(file); return 0;
}
