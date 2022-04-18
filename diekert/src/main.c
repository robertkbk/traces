#include <stdio.h>
#include <stdlib.h>
#include <err.h>

enum {
    COLOR_A = 0xffa44f,
    COLOR_B = 0x6fff4f,
    COLOR_C = 0x4fb3ff
};

void drawD1(FILE*, int);
void drawD2(FILE*, int);
void drawD3(FILE*, int);
void drawD4(FILE*, int);
void drawD5(FILE*, int);

void labelNodes(FILE*, int);

int main(int argc, char** argv)
{
    if (argc < 2)
        errx(EXIT_FAILURE, "usage: %s <n> [filename]\n", argv[0]);

    FILE* out = stdout;

    int n = atoi(argv[1]);

    if (n < 2)
        errx(EXIT_FAILURE, 
            "invalid parameter <n> (n = %d, argv = %s)\n", n, argv[1]);

    if (argc >= 3)
    {
        out = fopen(argv[2], "w");
        
        if (out == NULL)
            err(EXIT_FAILURE, "fopen");
    }
    
    fputs("digraph {\n", out);
    
    drawD1(out, n);
    drawD2(out, n);
    drawD3(out, n);
    drawD4(out, n);
    drawD5(out, n);

    labelNodes(out, n);

    fputs("}\n", out);

    return EXIT_SUCCESS;
}

#define EdgeD1(i, j, k)     "\tA_%d_%d -> B_%d_%d_%d;\n"    , i, k, i, j, k
#define EdgeD2(i, j, k)     "\tB_%d_%d_%d -> C_%d_%d_%d;\n" , i, j, k, i, j, k
#define EdgeD3(i, j, k, l)  "\tC_%d_%d_%d -> A_%d_%d;\n"    , l, j, i, i, k
#define EdgeD4(i, j, k, l)  "\tC_%d_%d_%d -> C_%d_%d_%d;\n" , l, j, k, i, j, k
#define EdgeD5(i, j, k, l)  "\tC_%d_%d_%d -> B_%d_%d_%d;\n" , l, j, k, i, j, k

#define __color "fillcolor=\"#%06x\""
#define __style "style=filled"

#define __labelA "label=<A<sub>%d,%d</sub>>"
#define __labelB "label=<B<sub>%d,%d,%d</sub>>"
#define __labelC "label=<C<sub>%d,%d,%d</sub>>"

#define __LabelAFormat "\tA_%d_%d [" __labelA "," __color "," __style "];\n"
#define __LabelBFormat "\tB_%d_%d_%d [" __labelB "," __color "," __style "];\n"
#define __LabelCFormat "\tC_%d_%d_%d [" __labelC "," __color "," __style "];\n"

#define LabelA(i, k, color)        __LabelAFormat, i, k, i, k, color
#define LabelB(i, j, k, color)     __LabelBFormat, i, j, k, i, j, k, color
#define LabelC(i, j, k, color)     __LabelCFormat, i, j, k, i, j, k, color

#define fori(var, start, end) for (int var = (start); var < (end); var++)

void drawD1(FILE* f, int n)
{
    fori (i, 0, n - 1)
        fori (j, i, n + 1)
            fori (k, i + 1, n)
                fprintf(f, EdgeD1(i, j, k));
}

void drawD2(FILE* f, int n)
{
    fori (i, 0, n - 1)
        fori (j, i, n + 1)
            fori (k, i + 1, n)
                fprintf(f, EdgeD2(i, j, k));
}

void drawD3(FILE* f, int n)
{
    fori (i, 0, n - 1)
        fori (k, i + 1, n)
            fori (l, 0, i)
                fprintf(f, EdgeD3(i, i, k, l));
}

void drawD4(FILE* f, int n)
{
    fori (i, 0, n - 1)
        fori (k, i + 1, n)
            fori (l, 0, i)
                fprintf(f, EdgeD4(i, i, k, l));
}

void drawD5(FILE* f, int n)
{
    fori (i, 0, n - 1)
        fori (k, i + 1, n)
            fori (l, 0, i)
                fprintf(f, EdgeD5(i, i, k, l));
}

void labelNodes(FILE* f, int n)
{
    fori (i, 0, n - 1)
        fori (k, i + 1, n)
            fprintf(f, LabelA(i, k, COLOR_A));

    fori (i, 0, n - 1)
        fori (j, i, n + 1)
            fori (k, i + 1, n)
                fprintf(f, LabelB(i, j, k, COLOR_B));

    fori (i, 0, n - 1)
        fori (j, i, n + 1)
            fori (k, i + 1, n)
                fprintf(f, LabelC(i, j, k, COLOR_C));
}
