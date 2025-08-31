#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 100

// Node for linked list (file/folder names)
typedef struct Node {
    char name[MAX_NAME_LEN];
    struct Node *next;
} Node;

// Linked list structure
typedef struct {
    Node *head;
} LinkedList;

// Stack structure (holds pairs of folder name + linked list pointer)
typedef struct StackNode {
    char folderName[MAX_NAME_LEN];
    LinkedList *contents;
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
} Stack;

// Function declarations
void linkedListInit(LinkedList *list);
void linkedListInsert(LinkedList *list, const char *name);
int linkedListDelete(LinkedList *list, const char *name);
int linkedListContains(LinkedList *list, const char *name);
void linkedListTraverse(LinkedList *list);
void linkedListClear(LinkedList *list);
LinkedList* linkedListCopy(LinkedList *original);

void stackInit(Stack *stack);
void stackPush(Stack *stack, const char *folderName, LinkedList *contents);
int stackPop(Stack *stack, char *folderName, LinkedList **contents);
int stackIsEmpty(Stack *stack);
void stackClear(Stack *stack);

void freeLinkedList(LinkedList *list);

void folderNavigationSystem();

int main() {
    folderNavigationSystem();
    return 0;
}

// LinkedList functions

void linkedListInit(LinkedList *list) {
    list->head = NULL;
}

void linkedListInsert(LinkedList *list, const char *name) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    strcpy(newNode->name, name);
    newNode->next = NULL;

    if (list->head == NULL) {
        list->head = newNode;
    } else {
        Node *curr = list->head;
        while (curr->next != NULL)
            curr = curr->next;
        curr->next = newNode;
    }
}

int linkedListDelete(LinkedList *list, const char *name) {
    Node *curr = list->head;
    Node *prev = NULL;

    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0) {
            if (prev == NULL) { // deleting head
                list->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return 1; // deleted
        }
        prev = curr;
        curr = curr->next;
    }
    return 0; // not found
}

int linkedListContains(LinkedList *list, const char *name) {
    Node *curr = list->head;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0)
            return 1;
        curr = curr->next;
    }
    return 0;
}

void linkedListTraverse(LinkedList *list) {
    Node *curr = list->head;
    if (curr == NULL) {
        printf(" (empty) ");
        return;
    }
    while (curr != NULL) {
        printf("- %s\n", curr->name);
        curr = curr->next;
    }
}

void linkedListClear(LinkedList *list) {
    Node *curr = list->head;
    while (curr != NULL) {
        Node *temp = curr;
        curr = curr->next;
        free(temp);
    }
    list->head = NULL;
}

LinkedList* linkedListCopy(LinkedList *original) {
    LinkedList *copy = (LinkedList *)malloc(sizeof(LinkedList));
    linkedListInit(copy);

    Node *curr = original->head;
    while (curr != NULL) {
        linkedListInsert(copy, curr->name);
        curr = curr->next;
    }
    return copy;
}

// Stack functions

void stackInit(Stack *stack) {
    stack->top = NULL;
}

void stackPush(Stack *stack, const char *folderName, LinkedList *contents) {
    StackNode *newNode = (StackNode *)malloc(sizeof(StackNode));
    strcpy(newNode->folderName, folderName);
    newNode->contents = linkedListCopy(contents); // copy the folder contents
    newNode->next = stack->top;
    stack->top = newNode;
}

int stackPop(Stack *stack, char *folderName, LinkedList **contents) {
    if (stack->top == NULL) return 0; // empty

    StackNode *topNode = stack->top;
    strcpy(folderName, topNode->folderName);
    *contents = topNode->contents;

    stack->top = topNode->next;
    free(topNode);
    return 1;
}

int stackIsEmpty(Stack *stack) {
    return stack->top == NULL;
}

void stackClear(Stack *stack) {
    while (stack->top != NULL) {
        StackNode *temp = stack->top;
        stack->top = temp->next;
        freeLinkedList(temp->contents);
        free(temp->contents);
        free(temp);
    }
}

// Helper function to free a linked list fully
void freeLinkedList(LinkedList *list) {
    linkedListClear(list);
}

// Main folder navigation system logic

