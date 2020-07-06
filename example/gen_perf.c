
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    int nit = 1000;
    int i;
    FILE *s;

    if (argc >= 2) {
        i = atoi(argv[1]);

        if (i > 0) nit = i;
    }

    /* generate codes */
    s = fopen("perf.c", "w+");
    assert(s != NULL);

    fprintf(s, "\n#include \"scapar.h\"\n\n");

    fprintf(s, "int main(void)\n{\n");

    fprintf(s, "    char *in = \"perf.dat\";\n");
    fprintf(s, "    SCAPAR scapar;\n");
    fprintf(s, "    double tm;\n\n");

    /* int */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    SCAPAR_INT si%d;\n", i);
    }

    fprintf(s, "\n");

    /* float */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    SCAPAR_FLT sf%d;\n", i);
    }

    fprintf(s, "\n");

    /* vector of int */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    SCAPAR_VEC vi%d;\n", i);
    }

    fprintf(s, "\n");

    /* vector of float */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    SCAPAR_VEC vf%d;\n", i);
    }

    fprintf(s, "\n");

    fprintf(s, "    printf(\"Generating %d integers, float, vectors of integers, and vector of float\\n\");\n", nit);
    fprintf(s, "\n");

    /* init */
    fprintf(s, "    /* init */\n");
    fprintf(s, "    scapar_init(&scapar, in);\n");
    fprintf(s, "    scapar_set_dup_check(&scapar, 0);\n");
    fprintf(s, "\n");

    fprintf(s, "    /* register */\n");
    fprintf(s, "    tm = scapar_get_time();\n");

    /* register int */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    scapar_register_int(&scapar, \"si%d\", &si%d);\n", i, i);
    }

    fprintf(s, "\n");

    /* register float */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    scapar_register_float(&scapar, \"sf%d\", &sf%d);\n", i, i);
    }

    fprintf(s, "\n");

    /* register vector of int */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    scapar_register_vec_int(&scapar, \"vi%d\", &vi%d);\n", i, i);
    }
    fprintf(s, "\n");

    /* register vector of float */
    for (i = 0; i < nit; i++) {
        fprintf(s, "    scapar_register_vec_float(&scapar, \"vf%d\", &vf%d);\n", i, i);
    }
    fprintf(s, "\n");

    fprintf(s, "    tm = scapar_get_time() - tm;\n");
    fprintf(s, "    printf(\"time for registering %d parameters: %%g s\\n\", tm);\n", nit * 4);
    fprintf(s, "\n");

    fprintf(s, "    /* parse */\n");
    fprintf(s, "    tm = scapar_get_time();\n");
    fprintf(s, "    scapar_parse(&scapar);\n");
    fprintf(s, "    tm = scapar_get_time() - tm;\n");
    fprintf(s, "    printf(\"time for parsing %d parameters: %%g s\\n\", tm);\n", nit * 4);
    fprintf(s, "\n");

    fprintf(s, "    /* finalize */\n");
    fprintf(s, "    scapar_finalize(&scapar);\n");
    fprintf(s, "\n");
    fprintf(s, "    return 0;\n");
    fprintf(s, "}\n");

    fclose(s);

    /* generate data */
    s = fopen("perf.dat", "w+");

    /* int */
    for (i = 0; i < nit; i++) {
        fprintf(s, "si%d: %d\n", i, i);
    }

    fprintf(s, "\n");

    /* float */
    for (i = 0; i < nit; i++) {
        fprintf(s, "sf%d: %g\n", i, i + 2.5);
    }

    fprintf(s, "\n");

    /* vector of int */
    for (i = 0; i < nit; i++) {
        fprintf(s, "vi%d: 1 2 3 4 5 6 7 8 9 10\n", i);
    }

    /* vector of float */
    for (i = 0; i < nit; i++) {
        fprintf(s, "vf%d: 1.2 2.3 3 4 5 6 7 8e-3 9.2222 10.11111\n", i);
    }

    fclose(s);
    return 0;
}
