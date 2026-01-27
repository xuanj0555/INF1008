#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node** index;      // Array of Node pointers
    int gapPos;        // Start position of gapå
    int gapSize;       // Fixed gap size (e.g., 10)
    int logicalSize;   // Number of actual elements
    int capacity;      // Total array capacity
} GapIndex;

typedef struct {
    Node* head;
    GapIndex gapIndex;
    int size;          // Same as gapIndex.logicalSize
} LinkedList;

void initGapIndex(GapIndex* gi, int capacity);
Node* gapGet(GapIndex* gi, int i);
void gapInsert(GapIndex* gi, int i, Node* node);
void gapDelete(GapIndex* gi, int i);
void initLinkedList(LinkedList* list, int capacity);
Node* get(LinkedList* list, int i);
void insert(LinkedList* list, int i, int value);
void delete(LinkedList* list, int i);
void printList(LinkedList* list);

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
    
    // Check if need to resize
    if(gi->logicalSize + gi->gapSize >= gi->capacity) {
        printf("Resizing needed (not implemented in this example)\n");
        return;
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
    if(i < 0 || i >= gi->logicalSize) {
        printf("Invalid delete position\n");
        return;
    }
    
    // Move gap to position i+1 (just after the element to delete)
    gapInsert(gi, i+1, NULL);  // Moves gap, doesn't insert
    
    // Now gap is at i+1, move it left to delete element i
    if(gi->gapPos > 0) {
        gi->gapPos--;
        gi->index[gi->gapPos] = NULL;  // Clear the deleted element
    }
    gi->logicalSize--;
}

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

int main() {
    LinkedList list;
    initLinkedList(&list, 50);
    
    printf("=== Testing Gap Buffer Indexed Linked List ===\n");
    
    // Test insertions
    insert(&list, 0, 10);
    insert(&list, 1, 20);
    insert(&list, 2, 30);
    insert(&list, 1, 15);  // Insert in middle
    printList(&list);
    
    // Test get (O(1))
    printf("\nTesting O(1) get:\n");
    for(int i = 0; i < list.size; i++) {
        Node* node = get(&list, i);
        if(node) {
            printf("Element at index %d: %d\n", i, node->data);
        }
    }
    
    // Test deletion
    printf("\nDeleting element at position 2:\n");
    delete(&list, 2);
    printList(&list);
    
    // Test invalid access
    printf("\nTesting invalid get at index 10:\n");
    Node* invalid = get(&list, 10);
    if(!invalid) {
        printf("Correctly returned NULL for invalid index\n");
    }
    
    return 0;
}