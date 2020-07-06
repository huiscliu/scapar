
#include "scapar.h"

typedef struct STT_
{
    SCAPAR_FLT km;
    SCAPAR_INT dim;
    SCAPAR_INT verb;
    char *fn;

    SCAPAR_VEC vd;
    SCAPAR_VEC vf;
    SCAPAR_VEC vs;

    SCAPAR_VEC vvi;
    SCAPAR_VEC vvf;
    SCAPAR_VEC vvs;

} STT;


int main(void)
{
    char *in = "section.dat";
    SCAPAR scapar;
    STT s;
    SCAPAR_SEC *sec;

    /* init */
    s.km = 0;
    s.dim = s.verb = 0;
    s.fn = "section";

    scapar_printf("initial values:\n");
    scapar_printf("dim: %"IFMT"\n", s.dim);
    scapar_printf("verb: %"IFMT"\n", s.verb);
    scapar_printf("km: %"FFMT"\n", s.km);
    scapar_printf("fn: %s\n", s.fn);

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register section */
    scapar_register_section(&scapar, "sc-offset", &s);

    /* get section and handle member */
    sec = scapar_get_section(&scapar, "sc-offset");
    assert(sec != NULL);

    scapar_sec_register_offset(sec, &scapar, "dim", offsetof(STT, dim), SCAPAR_T_INT, NULL);
    scapar_sec_register_offset(sec, &scapar, "km", offsetof(STT, km), SCAPAR_T_FLT, NULL);
    scapar_sec_register_offset(sec, &scapar, "verb", offsetof(STT, verb), SCAPAR_T_INT, NULL);
    scapar_sec_register_offset(sec, &scapar, "fn", offsetof(STT, fn), SCAPAR_T_STR, NULL);

    scapar_sec_register_offset(sec, &scapar, "vd", offsetof(STT, vd), SCAPAR_T_VEC, SCAPAR_INFO_INT);
    scapar_sec_register_offset(sec, &scapar, "vf", offsetof(STT, vf), SCAPAR_T_VEC, SCAPAR_INFO_FLT);
    scapar_sec_register_offset(sec, &scapar, "vs", offsetof(STT, vs), SCAPAR_T_VEC, SCAPAR_INFO_STR);

    scapar_sec_register_offset(sec, &scapar, "vvi", offsetof(STT, vvi), SCAPAR_T_VEC, SCAPAR_INFO_VEC_INT);
    scapar_sec_register_offset(sec, &scapar, "vvf", offsetof(STT, vvf), SCAPAR_T_VEC, SCAPAR_INFO_VEC_FLT);
    scapar_sec_register_offset(sec, &scapar, "vvs", offsetof(STT, vvs), SCAPAR_T_VEC, SCAPAR_INFO_VEC_STR);

    /* 3. parse */
    scapar_parse(&scapar);

    /* print info from keywords file */
    scapar_printf("\nvalues read from input file:\n");
    scapar_sec_print_by_key(sec, "dim");
    scapar_sec_print_by_key(sec, "km");
    scapar_sec_print_by_key(sec, "verb");
    scapar_sec_print_by_key(sec, "fn");

    scapar_sec_print_by_key(sec, "vd");
    scapar_sec_print_by_key(sec, "vf");
    scapar_sec_print_by_key(sec, "vs");

    scapar_sec_print_by_key(sec, "vvi");
    scapar_sec_print_by_key(sec, "vvf");
    scapar_sec_print_by_key(sec, "vvs");

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
