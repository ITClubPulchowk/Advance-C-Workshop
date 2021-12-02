#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    struct node *left, *right;
    union {
        float num;
        char op;
    };
    char prec;
} node;

typedef struct token {
    int val, is, inc;
} token;

typedef struct {
    node **stack;
    int ptr, cap;
} stack;

enum { op, num, par };
#define NUMBER -1

token get_next_token(char *buf){
    int counter = 0;
    for (; isspace(*buf); buf ++) counter ++;
    if (*buf >= '0' && *buf <= '9') {
        int val = 0;
        for (; *buf >= '0' && *buf <= '9'; buf ++) {
            val = val * 10 + *buf - '0';
            counter ++;
        }
        return (token){val, num, counter};
    } else if (*buf == '(' || *buf == ')'){
        return (token){*buf, par, counter + 1};
    } else {
        return (token){*buf, op, counter + 1};
    }
}

node *create_node(int val, int is_op){
    node *ret = malloc(sizeof(*ret));
    ret->left  = NULL;
    ret->right = NULL;
    if (is_op) {
        ret->op   = val;
        ret->prec = val == '*' || val == '/';
    } else {
        ret->num  = val;
        ret->prec = -1;
    }
    return ret;
}

// just in case
/* void add_node(node *parent, node *child){ */
/*     if (parent->left) */
/*         parent->right = child; */
/*     else */
/*         parent->left  = child; */
/* } */

void init_stack(stack *stack){
    stack->stack = malloc(4 * sizeof(node *));
    stack->cap   = 4;
    stack->ptr   = -1;
}

void push(stack *stack, node *val){
    stack->ptr ++;
    if (stack->ptr >= stack->cap) {
        stack->cap *= 2;
        stack->stack = realloc(stack->stack, stack->cap * sizeof(node *));
    }
    stack->stack[stack->ptr] = val;
}

static void solve(node *head) {
    if (head->prec == NUMBER) return;
    solve(head->left);
    solve(head->right);
    head->prec = NUMBER;
    switch(head->op) {
        case '+':
            head->num = head->left->num + head->right->num;
            break;
        case '-':
            head->num = head->left->num - head->right->num;
            break;
        case '/':
            head->num = head->left->num / head->right->num;
            break;
        case '*':
            head->num = head->left->num * head->right->num;
            break;
        default: break;
    }
    free(head->left );
    free(head->right);
}

node *make_tree(char **data, stack *b_stack) {
    node *head;
    token tmp = get_next_token(*data);
    if (tmp.is == op) {
        head = create_node(tmp.val, 1);
        *data += tmp.inc;
    } else {
        head = create_node('+', 1);
    }
    /* add_node(head, create_node(0, 0)); // adds to left */
    head->left = create_node(0, 0);
    push(b_stack, head);
    node *cur = head;

    int track = 0, add_to_stack = 0;

    while (**data && **data != '\n' && **data != '\r') {
        token tok = get_next_token(*data);
        *data += tok.inc;
        if (tok.is == op) {
            node *tmp = create_node(tok.val, 1);
            if (tmp->prec > cur->prec) {
                cur->right = tmp;
                cur = cur->right;
                /* add_node(cur, create_node(track, 0)); // adds to left */
                cur->left = create_node(track, 0); // adds to left
            } else {
                /* add_node(cur, create_node(track, 0)); // adds to right */
                cur->right = create_node(track, 0); // adds to right
                /* add_node(tmp, b_stack->stack[b_stack->ptr]->right); // adds to left */
                tmp->left = b_stack->stack[b_stack->ptr]->right; // adds to left
                b_stack->stack[b_stack->ptr]->right = tmp;
                cur = tmp;
            }
        } else if (tok.is == num){
            track = tok.val;
        } else if (tok.is == par){
            if (tok.val == '(') {
                push(b_stack, cur);
            } else {
                b_stack->ptr--;
            }
        }
    }
    *data += **data != 0;
    *data += **data == '\n';  // windows :PepeHands:
    cur->right = create_node(track, 0); // adds to right
    return head;
}

int main() {
    FILE *inp_file = fopen("expressions.txt", "rb");
    if (!inp_file) {
        printf("What file?\n");
        return -2;
    }
    fseek(inp_file, SEEK_SET, SEEK_END);
    int size = ftell(inp_file);
    rewind(inp_file);

    char *data = malloc(size + 2);
    fread(data, 1, size, inp_file);
    data[size] = 0;
    fclose(inp_file);

    stack bracket_stack;
    init_stack(&bracket_stack);

    FILE *out_file = fopen("output.txt", "wb");
    while (*data) {
        node *head = make_tree(&data, &bracket_stack);
        solve(head);
        fprintf(out_file, "%f\n", head->num);
        bracket_stack.ptr = -1;
    }
    fclose(out_file);
}