void folderNavigationSystem() {
    Stack backStack, forwardStack;
    stackInit(&backStack);
    stackInit(&forwardStack);

    char currentFolder[MAX_NAME_LEN] = "root";
    LinkedList currentContents;
    linkedListInit(&currentContents);

    // Add initial items to root folder
    linkedListInsert(&currentContents, "Documents");
    linkedListInsert(&currentContents, "Pictures");
    linkedListInsert(&currentContents, "file1.txt");

    while (1) {
        printf("\nCurrent Folder: %s\n", currentFolder);
        printf("Menu:\n");
        printf("1. Navigate to a new folder\n");
        printf("2. Go Back\n");
        printf("3. Go Forward\n");
        printf("4. Add a file/folder\n");
        printf("5. Delete a file/folder\n");
        printf("6. Show folder contents\n");
        printf("7. Exit\n");

        int choice;
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); // flush invalid input
            printf("Invalid input. Please enter a number 1-7.\n");
            continue;
        }
        getchar(); // consume newline

        if (choice == 1) {
            char newFolder[MAX_NAME_LEN];
            printf("Enter folder name to navigate: ");
            fgets(newFolder, MAX_NAME_LEN, stdin);
            newFolder[strcspn(newFolder, "\n")] = 0; // remove newline

            if (!linkedListContains(&currentContents, newFolder)) {
                printf("Folder '%s' does not exist in current folder.\n", newFolder);
            } else {
                // push current state to back stack
                stackPush(&backStack, currentFolder, &currentContents);
                stackClear(&forwardStack);

                strcpy(currentFolder, newFolder);
                linkedListClear(&currentContents); // start with empty folder contents
                printf("Navigated to folder '%s'.\n", currentFolder);
            }

        } else if (choice == 2) {
            if (stackIsEmpty(&backStack)) {
                printf("No folder to go back to.\n");
            } else {
                stackPush(&forwardStack, currentFolder, &currentContents);
                char folderName[MAX_NAME_LEN];
                LinkedList *contentsPtr;
                stackPop(&backStack, folderName, &contentsPtr);

                strcpy(currentFolder, folderName);
                linkedListClear(&currentContents);
                currentContents = *contentsPtr;
                free(contentsPtr); // free the pointer but not contents since transferred
                printf("Moved back to '%s'.\n", currentFolder);
            }

        } else if (choice == 3) {
            if (stackIsEmpty(&forwardStack)) {
                printf("No folder to go forward to.\n");
            } else {
                stackPush(&backStack, currentFolder, &currentContents);
                char folderName[MAX_NAME_LEN];
                LinkedList *contentsPtr;
                stackPop(&forwardStack, folderName, &contentsPtr);

                strcpy(currentFolder, folderName);
                linkedListClear(&currentContents);
                currentContents = *contentsPtr;
                free(contentsPtr);
                printf("Moved forward to '%s'.\n", currentFolder);
            }

        } else if (choice == 4) {
            char newItem[MAX_NAME_LEN];
            printf("Enter file/folder name to add: ");
            fgets(newItem, MAX_NAME_LEN, stdin);
            newItem[strcspn(newItem, "\n")] = 0; // remove newline

            if (linkedListContains(&currentContents, newItem)) {
                printf("'%s' already exists.\n", newItem);
            } else {
                linkedListInsert(&currentContents, newItem);
                printf("'%s' added to '%s'.\n", newItem, currentFolder);
            }

        } else if (choice == 5) {
            char delItem[MAX_NAME_LEN];
            printf("Enter file/folder name to delete: ");
            fgets(delItem, MAX_NAME_LEN, stdin);
            delItem[strcspn(delItem, "\n")] = 0;

            if (linkedListDelete(&currentContents, delItem)) {
                printf("'%s' deleted from '%s'.\n", delItem, currentFolder);
            } else {
                printf("'%s' not found in '%s'.\n", delItem, currentFolder);
            }

        } else if (choice == 6) {
            printf("Contents of '%s':\n", currentFolder);
            linkedListTraverse(&currentContents);

        } else if (choice == 7) {
            printf("Exiting program.\n");
            break;

        } else {
            printf("Invalid choice. Please enter a number from 1 to 7.\n");
        }
    }

    // Cleanup before exit
    linkedListClear(&currentContents);
    stackClear(&backStack);
    stackClear(&forwardStack);
}
