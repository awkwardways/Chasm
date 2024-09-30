#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FNV_OFFSET_BASIS  0xcbf29ce484222325
#define FNV_PRIME         0x00000100000001b3

// Make resizing mechanism
// Safety checks
// // Memory management
// // Collision management (fix collision insertion)

void nullifyLinkedList(node* map, size_t mapSize, size_t beginning) {
  for(size_t idx = beginning; idx < mapSize; idx++) map[idx].next = NULL;
}

uint64_t FNV1A(const char* str) {
  uint64_t hash = FNV_OFFSET_BASIS;
  while(*str != '\0') {
    hash ^= *(str++);
    hash *= FNV_PRIME;
  }
  return hash;
}

node* resizeMap(node* hashmap, size_t hashmapSize) {
  size_t newSize = 2 * hashmapSize;
  hashmap = (node*) realloc(hashmap, newSize * sizeof(node));
  if(hashmap == NULL) {
    // printf("Could not resize map\n");
    return NULL;
  }
  // // printf("Hashmap resized\n");
  // nullify new cells
  nullifyLinkedList(hashmap, newSize, hashmapSize);
  // rehash elements
  do {
    --newSize;
    if(hashmap[newSize].next != NULL) {
      // printf("Found something at %d with key %s\n\tHashing to new location %d\n", newSize, hashmap[newSize].key, FNV1A(hashmap[newSize].key) % (2 * hashmapSize));
      hashmap[FNV1A(hashmap[newSize].key) % (2 * hashmapSize)] = hashmap[newSize];
      if(FNV1A(hashmap[newSize].key) % (2 * hashmapSize) != newSize) {
        hashmap[newSize].next = NULL;
      }
    }
  } while(newSize);
  // else return hashmap;
  return hashmap;
}

node* deleteList(node* currentNode) {
  if(currentNode->next == NULL) {
    // // printf("deleting empty node\n", currentNode->key);
    free(currentNode);
    return NULL;
  }
  currentNode->next = deleteList(currentNode->next);
  // // printf("deleting %s\n", currentNode->key);
  free(currentNode);
  return NULL;
}
void deleteElement(const char* key, HashMap* hashmap) {
  size_t keyIdx = FNV1A(key) % hashmap->mapSize;
  node* aux = NULL;
  node* temp = NULL;
  if(key == hashmap->map[keyIdx].key) {
    //Head is key. Delete head.
    // printf("\tHEAD IS KEY\n");
    aux = &hashmap->map[keyIdx];
    temp = aux->next;
    do
    {
      // printf("Switch Begin\n");
      *(aux) = *(temp);
      aux = temp;
      temp = temp->next;
      // printf("Switch End\n");
    } while (temp->next != NULL);
    *(aux) = *(temp);
    free(temp);
    // printf("\tDONE\n");
  }
  else {
    //Look for key in linked list
    // printf("\tHEAD IS NOT KEY\n");
    aux = &hashmap->map[keyIdx];
    while(aux->next != NULL) {
      if(aux->next->key == key) {
        break;
      }
      aux = aux->next; 
    }
    if(aux->next == NULL) {
      return;
    }
    temp = aux->next->next;
    free(aux->next);
    aux->next = temp;
  } 
}
void findLists(size_t mapSize, node* _map) {
  size_t idx = 0;
  while(idx < mapSize) {
    if(_map[idx].next != NULL) {
      // // printf("Found list at %d with head %s\n", idx, _map[idx].key);
      deleteList(_map[idx].next);
    }
    idx++;
  }
}

//Dynamically allocates a hashmap of size initialMapSize and initializes all node pointers to the next node as NULL
HashMap* initHashMap(size_t initialMapSize) {
  HashMap* newmap = (HashMap*) malloc(sizeof(HashMap));
  newmap->mapSize = initialMapSize;
  newmap->map = (node*) malloc(initialMapSize * sizeof(node));
  nullifyLinkedList(newmap->map, initialMapSize, 0);
  newmap->count = 0;
  return newmap;
}

//Deletes all dynamically allocated memory
HashMap* deleteHashMap(HashMap* hashmap) {
  findLists(hashmap->mapSize, hashmap->map);
  free(hashmap->map);
  hashmap->map = NULL;
  free(hashmap);
  hashmap = NULL;
  return hashmap;
}

//Insert an integer into a hashmap using a string as a key
uint64_t insertElement(HashMap* hashmap, const char* _key, int value) {
  if(hashmap->mapSize == hashmap->count) {
    // printf("map size exceeded\n");
    hashmap->map = resizeMap(hashmap->map, hashmap->mapSize);
    if(hashmap->map == NULL) {
      return -1;
    }
    // printf("map resized\n");
    hashmap->mapSize *= 2; 
  }
  //hash the string
  uint64_t idx = FNV1A(_key) % hashmap->mapSize;
  // int i = 1;
  node* it = &hashmap->map[idx];

  while(it->next != NULL) {
    // // printf("\tdepth of:%d\n", i);
    it = it->next;
    // i++;
  }
  
  //insert element and create next node
  strcpy(it->key, _key);
  // it->key = _key;
  it->val = value;
  it->next = (node*) malloc(sizeof(node));
  it->next->next = NULL;  //Nullify new node's next pointer to signal an unpopulated node (new node has not been occupied)
  hashmap->count++;
  return idx;
}

//Looks for key in hashmap. Returns NULL if key doesn't exist
node* findElement(HashMap* hashmap, const char* key) {
  size_t idx = FNV1A(key) % hashmap->mapSize;
  node* it = &hashmap->map[idx];
  // // printf("%s\n", it->key);
  while(it != NULL && it->next != NULL) {
    // printf("\tHASHTABLE:comparing %s to %s\n", key, it->key);
    if(!strcmp(key, it->key)) {
      return it;
    } 
    it = it->next;
  }
  it = NULL;
  return it;
}

