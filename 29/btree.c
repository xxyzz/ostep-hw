#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
// max children per B-tree node = M-1
// (must be even and greater than 2)
#define M 4
#define ONE_MILLION 1000000

// internal nodes: only use key and next
// external nodes: only use key and value
typedef struct __entry_t {
    char *key;
    char *val;
    struct __node_t *next;    // helper field to iterate over array entries
} entry_t;

// helper B-tree node data type
typedef struct __node_t {
    int m;                   // number of children
    entry_t *children[M];    // the array of children
} node_t;

typedef struct __btree_t {
    node_t *root;     // root of the B-tree
    int    height;    // height of the B-tree
    int    n;         // number of key-value pairs in the B-tree  
    pthread_mutex_t lock;
} btree_t;

typedef struct __myarg_t {
    btree_t *btree;
    int threads;
} myarg_t;

entry_t *initEntry(char *key, char *val, node_t *next) {
    entry_t *new = malloc(sizeof(entry_t));
    new->key = key;
    new->val = val;
    new->next = next;
    return new;
}

node_t *initNode(int m) {
    node_t *new = malloc(sizeof(node_t));
    new->m = m;
    for(size_t i = 0; i < M; i++) {
        new->children[i] = initEntry("", "", NULL);
    } 
    return new;
}

btree_t *initBtree() {
    btree_t *new = malloc(sizeof(btree_t));
    new->height = 0;
    new->n = 0;
    new->root = initNode(0);
    pthread_mutex_init(&new->lock, NULL);
    return new;
}

char * search(node_t *x, char *key, int ht);
node_t *insert(node_t *h, char *key, char *val, int ht);
node_t *split(node_t *h);
char *toStringHelper(node_t *h, int ht, char *indent);

// Returns the value associated with the given key.
char * get(btree_t *btree, char *key) {
    if (key == NULL)
        return NULL;

    return search(btree->root, key, btree->height);
}

char * search(node_t *x, char *key, int ht) {
    entry_t *children[M];
    for(size_t i = 0; i < M; i++) {
        children[i] = x->children[i];
    }

    // external node
    if (ht == 0) {
        for (int j = 0; j < x->m; j++) {
            if (key == children[j]->key)
                return children[j]->val;
        }
    }

    // internal node
    else {
        for (int j = 0; j < x->m; j++) {
            if (j+1 == x->m || strcmp(key, children[j+1]->key) < 0)
                return search(children[j]->next, key, ht-1);
        }
    }
    return NULL;
}

/**
 * Inserts the key-value pair into the symbol table, overwriting the old value
 * with the new value if the key is already in the symbol table.
 * If the value is null, this effectively deletes the key from the symbol table.
 */
void put(btree_t *btree, char *key, char *val) {
    if (key == NULL)
        return;
    
    node_t *u = insert(btree->root, key, val, btree->height);
    btree->n++;
    if (u == NULL)
        return;
    
    // need to split root
    node_t *t = initNode(2);
    t->children[0] = initEntry(btree->root->children[0]->key, NULL, btree->root);
    t->children[1] = initEntry(u->children[0]->key, NULL, u);
    btree->root = t;
    btree->height++;
}

node_t *insert(node_t *h, char *key, char *val, int ht) {
    int j;
    entry_t *t = initEntry(key, val, NULL);

    // external node
    if (ht == 0) {
        for (j = 0; j < h->m; j++) {
            if (strcmp(key, h->children[j]->key) < 0)
                break;
        }
    }

    // internal node
    else {
        for (j = 0; j < h->m; j++) {
            if ((j+1 == h->m) || strcmp(key, h->children[j+1]->key) < 0) {
                node_t *u = insert(h->children[j++]->next, key, val, ht-1);
                if (u == NULL)
                    return NULL;
                t->key = u->children[0]->key;
                t->next = u;
                break;
            }
        }
    }

    for (int i = h->m; i > j; i--)
        h->children[i] = h->children[i-1];
    h->children[j] = t;
    h->m++;
    if (h->m < M)
        return NULL;
    else return split(h);
}

// split node in half
node_t *split(node_t *h) {
    node_t *t = initNode(M/2);
    h->m = M/2;
    for (int j = 0; j < M/2; j++)
        t->children[j] = h->children[M/2+j]; 
    return t; 
}

