#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>

// Initialize gap index
void initGapIndex(GapIndex* gi, int capacity) {
    gi->capacity = capacity;
    gi->gapSize = 10;  // Fixed gap size
    gi->gapPos = 0;
    gi->logicalSize = 0;
    
    // Allocate array of Node pointers
    gi->index = (Node**)malloc(capacity * sizeof(Node*));
    
    // Initialize all to NULL
    for(int i = 0; i < capacity; i++) {
        gi->index[i] = NULL;
    }
}

// Get node at logical index i (O(1))
Node* gapGet(GapIndex* gi, int i) {
    if(i < 0 || i >= gi->logicalSize) {
        return NULL;  // Index out of bounds
    }
    
    if(i < gi->gapPos) {
        // Position before gap - direct access
        return gi->index[i];
    } else {
        // Position after gap - skip the gap
        return gi->index[i + gi->gapSize];
    }
}

// Insert node at logical position i 
void gapInsert(GapIndex* gi, int i, Node* node) {
    if(i < 0 || i > gi->logicalSize) {
        printf("Invalid insert position\n");
        return;
    }
    
    // changes made here - resizing done
    if (gi->logicalSize + gi->gapSize >= gi->capacity) {
        int newCapacity = gi->capacity * 2;  // capacity doubled
        printf("Resizing from %d to %d capacity\n", gi->capacity, newCapacity);
        resizeGapIndex(gi, newCapacity);
    }
    
    // Move gap to position i if needed
    if(i != gi->gapPos) {
        int direction = (i < gi->gapPos) ? -1 : 1;
        int start, end;
        
        if(direction == -1) {
            // Moving gap left: shift elements right into gap
            start = i;
            end = gi->gapPos - 1;
            for(int pos = end; pos >= start; pos--) {
                int src = pos;
                int dst = pos + gi->gapSize;
                gi->index[dst] = gi->index[src];
                gi->index[src] = NULL;
            }
        } else {
            // Moving gap right: shift elements left into gap
            start = gi->gapPos;
            end = i - 1;
            for(int pos = start; pos <= end; pos++) {
                int src = pos + gi->gapSize;
                int dst = pos;
                gi->index[dst] = gi->index[src];
                gi->index[src] = NULL;
            }
        }
        gi->gapPos = i;
    }
    
    // Insert the new node at gap position
    gi->index[gi->gapPos] = node;
    gi->gapPos++;  // Move gap start right
    gi->logicalSize++;
}

// Delete node at logical position i
void gapDelete(GapIndex* gi, int i) {
    if (i < 0 || i >= gi->logicalSize) {
        printf("Invalid delete position\n");
        return;
    }

    // changes made here - move elements to position i for direct deletion
    if (i != gi->gapPos) {
        int direction = (i < gi->gapPos) ? -1 : 1;
        int start, end;

        if (direction == -1) {
            // moving gap left: shift elements right into gap
            start = i;
            end = gi->gapPos - 1;
            for (int pos = end; pos >= start; pos--) {
                int src = pos;
                int dst = pos + gi->gapSize;
                gi->index[dst] = gi->index[src];
                gi->index[src] = NULL;
            }
        }
        else {
            // moving gap right: shift elements left into gap
            start = gi->gapPos;
            end = i - 1;
            for (int pos = start; pos <= end; pos++) {
                int src = pos + gi->gapSize;
                int dst = pos;
                gi->index[dst] = gi->index[src];
                gi->index[src] = NULL;
            }
        }
        gi->gapPos = i;
    }

    gi->index[gi->gapPos] = NULL;
    
    gi->logicalSize--;
}

// new functions 
// resize function for gap buffer
void resizeGapIndex(GapIndex* gi, int newCapacity) {
    Node** newIndex = (Node**)malloc(newCapacity * sizeof(Node*));
    if (!newIndex) {
        printf("Memory Allocation failed during resize\n");
        return;
    }
    // initialise new array to NULL
    for (int i = 0; i < newCapacity; i++) {
        newIndex[i] = NULL;
    }
    // copy elements before gap
    for (int i = 0; i < gi->gapPos; i++) {
        newIndex[i] = gi->index[i];
    }
    // copy elements after gap - new gap position
    int oldGapEnd = gi->gapPos + gi->gapSize;
    int newGapEnd = gi->gapPos + gi->gapSize + (newCapacity - gi->capacity);

    for (int i = oldGapEnd; i < gi->capacity; i++) {
        int newPos = i + (newCapacity - gi->capacity);
        if (newPos < newCapacity) {
            newIndex[newPos] = gi->index[i];
        }
    }
    // free old array 
    free(gi->index);
    gi->index = newIndex;
    gi->capacity = newCapacity;
}

// free entire linked list function
void freeLinkedList(LinkedList* list) {
    // free nodes in linked list
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    // free index array
    free(list->gapIndex.index);

    // reset
    list->head = NULL;
    list->size = 0;
    list->gapIndex.logicalSize = 0;
    list->gapIndex.gapPos = 0;
}

// debug function - see gap buffer state
void printGapState(GapIndex* gi) {
    printf("Gap State: pos=%d, size=%d, logical=%d, capacity=%d\n",
        gi->gapPos, gi->gapSize, gi->logicalSize, gi->capacity);
}
// end of new functions added

void initLinkedList(LinkedList* list, int capacity) {
    list->head = NULL;
    list->size = 0;
    initGapIndex(&list->gapIndex, capacity);
}

Node* get(LinkedList* list, int i) {
    if(i < 0 || i >= list->size) {
        return NULL;
    }
    return gapGet(&list->gapIndex, i);
}

// O(1) insertion (amortized)
void insert(LinkedList* list, int i, int value) {
    if(i < 0 || i > list->size) {
        printf("Invalid position\n");
        return;
    }
    
    // Create new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    if(!newNode) {
        printf("Memory allocation failed\n");
        return;
    }
    newNode->data = value;
    newNode->next = NULL;
    
    // Insert into linked list
    if(i == 0) {
        newNode->next = list->head;
        list->head = newNode;
    } else {
        // Get previous node using O(1) gap index
        Node* prev = gapGet(&list->gapIndex, i-1);
        if(prev) {
            newNode->next = prev->next;
            prev->next = newNode;
        }
    }
    
    // Update gap index
    gapInsert(&list->gapIndex, i, newNode);
    list->size++;
    printf("Inserted %d at position %d\n", value, i);
}

// O(1) deletion (amortized)
void delete(LinkedList* list, int i) {
    if(i < 0 || i >= list->size) {
        printf("Invalid position\n");
        return;
    }
    
    // Remove from linked list
    Node* toDelete;
    if(i == 0) {
        toDelete = list->head;
        list->head = list->head->next;
    } else {
        Node* prev = gapGet(&list->gapIndex, i-1);
        toDelete = prev->next;
        prev->next = toDelete->next;
    }
    
    printf("Deleting node at position %d with value %d\n", i, toDelete->data);
    
    // Update gap index
    gapDelete(&list->gapIndex, i);
    
    // Free memory
    free(toDelete);
    list->size--;
}

void printList(LinkedList* list) {
    printf("Linked List (%d elements): ", list->size);
    Node* current = list->head;
    while(current != NULL) {
        printf("%d", current->data);
        if(current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf("\n");
}