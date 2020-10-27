#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

static hash_item HT_DELETED_ITEM = {NULL, NULL};
static const int HASH_INTIAL_BASE_SIZE = 53;

//static pointer to a new hashitem
static hash_item* hash_item_new(const char* key, const char* value){
    hash_item * i = malloc(sizeof(hash_item));
    i->key = strdup(key);
    i->value = strdup(value);
    return i;
}

//creates a new hashtable
hash_table* new_table(){
    hash_table* ht = malloc(sizeof(hash_table));

    ht->size = 53;
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(hash_item*));
    return ht;
}

//reads in the pointer to a hashtable and deletes all items in it and the table itself
static void del_hashtable(hash_table* ht){
    for(int i = 0; i< ht->size; i++){
        hash_item* item = ht->items[i];
        if (item != NULL){
            del_hashtable(item);
        }
    }
    free(ht->items);
    free(ht);
}

//basic hash function that gets the hashkey loc of the item
static int hash(const char* s, const int a, const int m){
    long hash = 0;
    const int len_s = strlen(s);
    for(int i= 0; i<len_s; i++){
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash%m;
    }
    return (int)hash;
}

//further strain the hash to prevent collisions
static int hash_getHash(const char * s, const int num_buckets, const int  attempt){
    const int hash_a = hash(s, 111, num_buckets);
    const int hash_b = hash(s, 91, num_buckets);
    return (hash_a + (attempt * (hash_b+1))) % num_buckets;
}

void hash_insert(hash_table* ht, const char* key, const char* value){
    const int load = ht->count * 100/ ht->size;
    if (load > 70){
        hash_resizeUP(ht);
    }

    hash_item* item = hash_item_new(key, value);
    int index = hash_getHash(item->key, ht->size, 0);
    hash_item* curr_item = ht->items[index];
    int i = 1;
    while(curr_item != NULL){
        if (curr_item != &HT_DELETED_ITEM) {
            if (strcmp(curr_item->key, key) == 0) {
                ht_del_item(curr_item);
                ht->items[index] = item;
                return;
            }
        }
        index = hash_getHash(item->key, ht->size, i);
        curr_item = ht->items[index];
        i++;
    }
    ht->items[index] = item;
    ht->count++;
}


char* hash_search(hash_table* ht, const char * key){
    int index = hash_getHash(key, ht->size, 0);
    hash_item * item = ht->items[index];
    int i = 1;
    while(item != NULL){
        if (strcmp(item->key, key)== 0){
            return item->value;
        }
        index = hash_getHash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    return NULL;
}



void ht_delete(hash_table* ht, const char* key){
    const int load = ht->count * 100/ ht->size;
    if (load < 10){
        hash_resizeDOWN(ht);
    }
    int index = ht_get_hash(key, ht->size, 0);
    hash_item* item = ht->items[index];
    int i = 1;
    while (item != NULL) {
        if (item != &HT_DELETED_ITEM) {
            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    } 
    ht->count--;
}

static hash_table* hash_new_size(const int base_size){
    hash_table* ht = xmalloc(sizeof(hash_table));
    ht->size = base_size;

    ht-> size = next_prime(ht->size);

    ht-> count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(hash_item));
    return ht;
}

hash_table* new_hashTable(){
    return hash_new_size(53);
}

static void hash_resize(hash_table* ht, const int base_size){
    if(base_size < 53){
        return;
    }
    hash_table* new_ht = hash_new_size(base_size);
    for(int i = 0; i< ht->size; i++){
        hash_item* item = ht->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM){
            hash_insert(new_ht, item->key, item->value);
        }
    }
    ht->size = new_ht->size;
    ht->count = new_ht->count;

    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    hash_item** temp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = temp_items;

    del_hashtable(new_ht);
}

static void hash_resizeUP(hash_table * ht){
    const int new_size = ht->size *2;
    hash_resize(ht, new_size);
}

static void hash_resizeDOWN(hash_table* ht){
    const int new_size = ht->size/2;
    hash_resize(ht, new_size);
}
