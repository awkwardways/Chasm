#include "lexer.h"
#include "hash.h"
#include <stdio.h>

#define IS_NUM(x) (x >= 48 && x <= 57)
#define IS_HEX(x) (x >= '0' && x <= '9') || (x >= 'a' && x <= 'f') || (x >= 'A' && x <= 'F')
#define IS_ID(x) (x >= 65 && x <= 90) || (x >= 97 && x <= 122) || (x == '_')

static FILE* src;
static char* peekChar;
static char buffer[BUFFER_SIZE];
static size_t line, column;
static int eofFlag;
static HashMap* symbolTable;
static int badAddressFlag;

static void reloadBuffer() {
  if(fgets(buffer, BUFFER_SIZE, src) == NULL) eofFlag = 1;
  peekChar = buffer;
  column = 0;
}

static void skipLineComment() {
  while(*(peekChar++) != '\n') {}
  reloadBuffer();
}

static void skipBlockComment() {
  while(1) {
    if(*peekChar == '*' && *(peekChar + 1) == '/') {
      column += 2; peekChar += 2;
      return;
    }
    else if(*peekChar == '\n') {
      line++; column = 1;
      reloadBuffer();
    }
    else {
      peekChar++;
    }
  }
}

static void skipWhitespace() {
  while(1 && !eofFlag) {
    if(*peekChar == '\n' || *peekChar == '\0' ) {
      line++; column = 0;
      reloadBuffer();
    }
    else if(*peekChar == '/') {
      if(*(peekChar + 1) == '/') {
        //skip line comment
        skipLineComment();
        line++; 
        column = 0;
      }
      else if(*(peekChar + 1) == '*') {
        //skip block comment
        skipBlockComment();
      }
    }
    else if(*peekChar == ' ' || *peekChar == '\t') {
      column++; peekChar++;
    }
    else break;
  }
}

static uint8_t hexToDec(char c) {
  if(c >= '0' && c <= '9') return (c - 48);
  else if(c >= 'A' && c <= 'F') return (c - 55);
  else if(c >= 'a' && c <= 'f') return (c - 87);
}

static token_t getAddress() {
  token_t temp;
  temp.val = 0;
  peekChar++;
  while(IS_HEX(*(peekChar))) {
    temp.val = temp.val * 16 + (hexToDec(*peekChar));
    column++;
    peekChar++;
  }
  temp.tag = ADDRESS;
  if(*(peekChar) != ' ' && *(peekChar) != '\n' && *(peekChar) != '\t' ) {
    printf("<%d , %d>: SYNTAX ERROR: BAD ADDRESS OR ILLEGAL HEX NUMBER\n", line, column);
    temp.tag = NO_TYPE;
  }
  return temp;
}

static token_t getOrigin() {
  token_t temp; 
  temp = getAddress();
  temp.tag = ORIGIN;
  return temp;
}

static token_t getLexeme() {
  token_t temp;
  size_t idx = 0;
  while(IS_ID(*peekChar) || IS_NUM(*peekChar)) {
    if(*peekChar >= 'a') *peekChar -= 32;
    temp.lexeme[idx++] = *peekChar;
    column++;
    peekChar++;
  }
  temp.lexeme[idx] = '\0';
  if(findElement(symbolTable, temp.lexeme) == NULL) {
    if(*(peekChar) == ':') temp.tag = LABEL;
    else temp.tag = ID;
    insertElement(symbolTable, temp.lexeme, temp.tag);
  }
  else temp.tag = findElement(symbolTable, temp.lexeme)->val;
  return temp;
}

static token_t getNum() {
  token_t temp;
  temp.val = 0;
  while(IS_NUM(*peekChar)) {
    temp.val = temp.val * 16 + hexToDec(*peekChar);
    peekChar++;
    column++;
  }
  temp.tag = NUM;
  return temp;
}

void initLexer(FILE* source, HashMap* symTab) {
  symbolTable = symTab;
  src = source;
  eofFlag = 0;
  line = 1;
  column = 0;
  peekChar = NULL;
  badAddressFlag = 0;
  reloadBuffer();
}

token_t getNextToken() {
  token_t token = {0, 0};
  //Skip whitespace
  skipWhitespace();
  //Check for id
  if(IS_ID(*peekChar)) {
    token = getLexeme();
  }
  //check for num
  else if(IS_NUM(*peekChar)) {
    token = getNum();
  }

  // else if(*peekChar == '$') {
  //   printf("address found\n");
  //   token = getAddress();
  // }

  //return char found
  else {
    token.tag = NO_TYPE;
    switch(*peekChar) {
      case '>':
      case '=':
      case '<':
        if(*(peekChar + 1) == '=') {
          token.lexeme[0] = *peekChar; 
          token.lexeme[1] = '='; 
          token.lexeme[2] = '\0'; 
          peekChar+=2;
          column+=2;
        }
        else {
          token.lexeme[0] = *peekChar;
          token.lexeme[1] = '\0';
          peekChar++;
          column++;
        }
      break;

      case '+':
      case '-':
        if(*(peekChar + 1) == *peekChar) {
          token.lexeme[0] = *peekChar;
          token.lexeme[1] = *peekChar;
          token.lexeme[2] = '\0';
          peekChar+=2;
          column+=2;
        }
        else{
          token.lexeme[0] = *peekChar;
          token.lexeme[1] = '\0';
          peekChar++;
          column++;
        }
      break;

      case '$':
        token = getAddress();
      break;

      case '.':
        token = getOrigin();
      break;

      case '@':
        token = getAddress();
        token.tag = AT;
      break;

      default:
        token.lexeme[0] = *peekChar;
        token.lexeme[1] = '\0'; 
          peekChar++;
          column++;
      break;
    }
    if(findElement(symbolTable, token.lexeme) != NULL) {
      token.tag = findElement(symbolTable, token.lexeme)->val; 
    }
  }
  if(eofFlag) {
    token.tag = EndOfFile;
    token.val = -1;
    return token;
  }
  token.line = line;
  token.column = column;
  return token;
}
