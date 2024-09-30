#ifndef CODEGEN_H
#define CODEGEN_H

#include "hash.h"

int genCode(HashMap* symbolTable, HashMap* labelTable, const char* srcName, const char* outputFileName);

#endif
