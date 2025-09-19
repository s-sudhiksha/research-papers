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

void cache_optimized_traverse(Node *head) {
    Node *cur = head;
    Node *prefetch = cur ? cur->next : NULL;
    while (cur) {
        if (prefetch) {
            __builtin_prefetch(prefetch->next, 0, 1);
            prefetch = prefetch->next;
        }
        volatile int x = cur->data; (void)x;
        cur = cur->next;
    }
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

    // Build list
    for (int i = 0; i < N; ++i) insert_tail(&head, i);

    // Prefetch traversal measurement
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    cache_optimized_traverse(head);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);
    double traverse_ms = t1 - t0;

    free_list(head);

    // Output: size,insert_ms (0 for this program),traverse_ms,search_ms (0),mem & faults (0)
    printf("%d,%.3f,%.3f,%.3f,%ld,%ld,%ld\n",
           N, 0.0, traverse_ms, 0.0,
           ru_maxrss_diff_kb(&rstart,&rend),
           ru_majflt_diff(&rstart,&rend),
           ru_minflt_diff(&rstart,&rend));

    return 0;
}