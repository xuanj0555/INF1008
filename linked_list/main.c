#include "linked_list.h"

int main() {
    LinkedList list;
    initLinkedList(&list, 10);  // Start with small capacity to test resizing

    printf("=== Testing Gap Buffer Indexed Linked List ===\n");

    // Test 1: Basic operations
    printf("\n--- Test 1: Basic Insertions ---\n");
    insert(&list, 0, 10);
    insert(&list, 1, 20);
    insert(&list, 2, 30);
    insert(&list, 1, 15);  // Insert in middle
    printList(&list);
    printGapState(&list.gapIndex);

    // Test 2: O(1) access verification
    printf("\n--- Test 2: O(1) Get Operations ---\n");
    for (int i = 0; i < list.size; i++) {
        Node* node = get(&list, i);
        if (node) {
            printf("Element at index %d: %d\n", i, node->data);
        }
    }

    // Test 3: Deletion
    printf("\n--- Test 3: Deletion ---\n");
    delete(&list, 2);
    printList(&list);
    printGapState(&list.gapIndex);

    // Test 4: Stress test with resizing
    printf("\n--- Test 4: Stress Test (50 insertions) ---\n");
    printf("Initial capacity: %d\n", list.gapIndex.capacity);

    for (int i = 0; i < 50; i++) {
        insert(&list, list.size, i * 100);  // Insert at end
    }

    printf("After 50 insertions:\n");
    printf("Final capacity: %d (should be resized)\n", list.gapIndex.capacity);
    printf("List size: %d\n", list.size);

    // Test 5: Random access in large list
    printf("\n--- Test 5: Random Access in Large List ---\n");
    printf("Accessing index 0: %d\n", get(&list, 0)->data);
    printf("Accessing index 25: %d\n", get(&list, 25)->data);
    printf("Accessing index 49: %d\n", get(&list, 49)->data);

    // Test 6: Invalid access
    printf("\n--- Test 6: Error Handling ---\n");
    Node* invalid = get(&list, 100);
    if (!invalid) {
        printf("Correctly returned NULL for invalid index 100\n");
    }

    // Test 7: Multiple deletions
    printf("\n--- Test 7: Multiple Deletions ---\n");
    delete(&list, 0);  // Delete head
    delete(&list, list.size / 2);  // Delete middle
    delete(&list, list.size - 1);  // Delete tail
    printf("After deletions, list size: %d\n", list.size);

    // Clean up
    freeLinkedList(&list);
    printf("\n=== All tests completed, memory freed ===\n");

    return 0;
}