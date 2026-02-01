#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

// Node structure
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Gap Buffer Index structure
typedef struct {
    Node** index;      // Array of Node pointers
    int gapPos;        // Start position of gap
    int gapSize;       // Fixed gap size (e.g., 10)
    int logicalSize;   // Number of actual elements
    int capacity;      // Total array capacity
} GapIndex;

// Linked List structure
typedef struct {
    Node* head;
    GapIndex gapIndex;
    int size;          // Same as gapIndex.logicalSize
} LinkedList;

// Function prototypes
void initGapIndex(GapIndex* gi, int capacity);
Node* gapGet(GapIndex* gi, int i);
void gapInsert(GapIndex* gi, int i, Node* node);
void gapDelete(GapIndex* gi, int i);
void resizeGapIndex(GapIndex* gi, int newCapacity);
void printGapState(GapIndex* gi);

// Linked List functions
void initLinkedList(LinkedList* list, int capacity);
Node* get(LinkedList* list, int i);
void insert(LinkedList* list, int i, int value);
void delete(LinkedList* list, int i);
void printList(LinkedList* list);
void freeLinkedList(LinkedList* list);

#endif // LINKED_LIST_H