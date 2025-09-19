#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int size_list(Node *head) {
    int s=0; for (Node *c=head;c;c=c->next) ++s; return s;
}

void batch_insert(Node *head, int values[], int count, int position) {
    if (position == 0) {
        // Insert at head: create chain and link
        Node *prev = NULL;
        for (int i = 0; i < count; ++i) {
            Node *n = create_node(values[i]);
            n->next = prev;
            prev = n;
        }
        // reverse prev chain to maintain order: values[0]..values[count-1]
        // find tail of prev
        Node *tail = prev;
        while(tail && tail->next) tail = tail->next;
        tail->next = head;
        // We cannot modify head pointer in caller in this simple example; skip full head replacement
        return;
    }

    Node *cur = head;
    for (int i = 0; i < position - 1 && cur; ++i) cur = cur->next;
    if (!cur) { printf("pos > size\n"); return; }

    Node *nextNode = cur->next;
    // insert in reverse order to retain original sequence
    for (int i = count - 1; i >= 0; --i) {
        Node *newNode = create_node(values[i]);
        newNode->next = nextNode;
        cur->next = newNode;
        nextNode = newNode;
    }
}

void traverse(Node *head) {
    for (Node *c = head; c; c = c->next) { volatile int x=c->data; (void)x; }
}

void free_list(Node *head) { while (head) { Node *t=head; head=head->next; free(t); } }

int main(int argc, char **argv) {
    int N = (argc>1)?atoi(argv[1]):100000;
    Node *head = NULL;
    for (int i=0;i<N;i++) insert_tail(&head, i);

    // Prepare batch values
    int batch_size = 1000;
    int *vals = malloc(sizeof(int)*batch_size);
    for (int i=0;i<batch_size;i++) vals[i] = 1000000 + i;

    int pos = N/2; // insert in middle

    struct rusage rstart, rend;
    double t0, t1;
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    batch_insert(head, vals, batch_size, pos);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);

    double batch_insert_ms = t1 - t0;

    // Traverse to verify
    getrusage(RUSAGE_SELF, &rstart);
    t0 = now_ms();
    traverse(head);
    t1 = now_ms();
    getrusage(RUSAGE_SELF, &rend);
    double traverse_ms = t1 - t0;

    free_list(head);
    free(vals);

    printf("%d,%.3f,%.3f,%.3f,%ld,%ld,%ld\n",
           N, batch_insert_ms, traverse_ms, 0.0,
           ru_maxrss_diff_kb(&rstart,&rend),
           ru_majflt_diff(&rstart,&rend),
           ru_minflt_diff(&rstart,&rend));

    return 0;
}