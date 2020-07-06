
#include "scapar.h"

typedef struct SEC_STT_
{
    SCAPAR_FLT km;
    SCAPAR_INT dim;
    SCAPAR_INT verb;
    char *fn;

} SEC_STT;


int main(void)
{
    char *in = "struct.dat";
    SCAPAR scapar;
    SEC_STT s;

    /* init */
    s.km = 0;
    s.dim = s.verb = 0;
    s.fn = "struct";

    scapar_printf("initial values:\n");
    scapar_printf("dim: %"IFMT"\n", s.dim);
    scapar_printf("verb: %"IFMT"\n", s.verb);
    scapar_printf("km: %"FFMT"\n", s.km);
    scapar_printf("fn: %s\n", s.fn);

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register scalar key words */
    scapar_register_int(&scapar, "dim", &s.dim);
    scapar_register_float(&scapar, "km", &s.km);
    scapar_register_string(&scapar, "fn", &s.fn);
    scapar_register_int(&scapar, "verb", &s.verb);

    /* 3. parse */
    scapar_parse(&scapar);

    /* print info from keywords file */
    scapar_printf("\nvalues read from input file:\n");
    scapar_print_by_key(&scapar, "dim");
    scapar_print_by_key(&scapar, "verb");
    scapar_print_by_key(&scapar, "km");
    scapar_print_by_key(&scapar, "fn");

    scapar_printf("\nparsed values:\n");
    scapar_printf("dim: %"IFMT"\n", s.dim);
    scapar_printf("verb: %"IFMT"\n", s.verb);
    scapar_printf("km: %"FFMT"\n", s.km);
    scapar_printf("fn: %s\n", s.fn);

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
