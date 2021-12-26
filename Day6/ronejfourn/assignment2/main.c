#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint32_t type;
    uint32_t cx, cy;
    union {
        uint32_t radius;
        uint32_t length;
    };
} shape;

typedef struct {
    uint32_t id;
    char *data;
    shape shp;
} lexer;

typedef int (*collision) (shape, shape);
#define abs(a) ((a) < 0 ? (a) * -1 : (a))

int circle_circle (shape c1, shape c2) {
    uint32_t dist_sqr = (c1.cx - c2.cx) * (c1.cx - c2.cx) + (c1.cy - c2.cy) * (c1.cy - c2.cy);
    return dist_sqr <= (c1.radius + c2.radius) * (c1.radius + c2.radius);
}

int circle_square (shape c1, shape c2) {
    shape circle, square;
    if (c1.type) {
        circle = c2;
        square = c1;
    } else {
        circle = c1;
        square = c2;
    }
    int dif_x = square.cx - circle.cx;
    int dif_y = square.cy - circle.cy;
    double c_sqr;
    if (abs(dif_x) > abs(dif_y)) {
        if (dif_x < 0) {
            c_sqr = (square.cx + square.length / 2.0 - circle.cx) * (square.cx + square.length / 2.0 - circle.cx);
        } else {
            c_sqr = (square.cx - square.length / 2.0 - circle.cx) * (square.cx - square.length / 2.0 - circle.cx);
        }
    } else {
        if (dif_y < 0) {
            c_sqr = (square.cy + square.length / 2.0 - circle.cy) * (square.cy + square.length / 2.0 - circle.cy);
        } else {
            c_sqr = (square.cy - square.length / 2.0 - circle.cy) * (square.cy - square.length / 2.0 - circle.cy);
        }
    }
    return c_sqr <= circle.radius * circle.radius;
}

int square_square (shape c1, shape c2) {
    int dif_x = c1.cx - c2.cx;
    int dif_y = c1.cy - c2.cy;
    if (abs(dif_x) > abs(dif_y)) {
        if (dif_x < 0) {
            return c2.cx - c2.length / 2 <= c1.cx + c1.length / 2;
        } else {
            return c2.cx + c2.length / 2 >= c1.cx - c1.length / 2;
        }
    } else {
        if (dif_y < 0) {
            return c2.cy - c2.length / 2 <= c1.cy + c1.length / 2;
        } else {
            return c2.cy + c2.length / 2 >= c1.cy - c1.length / 2;
        }
    }
}

#define isnumber(n) ((n) >= '0' && (n) <= '9')

uint32_t get_next_num(lexer *lex) {
    for (; *lex->data && !isnumber(*lex->data); lex->data ++);
    uint32_t ret = 0;
    for (; *lex->data &&  isnumber(*lex->data); lex->data ++)
        ret = ret * 10 + *lex->data - '0';
    return ret;
}

void get_next(lexer *lex) {
    lex->id   = get_next_num(lex);
    lex->shp.type = get_next_num(lex);
    lex->shp.cx   = get_next_num(lex);
    lex->shp.cy   = get_next_num(lex);
    if (lex->shp.type)
        lex->shp.length = get_next_num(lex);
    else
        lex->shp.radius = get_next_num(lex);
    lex->data += *lex->data != 0   ;
    lex->data += *lex->data == '\n';
}

int main() {
    FILE *f = fopen("input.csv", "rb");
    if (!f) {
        printf("could not open input.csv");
        return -1;
    }
    fseek(f, 0, SEEK_END);
    uint32_t fsize = ftell(f);
    rewind(f);
    char *buf = malloc(fsize + 1);
    buf[fsize] = 0;
    fread(buf, fsize, 1, f);
    fclose(f);

    char *trav = buf;
    uint32_t lcount = 0, shp_count = 0;
    for (; *trav; trav ++)
        lcount += *trav == '\n' || (*trav == '\r' && *(trav + 1) !='\n');
    shape *shapez = malloc(sizeof(shape) * lcount);

    lexer lex;
    lex.data = buf;

    collision dispatcher[] = {
        circle_circle,
        circle_square,
        square_square
    };

    while (*lex.data) {
        get_next(&lex);
        shapez[lex.id - 1] = lex.shp;
        shp_count ++;
    }

    for (int i = 0; i < shp_count; i ++)
        for (int j = i + 1; j < shp_count; j ++)
            printf("%d%c", dispatcher[shapez[i].type + shapez[j].type](shapez[i], shapez[j]), j == shp_count - 1 ? '\n' : ' ');

    return 0;
}