// Returns a string representation of this B-tree (for debugging).
char *toString(btree_t *btree) {
    return toStringHelper(btree->root, btree->height, "");
}

char *toStringHelper(node_t *h, int ht, char *indent) {
    char *s = malloc(1024 * sizeof(char));
    entry_t *children[M];
    for(size_t i = 0; i < M; i++) {
        children[i] = h->children[i];
    }

    if (ht == 0) {
        for (int j = 0; j < h->m; j++) {
            strcat(s, indent);
            strcat(s, children[j]->key);
            strcat(s, " ");
            if (children[j]->val == NULL) {
                strcat(s, "null");
            } else {
                strcat(s, children[j]->val);
            }
            strcat(s, "\n");
        }
    }
    else {
        for (int j = 0; j < h->m; j++) {
            if (j > 0) {
                strcat(s, indent);
                strcat(s, "(");
                strcat(s, children[j]->key);
                strcat(s, ")\n");
            }
            char *intentCp = malloc(1024 * sizeof(char));
            strcpy(intentCp, indent);
            strcat(intentCp, "     ");
            strcat(s, toStringHelper(children[j]->next, ht-1, intentCp));
        }
    }
    return s;
}

void *thread_function(void *args) {
    myarg_t *m = (myarg_t *) args;
    pthread_mutex_lock(&m->btree->lock);
    for(size_t i = 0; i < 100 / m->threads; i++) {
        put(m->btree, "www.cs.princeton.edu", "128.112.136.12");
    }
    pthread_mutex_unlock(&m->btree->lock);
    pthread_exit(0);
}

// Unit tests the BTree data type.
int main(int argc, char *argv[]) {
    for(int i = 1; i < 11; i++) {
        btree_t *btree = initBtree();
        myarg_t args;
        args.btree = btree;
        args.threads = i;
        pthread_t threads[i];
        struct timeval start, end;
        gettimeofday(&start, NULL);
        for(int j = 0; j < i; j++) {
            pthread_create(&threads[j], NULL, &thread_function, &args);
        }
        for(int k = 0; k < i; k++) {
            pthread_join(threads[k], NULL);
        }
        gettimeofday(&end, NULL);
        printf("%d threads, time (seconds): %f\n", i,
            (float) (end.tv_usec - start.tv_usec + (end.tv_sec - start.tv_sec) * ONE_MILLION) / ONE_MILLION);
        printf("size: %d\n\n", btree->n);
    }

    // put(btree, "www.cs.princeton.edu", "128.112.136.12");
    // put(btree, "www.cs.princeton.edu", "128.112.136.11");
    // put(btree, "www.cs.princeton.edu", NULL);
    // put(btree, "www.princeton.edu",    "128.112.128.15");
    // put(btree, "www.yale.edu",         "130.132.143.21");
    // put(btree, "www.simpsons.com",     "209.052.165.60");
    // put(btree, "www.apple.com",        "17.112.152.32");
    // put(btree, "www.amazon.com",       "207.171.182.16");
    // put(btree, "www.ebay.com",         "66.135.192.87");
    // put(btree, "www.cnn.com",          "64.236.16.20");
    // put(btree, "www.google.com",       "216.239.41.99");
    // put(btree, "www.nytimes.com",      "199.239.136.200");
    // put(btree, "www.microsoft.com",    "207.126.99.140");
    // put(btree, "www.dell.com",         "143.166.224.230");
    // put(btree, "www.slashdot.org",     "66.35.250.151");
    // put(btree, "www.espn.com",         "199.181.135.201");
    // put(btree, "www.weather.com",      "63.111.66.11");
    // put(btree, "www.yahoo.com",        "216.109.118.65");

    // printf("cs.princeton.edu:  %s\n", get(btree, "www.cs.princeton.edu"));
    // printf("hardvardsucks.com: %s\n", get(btree, "www.harvardsucks.com"));    // lol
    // printf("simpsons.com:      %s\n", get(btree, "www.simpsons.com"));
    // printf("apple.com:         %s\n", get(btree, "www.apple.com"));
    // printf("ebay.com:          %s\n", get(btree, "www.ebay.com"));
    // printf("dell.com:          %s\n", get(btree, "www.dell.com"));

    // printf("size:      %d\n", btree->n);
    // printf("height:    %d\n", btree->height);
    // printf("%s\n", toString(btree));
    return 0;
}
