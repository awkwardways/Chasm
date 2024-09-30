#include "codegen.h"
#include "lexer.h"

static void errorExit(FILE* src) {
  fclose(src);
  src = NULL;
}

static uint8_t charToRegister(char c) {
  if(c >= '0' && c <= '9') return (c - 48);
  else if(c >= 'A' && c <= 'F') return (c - 55);
}

int genCode(HashMap* symbolTable, HashMap* labelTable, const char* srcName, const char* outputFileName) {
  FILE* src = fopen(srcName, "r");  //This pointer has to be deallocated before returning control to
                                    //the parser function
  //The whole source file has to be read and the mnemonics have to be translated to opcodes
  //This will be done with the lexer and with help of the symbol and label table.
  
  initLexer(src, symbolTable);  //initialize lexer for code generator with the symbol table that the parser modified
  token_t currentToken = {0, 0, 0};
  uint8_t programBuffer[4096];
  size_t  programBufferIdx = 0;
  uint16_t currentInstruction = 0x0000;
  uint16_t programSize = 0;
  node* tempNode = NULL;

  currentToken = getNextToken();

  while(currentToken.tag != EndOfFile) {
    switch(currentToken.tag) {
      case AT:
        currentInstruction = getNextToken().val;
        programBuffer[currentToken.val] = currentInstruction & 0x00ff;
        programSize = currentToken.val + 1;
      break;
      case ORIGIN:
        currentInstruction = 0;
        while(programBufferIdx < currentToken.val) {
          programBuffer[programBufferIdx++] = 0;
        }
      break;
      case ID:
      case LABEL:
        (void)getNextToken();
      break;
      
      case CLS:
        currentInstruction = 0x00e0;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case RET:
        currentInstruction = 0x00ee;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case JMP:
        currentToken = getNextToken();
        if(currentToken.tag == ADDRESS) {
          currentInstruction = 0x1000 ^ currentToken.val;
          programBuffer[programBufferIdx++] = currentInstruction >> 8;
          programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
          break;
        }
        tempNode = findElement(labelTable, currentToken.lexeme);
        if(tempNode == NULL) {
          printf("<%d , %d>: CODE GENERATION ERROR: LABEL NOT DEFINED\n", currentToken.line, currentToken.column);
          errorExit(src);
          return 1;
        }
        currentInstruction = 0x1000 ^ tempNode->val;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case CALL:
        currentToken = getNextToken();
        if(currentToken.tag == ADDRESS) {
          currentInstruction = 0x2000 ^ currentToken.val;
          break;
        }
        tempNode = findElement(labelTable, currentToken.lexeme);
        if(tempNode == NULL) {
          printf("<%d , %d>: CODE GENERATION ERROR: LABEL NOT DEFINED\n", currentToken.line, currentToken.column);
          errorExit(src);
          return 1;
        }
        currentInstruction = 0x2000 ^ tempNode->val;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SKPE:  //Can compare register-number or register-register
        currentInstruction = 0x0000;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        if(currentToken.tag == NUM) {
          currentInstruction ^= 0x3000 ^ currentToken.val;
        }
        else {
          currentInstruction ^= 0x5000 ^ (charToRegister(currentToken.lexeme[1]) << 4); 
        }
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SKPD:  //Can also compare register-number or register-register
        currentInstruction = 0x0000;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        if(currentToken.tag == NUM) {
          currentInstruction ^= 0x4000 ^ currentToken.val;
        }
        else {
          currentInstruction ^= 0x9000 ^ (charToRegister(currentToken.lexeme[1]) << 4); 
        }
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case MOV: //Can assign a value to a register or a register's value to a register
        currentInstruction = 0x0000;
        currentToken = getNextToken();
        currentInstruction ^= ((charToRegister(currentToken.lexeme[1])) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        if(currentToken.tag == NUM) {
          currentInstruction ^= 0x6000 ^ currentToken.val;
        }
        else {
          currentInstruction ^= 0x8000 ^ (charToRegister(currentToken.lexeme[1]) << 4); 
        }
        programBuffer[programBufferIdx++] = (currentInstruction&0xff00) >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case ADD: //Can add a value to a register or a register's value to a register
        currentInstruction = 0x0000;
        currentToken = getNextToken();
        currentInstruction ^= ((charToRegister(currentToken.lexeme[1])) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        if(currentToken.tag == NUM) {
          currentInstruction ^= 0x7000 ^ currentToken.val;
        }
        else {
          currentInstruction ^= 0x8004 ^ (charToRegister(currentToken.lexeme[1]) << 4); 
        }
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case OR:
        currentInstruction = 0x8001;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 4); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case AND:
        currentInstruction = 0x8002;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 4); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case XOR:
        currentInstruction = 0x8003;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 4); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SUB:
        currentInstruction = 0x8005;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 4);
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SHR:
        currentInstruction = 0x8006;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SHL:
        currentInstruction = 0x800e;
        currentToken = getNextToken();
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 8); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case STI:
        currentInstruction = 0xa000;
        currentToken = getNextToken();
        currentInstruction ^= currentToken.val;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case JMPO:
        currentToken = getNextToken();
        tempNode = findElement(labelTable, currentToken.lexeme);
        if(tempNode == NULL) {
          printf("<%d , %d>: CODE GENERATION ERROR: LABEL NOT DEFINED\n", currentToken.line, currentToken.column);
          errorExit(src);
          return 1;
        }
        currentInstruction = 0xb000 ^ tempNode->val;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case RND:
        currentToken = getNextToken();
        currentInstruction = 0xc000 ^ (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        currentInstruction ^= currentToken.val;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case DRW:
        currentToken = getNextToken();
        currentInstruction = 0xd000 ^ (charToRegister(currentToken.lexeme[1]) << 8); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        currentInstruction ^= (charToRegister(currentToken.lexeme[1]) << 4); 
        getNextToken(); currentToken = getNextToken();  //First function call will return a comma token, so it is not stored
        currentInstruction ^= currentToken.val;
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SKPK:
        currentToken = getNextToken();
        currentInstruction = 0xe09e ^ (charToRegister(currentToken.lexeme[1]) << 8); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SKPNK:
        currentToken = getNextToken();
        currentInstruction = 0xe0a1 ^ (charToRegister(currentToken.lexeme[1]) << 8); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case GDLY:
        currentToken = getNextToken();
        currentInstruction = 0xf007 ^ (charToRegister(currentToken.lexeme[1]) << 8); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case GKEY:
        currentToken = getNextToken();
        currentInstruction = 0xf00a ^ (charToRegister(currentToken.lexeme[1]) << 8); 
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SDLY:
        currentToken = getNextToken();
        currentInstruction = 0xf015 ^ (charToRegister(currentToken.lexeme[1]) << 8);
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case SSND:
        currentToken = getNextToken();
        currentInstruction = 0xf018 ^ (charToRegister(currentToken.lexeme[1]) << 8);
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case ADDI:
        currentToken = getNextToken();
        currentInstruction = 0xf01e ^ (charToRegister(currentToken.lexeme[1]) << 8);
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;
      
      case BCD:
        currentToken = getNextToken();
        currentInstruction = 0xf033 ^ (charToRegister(currentToken.lexeme[1]) << 8);
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case REGL:
        currentToken = getNextToken();
        currentInstruction = 0xf055 ^ (charToRegister(currentToken.lexeme[1]) << 8);
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      case REGD:
        currentToken = getNextToken();
        currentInstruction = 0xf065 ^ (charToRegister(currentToken.lexeme[1]) << 8);
        programBuffer[programBufferIdx++] = currentInstruction >> 8;
        programBuffer[programBufferIdx++] = currentInstruction & 0x00ff;
      break;

      default:
        printf("<%d, %d>: CODE GENERATION ERROR: CURRENT TOKEN IS NOT INSTRUCTION. TOKEN TAG IS %d\n", currentToken.line, currentToken.column, currentToken.tag);
        currentToken = getNextToken();
        printf("NEXT TOKEN HAS TAG %d\n", currentToken.tag);
        errorExit(src);
        return 1;
      break;
    }
    currentToken = getNextToken();
  }
  if(programBufferIdx > programSize) programSize = programBufferIdx;
  printf("Code generated successfully! %d bytes written\n", programSize);  
  FILE* out = fopen(outputFileName, "wb");
  fwrite(programBuffer, sizeof(uint8_t),  programSize, out);
  fclose(out);
  fclose(src);
  return 0;
}
