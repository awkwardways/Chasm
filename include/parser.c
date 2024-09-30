#include "lexer.h"
#include "hash.h"
#include "codegen.h"
#include "parser.h"
#include <stdio.h>

static token_t currentToken = {0, 0, 0};
static uint16_t address = 0x200;
static HashMap* labelTable = NULL;
static token_t tempToken = {0, 0, 0};
static HashMap* symbolTable = NULL;

int list();
int instruction();

int parse(FILE* src, const char* srcName, const char* outputFileName) {
  symbolTable = initHashMap(523);
  labelTable = initHashMap(123);

  initLexer(src, symbolTable);

  insertElement(symbolTable, "CLS", CLS);
  insertElement(symbolTable, "JMP", JMP);
  insertElement(symbolTable, "RET", RET);
  insertElement(symbolTable, "CALL", CALL);
  insertElement(symbolTable, "SKPE", SKPE);
  insertElement(symbolTable, "SKPD", SKPD);
  insertElement(symbolTable, "MOV", MOV);
  insertElement(symbolTable, "ADD", ADD);
  insertElement(symbolTable, "OR", OR);
  insertElement(symbolTable, "XOR", XOR);
  insertElement(symbolTable, "SUB", SUB);
  insertElement(symbolTable, "SHR", SHR);
  insertElement(symbolTable, "SHL", SHL);
  insertElement(symbolTable, "STI", STI);
  insertElement(symbolTable, "JMPO", JMPO);
  insertElement(symbolTable, "RND", RND);
  insertElement(symbolTable, "DRW", DRW);
  insertElement(symbolTable, "SKPK", SKPK);
  insertElement(symbolTable, "SKPNK", SKPNK);
  insertElement(symbolTable, "GDLY", GDLY);
  insertElement(symbolTable, "SDLY", SDLY);
  insertElement(symbolTable, "GKEY", GKEY);
  insertElement(symbolTable, "SSND", SSND);
  insertElement(symbolTable, "ADDI", ADDI);
  insertElement(symbolTable, "BCD", BCD);
  insertElement(symbolTable, "REGD", REGD);
  insertElement(symbolTable, "REGL", REGL);
  insertElement(symbolTable, "V0", REG);
  insertElement(symbolTable, "V1", REG);
  insertElement(symbolTable, "V2", REG);
  insertElement(symbolTable, "V3", REG);
  insertElement(symbolTable, "V4", REG);
  insertElement(symbolTable, "V5", REG);
  insertElement(symbolTable, "V6", REG);
  insertElement(symbolTable, "V7", REG);
  insertElement(symbolTable, "V8", REG);
  insertElement(symbolTable, "V9", REG);
  insertElement(symbolTable, "VA", REG);
  insertElement(symbolTable, "VB", REG);
  insertElement(symbolTable, "VC", REG);
  insertElement(symbolTable, "VD", REG);
  insertElement(symbolTable, "VE", REG);
  insertElement(symbolTable, "VF", REG);
  insertElement(symbolTable, ",", COMMA);
  insertElement(symbolTable, ":", COLON);
  currentToken = getNextToken();
  if(list()) {
    //generate code
    fclose(src);
    genCode(symbolTable, labelTable, srcName, outputFileName);
    deleteHashMap(symbolTable);
    deleteHashMap(labelTable);
    return 0;
  }
  else {
    deleteHashMap(symbolTable);
    deleteHashMap(labelTable);
    return 1;
  }
}

int list() {
  if(instruction()) {
    currentToken = getNextToken();
    if(currentToken.tag == EndOfFile) {
      printf("Parse successful!\n");
      return 1;
    }
    else {
      return list();
    }
  }
  else return 0;
}

int args(const char* instruction) {
  if(currentToken.tag != REG) {
    printf("<%d, %d>: SYNTAX ERROR: FIRST OPERAND IN %s INSTRUCTION MUST BE A REGISTER\n", currentToken.line, currentToken.column, instruction);
    return 0;
  }
  currentToken = getNextToken();
  if(currentToken.tag != COMMA) {
    printf("<%d, %d>: SYNTAX ERROR: MISSING COMMA\n", currentToken.line, currentToken.column);         
    return 0;
  }
  currentToken = getNextToken();
  if(currentToken.tag != REG && currentToken.tag != NUM) {
    printf("<%d, %d>: SYNTAX ERROR: SECOND OPERAND IN %s INSTRUCTION MUST BE A REGISTER OR AN 8 BIT NUMBER\n", currentToken.line, currentToken.column, instruction);
    return 0;
  }
  return 1;
}

