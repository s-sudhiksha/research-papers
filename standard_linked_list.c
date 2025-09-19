#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "performance_metrics.h"

typedef struct Node {
    int data;
    struct Node *next;
} Node;

Node* create_node(int v) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->data = v;
    n->next = NULL;
    return n;
}

void insert_tail(Node **head, int v) {
    if (*head == NULL) { *head = create_node(v); return; }
    Node *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = create_node(v);
}

void traverse(Node *head) {
    for (Node *cur = head; cur; cur = cur->next) {
        volatile int x = cur->data; (void)x; // simulate work
    }
}

Node* search(Node *head, int key) {
    for (Node *cur = head; cur; cur = cur->next)
        if (cur->data == key) return cur;
    return NULL;
}

void free_list(Node *head) {
    while (head) {
        Node *t = head;
        head = head->next;
        free(t);
    }
}

int main(int argc, char **argv) {
    int N = (argc > 1) ? atoi(argv[1]) : 100000;
    Node *head = NULL;
    struct rusage rstart, rend;
    double t0, t1;
    // Insertion
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    for (int i = 0; i < N; ++i) insert_tail(&head, i);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);
    double insert_ms = t1 - t0;
    long mem_alloc_kb = ru_maxrss_diff_kb(&rstart, &rend);

    // Traversal
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    traverse(head);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);
    double traverse_ms = t1 - t0;

    // Search last element (worst-case)
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    Node *f = search(head, N - 1);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);
    double search_ms = t1 - t0;

    free_list(head);

    // CSV output: size,insert_ms,traverse_ms,search_ms,mem_kb:majflt:minflt
    printf("%.0d,%.3f,%.3f,%.3f,%ld,%ld,%ld\n",
           N, insert_ms, traverse_ms, search_ms,
           mem_alloc_kb,
           ru_majflt_diff(&rstart, &rend),
           ru_minflt_diff(&rstart, &rend));

    return 0;
}