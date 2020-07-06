
#include "scapar.h"

int main(void)
{
    char *in = "scalar.dat";
    SCAPAR scapar;

    /* string */
    char *fn = "test";

    /* scale */
    SCAPAR_INT dim = -1, verb = -1;
    SCAPAR_FLT km = -1.;

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register scalar key words */
    scapar_register_int(&scapar, "dim", &dim);
    scapar_register_float(&scapar, "km", &km);
    scapar_register_string(&scapar, "fn", &fn);
    scapar_register_int(&scapar, "verb", &verb);

    /* print initial values */
    scapar_printf("initial values:\n");
    scapar_printf("dim: %"IFMT"\n", dim);
    scapar_printf("verb: %"IFMT"\n", verb);
    scapar_printf("km: %"FFMT"\n", km);
    if (fn != NULL) scapar_printf("fn: %s\n", fn);

    /* 3. parse */
    scapar_parse(&scapar);

    /* print info from keywords file */
    scapar_printf("\nvalues read from input file:\n");
    scapar_print_by_key(&scapar, "dim");
    scapar_print_by_key(&scapar, "verb");
    scapar_print_by_key(&scapar, "km");
    scapar_print_by_key(&scapar, "fn");

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
