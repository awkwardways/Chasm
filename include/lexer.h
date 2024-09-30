#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "hash.h"
#define BUFFER_SIZE 512

typedef enum {
  ID = 256, NUM, AT, DOT, COLON, ORIGIN, LABEL, ADDRESS, CLS, JMP, RET, CALL, SKPE, SKPD, MOV, ADD, OR, AND, XOR, SUB, SHR, SHL, STI, JMPO,
  RND, DRW, SKPK, SKPNK, GDLY, SDLY, GKEY, SSND, ADDI, BCD, REGD, REGL, REG, COMMA, NO_TYPE, EndOfFile  
  } tokenTag;

typedef struct Token {
  tokenTag tag;
  unsigned int line, column;
  union {
    char lexeme[256];
    uint16_t val;
  };
} token_t;

void initLexer(FILE* source, HashMap* symbolTable);

token_t getNextToken();
#endif
