// Assembler for the chip8 machine

#include <stdio.h>
#include "include/parser.h" 
#include <string.h>
#include "include/hash.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

int main(int argc, char* argv[]) {
  FILE* src = NULL;
  HashMap* labelTable = NULL;
  char outputFileName[1024] = "a.ch8";

  if(argc < 2) {
    printf("Assembler usage:\n\tchasm [filename].s8 -o [output-filename]");
    return EXIT_FAILURE;
  }
  src = fopen(argv[1], "r");
  if(argc == 4) {
    strcpy(outputFileName, argv[3]);
    strcat(outputFileName, ".ch8");
  }
  printf("Parsing...\n");
  parse(src, argv[1], outputFileName);
  fclose(src);
  return EXIT_SUCCESS;
}