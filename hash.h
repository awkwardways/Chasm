#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#include <stdint.h>

typedef struct node {
  int val;
  char key[512];
  struct node* next;
} node; 

typedef struct HashMap {
  size_t mapSize, count;
  node* map;
} HashMap;

HashMap* initHashMap(size_t initialMapSize);

HashMap* deleteHashMap(HashMap* map);

uint64_t insertElement(HashMap* hashmap, const char* _key, int value);

node* findElement(HashMap* hashmap, const char* key);

void deleteElement(const char* key, HashMap* hashmap);

#endif