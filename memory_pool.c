#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "performance_metrics.h"

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct MemoryPool {
    Node *nodes;
    int capacity;
    Node *free_head;
} MemoryPool;

MemoryPool* pool_create(int capacity) {
    MemoryPool* p = (MemoryPool*)malloc(sizeof(MemoryPool));
    p->nodes = (Node*)malloc(sizeof(Node) * capacity);
    p->capacity = capacity;
    p->free_head = NULL;
    // initialize free list
    for (int i = 0; i < capacity; ++i) {
        p->nodes[i].next = p->free_head;
        p->free_head = &p->nodes[i];
    }
    return p;
}

Node* pool_alloc(MemoryPool *p) {
    if (!p->free_head) return NULL;
    Node *n = p->free_head;
    p->free_head = p->free_head->next;
    n->next = NULL;
    return n;
}

void pool_free(MemoryPool *p, Node *n) {
    n->next = p->free_head;
    p->free_head = n;
}

void pool_destroy(MemoryPool *p) {
    free(p->nodes);
    free(p);
}

void insert_tail_pool(Node **head, MemoryPool *p, int value) {
    Node *n = pool_alloc(p);
    if (!n) { fprintf(stderr,"Pool exhausted\n"); exit(EXIT_FAILURE); }
    n->data = value;
    n->next = NULL;
    if (!*head) { *head = n; return; }
    Node *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = n;
}

void traverse(Node *head) { for (Node *c=head;c;c=c->next) { volatile int x=c->data; (void)x; } }

int main(int argc, char **argv) {
    int N = (argc>1)?atoi(argv[1]):100000;
    MemoryPool *pool = pool_create(N + 10);
    Node *head = NULL;

    struct rusage rstart, rend;
    double t0, t1;
    // insertion from pool
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    for (int i = 0; i < N; ++i) insert_tail_pool(&head, pool, i);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);
    double insert_ms = t1 - t0;

    // traversal
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    traverse(head);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);
    double traverse_ms = t1 - t0;

    // return nodes to pool (walk and free)
    Node *cur = head;
    while (cur) {
        Node *n = cur;
        cur = cur->next;
        pool_free(pool, n);
    }

    pool_destroy(pool);

    printf("%d,%.3f,%.3f,%.3f,%ld,%ld,%ld\n",
           N, insert_ms, traverse_ms, 0.0,
           ru_maxrss_diff_kb(&rstart,&rend),
           ru_majflt_diff(&rstart,&rend),
           ru_minflt_diff(&rstart,&rend));
    return 0;
}