#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct node {
    struct node *left, *right, *parent;
    int precedence;
    union {
        float num;
        char op;
    };
    char is_left;

} node;

typedef struct {
    node **stack;
    int ptr, cap;
} stack;

enum {
    number = -1,
    div_or_mul,
    add_or_sub
};

#define STACK_CAP_INIT 4

static void init_stack(stack *ret){
    ret->stack = malloc(sizeof(*ret->stack) * STACK_CAP_INIT);
    ret->ptr = -1;
    ret->cap = STACK_CAP_INIT;
}

static void push(stack *stack, node* node){
    stack->ptr ++;
    if (stack->ptr >= stack->cap) {
        stack->cap *= 2;
        stack->stack = realloc(stack->stack, stack->cap);
    }
    stack->stack[stack->ptr] = node;
}

static node *pop(stack *stack){
    stack->ptr --;
    return stack->stack[stack->ptr + 1];
}

static node *create_node(node *parent){
    node *ret    = malloc(sizeof(*ret));
    ret->parent  = parent;
    ret->left    = NULL;
    ret->right   = NULL;
    return ret;
}

static void add_branch(node *parent, node *child){
    if (!parent->left){
        parent->left = child;
        child->is_left = 1;
    } else {
        parent->right = child;
        child->is_left = 0;
    }
}

static void adopt(node *parent, node *child){
    parent->parent = child->parent;
    parent->is_left = 0;
    if (child->parent) {
        if (!child->parent->left) {
            child->parent->left = parent;
            parent->is_left = 1;
        } else {
            child->parent->right = parent;
        }
    }
    child->parent  = parent;
    add_branch(parent, child);
}

static void solve(node *head) {
    if (head->precedence == number) return;
    solve(head->left);
    solve(head->right);
    head->precedence = number;
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
    free(head->left);
    free(head->right);
}

int main(){
    FILE *inp_file = fopen("expressions.txt", "rb");
    if (!inp_file) {
        printf("What file?\n");
        return -2;
    }
    fseek(inp_file, SEEK_SET, SEEK_END);
    int size = ftell(inp_file);
    rewind(inp_file);

    char *data = malloc(size + 1);
    fread(data, 1, size, inp_file);
    data[size] = 0;
    fclose(inp_file);

    stack node_stack;
    init_stack(&node_stack);

    node *head = create_node(NULL);
    head->num = 0;
    for (; isspace(*data); data ++);
    for (; *data >= '0' && *data <= '9'; data ++)
        head->num = head->num * 10 + *data - '0';
    head->precedence = number;
    head->is_left = 0;
    push(&node_stack, head);
    node *curr = head;

    char add_to_stack = 0;

    FILE *out_file = fopen("output.txt", "wb");
    while (*data){
        if (*data >= '0' && *data <= '9') {
            node *tmp = create_node(curr);
            tmp->num  = 0;
            for (; *data >= '0' && *data <= '9'; data ++)
                tmp->num = tmp->num * 10 + *data - '0';
            data --;
            tmp->precedence = number;
            if (add_to_stack) {
                add_to_stack = 0;
                push(&node_stack, tmp);
            }
            add_branch(curr, tmp);
            curr = tmp;
        } else if (*data == '+' || *data == '-' || *data == '/' || *data == '*') {
            node *tmp = create_node(curr);
            char dorm = *data == '/' || *data == '*';
            tmp->op   = *data;
            tmp->precedence = dorm;
            if (dorm) {
                if (curr == node_stack.stack[node_stack.ptr]) {
                    adopt(tmp, pop(&node_stack));
                    push(&node_stack, tmp);
                    if (!node_stack.ptr) head = tmp;
                } else {
                    adopt(tmp, curr);
                }
            } else {
                adopt(tmp, pop(&node_stack));
                push(&node_stack, tmp);
                if (!node_stack.ptr) head = tmp;
            }
            curr = tmp;
        } else if (*data == '(') {
            add_to_stack = 1;
        } else if (*data == ')') {
            node_stack.ptr --;
        }
        data ++;
        if (*data == '\n' || (*data == '\r' && *(data + 1) != '\n') || !*data) {
            solve(head);
            fprintf(out_file, "%f\n", head->num);
            free(head);
            data++;
            if (!*data) break;
            node_stack.ptr = -1;
            head = create_node(NULL);
            head->is_left = 0;
            head->num = 0;
            for (; isspace(*data); data ++);
            for (; *data >= '0' && *data <= '9'; data ++)
                head->num = head->num * 10 + *data - '0';
            head->precedence = number;
            push(&node_stack, head);
            curr = head;
        }
    }
    fclose(out_file);
    return 0;
}
