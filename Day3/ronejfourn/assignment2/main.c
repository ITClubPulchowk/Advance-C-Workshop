#include <stdint.h>
#include <stdio.h>
#include "bump.c"
#include "free_list.c"

typedef struct {
    char *data;
    size_t length;
} string;

typedef struct {
    char *data;
    string token;
} lexer;

typedef struct bst_node {
    struct bst_node *left, *right;
    uint32_t value;
} bst_node;

uint32_t alder32(string string) {
    uint32_t a = 1, b = 0;
    for (int i = 0; i < string.length; i ++) {
        a = (a + *(string.data + i)) % 65521;
        b = (b + a) % 65521;
    }
    return (b << 16) | a;
}

#define isspace(d) ((d) == '\r' || (d) == '\n')

void get_next(lexer *lex) {
    lex->token.data = lex->data;
    lex->token.length = 0;

    while (*lex->data && !isspace(*lex->data)) {
        lex->token.length ++;
        lex->data ++;
    }

    lex->data += *lex->data != 0;
    lex->data += *lex->data == '\n';
}

void create_bst_node(bst_node *tree, uint32_t value) {
    tree->left  = NULL;
    tree->right = NULL;
    tree->value = value;
}

void insert_into_bst(bst_node *tree, uint32_t value) {
    //assuming no collisions occur during hashing
    if (value > tree->value) {
        if (tree->left) {
            insert_into_bst(tree->left , value);
        } else {
            tree->left  = lalloc();
            create_bst_node(tree->left , value);
        }
    } else {
        if (tree->right) {
            insert_into_bst(tree->right, value);
        } else {
            tree->right = lalloc();
            create_bst_node(tree->right, value);
        }
    }
}

int main() {
    FILE *inp_file = fopen("data.txt", "rb");
    if (!inp_file) {
        printf("What File?\n");
        return -1;
    }
    fseek(inp_file, 0, SEEK_END);
    int file_size = ftell(inp_file);
    rewind(inp_file);

    // assuming data.txt will always be larger than search.txt and delete.txt
    init_bump_context(file_size + 2);  // 1 bit wiggle room
    char *buffer = bump(file_size + 1);
    fread(buffer, 1, file_size, inp_file);
    buffer[file_size] = 0;
    fclose(inp_file);

    size_t line_count = 1;
    for (char *trav = buffer; *trav; trav ++)
        line_count += *trav == '\n' || (*trav == '\r' && *(trav + 1) != '\n');

    list_context_init(line_count * sizeof(bst_node), sizeof(bst_node));

    lexer lex;
    lex.data = buffer;
    get_next(&lex);

    bst_node *tree = lalloc();
    create_bst_node(tree, alder32(lex.token));

    while (*lex.data) {
        get_next(&lex);
        insert_into_bst(tree, alder32(lex.token));
    }

    return 0;
}
