#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"


typedef struct HashMap HashMap;
int enlarge_called=0;

struct HashMap {
    Pair ** buckets;
    long size; //cantidad de datos/pairs en la tabla
    long capacity; //capacidad de la tabla
    long current; //indice del ultimo dato accedido
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash = 0;
     char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash += hash*32 + tolower(*ptr);
    }
    return hash%capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


void insertMap(HashMap * map, char * key, void * value) {
    if (map == NULL || key == NULL) return;
    long idx = hash(key, map->capacity);
    long inicio = idx;
    long firstFree = -1;

    while(1){
        Pair *slot = map->buckets[idx];
        if(slot == NULL) {
            long destino = (firstFree != -1) ? firstFree : idx;
            if(map->buckets[destino] == NULL){
                Pair *p = createPair(key, value);
                if(!p) return;
                map->buckets[destino] = p;
            } else {
                map ->buckets[destino]->key = key;
                map->buckets[destino]->value = value;
            }
            map->size++;
            map->current = destino;
            if(map->size * 10 > map->capacity * 7) enlarge(map);
            return;
        }

        if(slot->key == NULL){
            if(firstFree == -1) firstFree = idx;
        } else if (is_equal(slot->key, key)){
            map->current = idx;
            return;
        }
        idx = (idx + 1) & map->capacity;
        if (idx == inicio){
            if (firstFree  != 1){
                long destino = firstFree;
                if (map->buckets[destino] == NULL){
                    Pair *p = createPair(key, value);
                    if(!p) return;
                    map->buckets[destino] = p;
                } else {
                    map ->buckets[destino]->key = key;
                    map->buckets[destino]->value = value;
                }
                map -> size++;
                map-> current = destino;
                if(map->size * 10 > map->capacity * 7) enlarge(map);
            }
            return;
        }
    }
}

void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)
    Pair **oldBuckets = map -> buckets;
    long oldCapacity = map -> capacity;
    long newCapacity = (oldCapacity > 0) ? oldCapacity * 2 : 1;
    Pair **newBuckets = (Pair**) calloc(newCapacity, sizeof(Pair*));
    if(!newBuckets) return;

    map->buckets =  newBuckets;
    map->capacity = newCapacity;
    map->size = 0;
    map->current = -1;

    for(long k = 0; k < oldCapacity; k++){
        Pair *p = oldBuckets[k];
        if (p != NULL && p->key != NULL){
            insertMap(map, p->key, p->value);
            free(p);
        }
    }
    free(oldBuckets);
}


HashMap * createMap(long capacity) {
    if (capacity < 1) capacity = 1;
    HashMap *map = (HashMap*)calloc(1, sizeof(HashMap));
    if(!map) return NULL;
    map->buckets = (Pair**)calloc(capacity, sizeof(Pair*));
    if(!map -> buckets){
        free(map);
        return NULL;
    }

    map->capacity = capacity;
    map->size = 0;
    map->current = -1;
    return map;
}

void eraseMap(HashMap * map,  char * key) {    
    if (map == NULL || key == NULL) return;

    Pair *slot = searchMap(map, key);
    if (slot == NULL) return;
    slot->key = NULL;
    slot->value = NULL;
    map->size--;
}


Pair * searchMap(HashMap * map,  char * key) {   
    if (map == NULL || key == NULL) return NULL;

    long idx = hash(key, map->capacity);
    long inicio = idx;

    while (1) {
        Pair *slot = map->buckets[idx];

        if (slot == NULL) {
            map->current = -1;
            return NULL;
        }

        if (slot->key != NULL && is_equal(slot->key, key)) {
            map->current = idx;
            return slot;
        }

        idx = (idx + 1) % map->capacity;
        if (idx == inicio) {
            map->current = -1;
            return NULL;
        }
    }


    return NULL;
}

Pair * firstMap(HashMap * map) {
    if (map == NULL || map->buckets == NULL || map->capacity <= 0) return NULL;

    for (long k = 0; k < map->capacity; k++) {
        Pair *p = map->buckets[k];
        if (p != NULL && p->key != NULL) {
            map->current = k;
            return p;
        }
    }
    map->current = -1;
    return NULL;
}


Pair * nextMap(HashMap * map) {
    if (map = NULL || map ->buckets == NULL || map->capacity <= 0) return NULL;
    long start = (map ->current < 0) ? 0 : map -> current + 1;

    for (long k = start; k < map->capacity; k++){
        Pair *p = map->buckets[k];
        if (p != NULL && p -> key != NULL){
            map -> current = k;
            return p;
        }
    }
    map->current = -1;
    return NULL;
}
