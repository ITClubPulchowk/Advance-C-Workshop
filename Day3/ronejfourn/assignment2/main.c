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
    } else if (value < tree->value) {
        if (tree->right) {
            insert_into_bst(tree->right, value);
        } else {
            tree->right = lalloc();
            create_bst_node(tree->right, value);
        }
    } else {
        return;
    }
}

bool find_bst_node(bst_node *tree, uint32_t value) {
    if (value > tree->value) {
        if (tree->left)
            return find_bst_node(tree->left , value);
        else
            return false;
    } else if (value < tree->value) {
        if (tree->right)
            return find_bst_node(tree->right, value);
        else
            return false;
    } else
        return true;
}

void delete_bst_node(bst_node *tree, uint32_t value) {
    if (tree->value == value) {
        tree->value = 0;
    } else if (tree->left && tree->left ->value == value) {
        void *tmp = tree->left;
        if (!tree->left->left && !tree->left->right) {
            tree->left = NULL;
        } else if (!tree->left->left) {
            tree->left = tree->left->right;
        } else if (!tree->left->right) {
            tree->left = tree->left->left;
        } else {
            bst_node *trav = tree->left->left->right;
            for (; trav; trav = trav->right);
            trav->right = tree->left->right;
            tree->left  = tree->left->left;
        }
        lfree(tmp);
    } else if (tree->right && tree->right->value == value) {
        void *tmp = tree->right;
        if (!tree->right->left && !tree->right->right) {
            tree->right = NULL;
        } else if (!tree->right->left) {
            tree->right = tree->right->right;
        } else if (!tree->right->right) {
            tree->right = tree->right->left;
        } else {
            bst_node *trav = tree->right->right->left;
            for (; trav; trav = trav->left);
            trav->left = tree->right->left;
            tree->right  = tree->right->right;
        }
        lfree(tmp);
    } else {
        if (value > tree->value && tree->left) {
            delete_bst_node(tree->left , value);
        } else if (tree->right) {
            delete_bst_node(tree->right, value);
        }
    }
}

int main() {
    FILE *inp_file = fopen("data.txt", "rb");
    if (!inp_file) {
        printf("data.txt missing\n");
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

    init_list_context(line_count * sizeof(bst_node), sizeof(bst_node));

    lexer lex;
    lex.data = buffer;
    get_next(&lex);

    bst_node *tree = lalloc();
    create_bst_node(tree, alder32(lex.token));

    while (*lex.data) {
        get_next(&lex);
        insert_into_bst(tree, alder32(lex.token));
    }

    reset_bump_context();

    inp_file = fopen("delete.txt", "rb");
    if (inp_file) {
        fseek(inp_file, 0, SEEK_END);
        file_size = ftell(inp_file);
        rewind(inp_file);

        buffer = bump(file_size + 1);
        fread(buffer, 1, file_size, inp_file);
        buffer[file_size] = 0;
        fclose(inp_file);

        lex.data = buffer;

        while (*lex.data) {
            get_next(&lex);
            delete_bst_node(tree, alder32(lex.token));
        }
        reset_bump_context();
    } else {
        printf("delete.txt not found. Skipping\n");
    }

    inp_file = fopen("search.txt", "rb");
    if(!inp_file) {
        printf("search.txt not found\n");
        return -2;
    }

    fseek(inp_file, 0, SEEK_END);
    file_size = ftell(inp_file);
    rewind(inp_file);

    buffer = bump(file_size + 1);
    fread(buffer, 1, file_size, inp_file);
    buffer[file_size] = 0;
    fclose(inp_file);

    lex.data = buffer;

    FILE *out_file = fopen("result.txt", "wb");
    while (*lex.data) {
        get_next(&lex);
        if (find_bst_node(tree, alder32(lex.token)))
            fprintf(out_file, "Found\n");
        else
            fprintf(out_file, "Not Found\n");
    }
    fclose(out_file);
    end_list_context();
    end_bump_context();
    return 0;
}
