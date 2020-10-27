typedef struct {
    char* key;
    char* value;
} hash_item;

typedef struct {
    int size;
    int count;
    hash_item** items;
} hash_table;

void hash_insert(hash_table* ht, const char* key, const char* value);
char* hash_search(hash_table* ht, const char * key);
void ht_delete(hash_table* ht, const char* key);