int instruction() {
  switch(currentToken.tag) {
    case AT:
      currentToken = getNextToken();
      if(currentToken.tag != NUM) {
        printf("<%d, %d>: SYNTAX WARNING: EMPTY DEFINITION. NOTHING WILL BE WRITTEN TO THE SPECIFIED ADDRESS %d\n", currentToken.line, currentToken.column, currentToken.tag);
      }
    break;
    case ORIGIN: break;
    case LABEL:
      tempToken = currentToken;
      currentToken = getNextToken();
      if(currentToken.tag != COLON) {
        printf("<%d, %d>: SYNTAX ERROR: SECTION LABEL MUST BE FOLLOWED BY A COLON\n", currentToken.line, currentToken.column);
        return 0;
      }
      //register tag
      insertElement(labelTable, tempToken.lexeme, address);
      address -= 2;
    break;
    case CLS: break;

    case RET: break;

    case JMP:
      currentToken = getNextToken();
      // printf("Current token is %d\n", currentToken.tag);
      if(currentToken.tag != ADDRESS && currentToken.tag != LABEL && currentToken.tag != ID) {
        printf("<%d, %d>: SYNTAX ERROR: JMP STATEMENT MUST TO BE FOLLOWED BY A 12 BIT ADDRESS\n", currentToken.line, currentToken.column);
        return 0;
      }
      else break;
    break;

    case CALL:
      currentToken = getNextToken();
      if(currentToken.tag != ID && currentToken.tag != LABEL && currentToken.tag != ADDRESS) {
        printf("<%d, %d>: SYNTAX ERROR: CALL STATEMENTE MUST BE FOLLOWED BY A SUBROUTINE IDENTIFIER\n", currentToken.line, currentToken.column);
        return 0;
      }
      else break;
    break;

    case SKPE:
      currentToken = getNextToken();
      if(args("CONDITIONAL")) break;
      else return 0;
    break;

    case SKPD:
      currentToken = getNextToken();
      if(args("CONDITIONAL")) break;
      else return 0;
    break;

    case MOV:
      currentToken = getNextToken();
      if(args("MOVE")) break;
      else return 0;
    break;

    case ADD:
      currentToken = getNextToken();
      if(args("ADDITION")) break;
      else return 0;
    break;

    case OR: 
      currentToken = getNextToken();
      if(args("OR")) break;
      else return 0;
    break;

    case AND:
      currentToken = getNextToken();
      if(args("AND")) break;
      else return 0;
    break;

    case XOR:
      currentToken = getNextToken();
      if(args("XOR")) break;
      else return 0;
    break;

    case SUB:
      currentToken = getNextToken();
      if(args("SUBSTRACTION")) break;
      else return 0;
    break;

    case SHR:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d, %d>: SYNTAX ERROR: SHIFT INSTRUCTION MUST BE FOLLOWED BY REGISTER TO BE SHIFTED\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case SHL:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d, %d>: SYNTAX ERROR: SHIFT INSTRUCTION MUST BE FOLLOWED BY REGISTER TO BE SHIFTED\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case STI:
      currentToken = getNextToken();
      if(currentToken.tag != ADDRESS) {
        printf("<%d, %d>: SYNTAX ERROR: SET INSTRUCTION MUST BE FOLLOWED BY AN ADDRESS TO SET I TO\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case JMPO:
      currentToken = getNextToken();
      if(currentToken.tag != ADDRESS) {
        printf("<%d, %d>: SYNTAX ERROR: JUMP WITH OFFSET INSTRUCTION MUST BE FOLLOWED BY AND ADDRESS TO JUMP TO\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case RND:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d, %d>: SYNTAX ERROR: RND INSTRUCTION MUST BE FOLLOWED BY A VALID REGISTER NAME\n", currentToken.line, currentToken.column);
        return 0;
      }
      getNextToken(); currentToken = getNextToken();
      if(currentToken.tag != NUM) {
        printf("<%d, %d>: SYNTAX ERROR: RND INSTRUCTION MUST BE FOLLOWED BY AN 8 BIT NUMBER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case DRW:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: DRW INSTRUCTION MUST BE FOLLOWED BY 2 REGISTERS AND A 4 BIT NUMBER\n", currentToken.line, currentToken.column);
        return 0;
      }
      currentToken = getNextToken();
      if(currentToken.tag != COMMA) {
        printf("<%d , %d>: SYNTAX ERROR: MISSING COMMA IN LINE %d\n", currentToken.line);
        return 0;
      }
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: DRW INSTRUCTION MUST BE FOLLOWED BY 2 REGISTERS AND A 4 BIT NUMBER\n", currentToken.line, currentToken.column);
        return 0;
      }
      currentToken = getNextToken();
      if(currentToken.tag != COMMA) {
        printf("<%d , %d>: SYNTAX ERROR: MISSING COMMA IN LINE %d\n", currentToken.line);
        return 0;
      }
      currentToken = getNextToken();
      if(currentToken.tag != NUM) {
        printf("<%d , %d>: SYNTAX ERROR: DRW INSTRUCTION MUST BE FOLLOWED BY 2 REGISTERS AND A 4 BIT NUMBER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case SKPK:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: SKIP ON KEY INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case SKPNK:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: SKIP ON NOT KEY INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case GDLY:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: GET DELAY INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case SDLY:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: SET DELAY INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case GKEY:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: GET KEY INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case SSND:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: SET SOUND INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case ADDI:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: ADDITION TO I INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case BCD:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: BCD INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case REGD:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: REG DUMP INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case REGL:
      currentToken = getNextToken();
      if(currentToken.tag != REG) {
        printf("<%d , %d>: SYNTAX ERROR: REG LOAD INSTRUCTION MUST BE FOLLOWED BY A REGISTER\n", currentToken.line, currentToken.column);
        return 0;
      }
    break;

    case ID:
      tempToken = currentToken;
      currentToken = getNextToken();
      if(currentToken.tag != COLON) {
        printf("<%d , %d>: SYNTAX ERROR: ID MUST BE PRECEDED BY AN INSTRUCTION OR FOLLOWED BY A COLON TO FORM A TAG\n", currentToken.line, currentToken.column);
        return 0;
      }
      insertElement(labelTable, tempToken.lexeme, address);
      address -= 2;
    break;

    default:
      printf("<%d , %d>: SYNTAX ERROR: UNIDENTIFIED INSTRUCTION WITH TAG %d\n", currentToken.line, currentToken.column, currentToken.tag);
      return 0;
    break;
  }
  address += 2;
  return 1;
}
