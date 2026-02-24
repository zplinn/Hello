#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

const char alphabet[] = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
#define ALPHABET_SIZE (sizeof(alphabet)-1)

#define MAX_NODES 2000

char* password = NULL;

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

// Trie node structure
typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    uint64_t isEndOfWord;
} TrieNode;


size_t alloc_size = 0;
size_t num_nodes = 0;
// Create a new trie node
TrieNode* createNode() {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    alloc_size += sizeof(TrieNode);
    if (node) {
        node->isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    else {
        // oom
        perror("My patience wears thin for your games! Begone!");
        exit(0);
    }
    num_nodes += 1;
    return node;
}

// Search for a word in the trie
bool search(TrieNode* root, const char* word) {
    TrieNode* current = root;
    int length = strlen(word);

    for (int i = 0; i < length; i++) {
        int index = word[i] - 0x20;

        if (current->children[index] == NULL) {
            return false;  // Word not found
        }

        current = current->children[index];
    }

    return current->isEndOfWord;
}

// Insert a word into the trie
bool insert(TrieNode* root, const char* word) {
    TrieNode* current = root;
    int length = strlen(word);

    // count the number of nodes that need to be made
    int num_required_nodes = 0;
    for (int i = 0; i < length; i++) {
        int index = word[i] - 0x20;  // Convert char to index

        // Create new node if path doesn't exist
        if (current->children[index] == NULL) {
            num_required_nodes = length - i;
            break;
        }
        current = current->children[index];
    }

    if (num_required_nodes + num_nodes > MAX_NODES) {
        puts("I will not be bothered with such things!");
        return false;
    }

    current = root;
    for (int i = 0; i < length; i++) {
        int index = word[i] - 0x20;  // Convert char to index

        // Create new node if path doesn't exist
        if (current->children[index] == NULL) {
            current->children[index] = createNode();
        }

        current = current->children[index];
    }

    // Mark the end of word
    current->isEndOfWord = true;
    return true;
}

// Check if a node has any children
bool hasChildren(TrieNode* node) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            return true;
        }
    }
    return false;
}

// Delete a word from the trie (recursive helper)
TrieNode* deleteHelper(TrieNode* root, const char* word, int index) {
    if (index == strlen(word)) {
        // We've reached the end of the word
        if (root->isEndOfWord) {
            root->isEndOfWord = false;
        }

        // If root has no children, it can be deleted
        if (!hasChildren(root)) {
            num_nodes -= 1;
            free(root);
            root = NULL;
        }

        return root;
    }

    int charIndex = word[index] - 0x20;
    if (root->children[charIndex])
    {
        root->children[charIndex] = deleteHelper(root->children[charIndex], word, index + 1);
    }

    // If root doesn't have any child and is not end of another word
    if (!root->isEndOfWord && !hasChildren(root) && index != 0) {
        num_nodes -= 1;
        free(root);
        root = NULL;
    }

    return root;
}

// Delete a word from the trie
void delete(TrieNode* root, const char* word) {
    deleteHelper(root, word, 0);
}

// Free the entire trie
void freeTrie(TrieNode* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        freeTrie(root->children[i]);
    }

    free(root);
}


void menu() {
    puts("1. Tell the genie a fact\n"
    "2. Confirm a fact\n"
    "3. Refute a fact\n"
    "4. Enter the cave of knowledge\n"
    );
}


// Example usage
int main() {
    init();
    TrieNode* root = createNode();

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE* fp = fopen("facts.txt", "r");
    if (fp == NULL)
    {
        perror("Read failed");
        exit(1);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0)
            continue;
        char* password_ptr = strstr(line, "password is ");
        if (password_ptr != NULL)
        {
            password = strdup(password_ptr + 12);
        }

        if (!insert(root, line))
        {
            puts("Reading failed!");
            exit(1);
        }
    }

    fclose(fp);
    if (line)
    {
        free(line);
        line = NULL;
    }

    if (!password)
    {
        printf("Failed to read password!");
        exit(1);
    }

    char input[64] = {0};

    puts("After weeks of travel, you arrive at the entrance to the cave of knowledge.");
    puts("Suddenly, a genie appears in front of the entrance!");
    puts("\"I am the guardian of all knowledge. We may exchange knowledge here, but before I grant you access to the cave, you must tell me the password.\"");
    puts("");

    while (1)
    {
        menu();
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL)
            break;
        input[strcspn(input, "\n")] = 0;

        int choice = atoi(input);
        switch (choice) {
            case 1: {
                printf("You believe you know something I do not?\n> ");

                if (fgets(input, sizeof(input), stdin) == NULL)
                    break;
                input[strcspn(input, "\n")] = 0;

                if (search(root, input))
                {
                    puts("Fool! Of course I already know this!");
                }
                else
                {
                    if (insert(root, input))
                    {
                        puts("Very interesting...");
                    }
                }

                break;
            }
            case 2: {
                printf("Ask, mortal.\n> ");

                if (fgets(input, sizeof(input), stdin) == NULL)
                    break;
                input[strcspn(input, "\n")] = 0;

                if (search(root, input)) {
                    puts("Haha! Of course!");
                }
                else {
                    puts("Of course not! Are you a fool?!");
                }
                break;
            }
            case 3: {
                printf("Tell me: what fact do you say is false?\n> ");
                if (fgets(input, sizeof(input), stdin) == NULL)
                    break;
                input[strcspn(input, "\n")] = 0;

                if (!search(root, input))
                {
                    puts("Of course this is not true! I do not need you to tell me this!");
                    break;
                }

                delete(root, input);
                puts("Hmm... I see...");
                break;
            }
            case 4:
            {
                printf("What is the password?\n> ");
                if (fgets(input, sizeof(input), stdin) == NULL)
                    break;
                input[strcspn(input, "\n")] = 0;

                if (strcmp(password, input) == 0)
                {
                    puts("So it is! Enter!");
                    system("/bin/sh");
                }
                else
                {
                    puts("Incorrect!");
                }

                break;
            }
            default:
                puts("Invalid choice");
                break;
        }
    }

    // Clean up
    freeTrie(root);

    return 0;
}
