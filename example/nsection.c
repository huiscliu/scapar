
#include "scapar.h"

/* a demo for handling complex struct */

typedef struct SEC_ST1_
{
    SCAPAR_FLT x;
    SCAPAR_INT d;
    SCAPAR_INT v;

    SCAPAR_VEC vd;

} SEC_ST1;

typedef struct SEC_STT_
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

    /* struct of struct */
    SEC_ST1 st;

} SEC_STT;


int main(void)
{
    char *in = "nsection.dat";
    SCAPAR scapar;
    SEC_STT s;
    SCAPAR_SEC *sec, *s2;

    /* init */
    s.km = 0;
    s.dim = s.verb = 0;
    s.fn = NULL;

    scapar_printf("initial values:\n");
    scapar_printf("dim: %"IFMT"\n", s.dim);
    scapar_printf("verb: %"IFMT"\n", s.verb);
    scapar_printf("km: %g\n", s.km);
    if (s.fn != NULL) scapar_printf("fn: %s\n", s.fn);

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register first level section */
    scapar_register_section(&scapar, "sec-stt", &s);

    /* get section and handle member */
    sec = scapar_get_section(&scapar, "sec-stt");
    assert(sec != NULL);

    scapar_sec_register_offset(sec, &scapar, "dim", offsetof(SEC_STT, dim), SCAPAR_T_INT, NULL);
    scapar_sec_register_offset(sec, &scapar, "km", offsetof(SEC_STT, km), SCAPAR_T_FLT, NULL);
    scapar_sec_register_offset(sec, &scapar, "verb", offsetof(SEC_STT, verb), SCAPAR_T_INT, NULL);
    scapar_sec_register_offset(sec, &scapar, "fn", offsetof(SEC_STT, fn), SCAPAR_T_STR, NULL);

    scapar_sec_register_offset(sec, &scapar, "vd", offsetof(SEC_STT, vd), SCAPAR_T_VEC, SCAPAR_INFO_INT);
    scapar_sec_register_offset(sec, &scapar, "vf", offsetof(SEC_STT, vf), SCAPAR_T_VEC, SCAPAR_INFO_FLT);
    scapar_sec_register_offset(sec, &scapar, "vs", offsetof(SEC_STT, vs), SCAPAR_T_VEC, SCAPAR_INFO_STR);

    scapar_sec_register_offset(sec, &scapar, "vvi", offsetof(SEC_STT, vvi), SCAPAR_T_VEC, SCAPAR_INFO_VEC_INT);
    scapar_sec_register_offset(sec, &scapar, "vvf", offsetof(SEC_STT, vvf), SCAPAR_T_VEC, SCAPAR_INFO_VEC_FLT);
    scapar_sec_register_offset(sec, &scapar, "vvs", offsetof(SEC_STT, vvs), SCAPAR_T_VEC, SCAPAR_INFO_VEC_STR);

    /* 2. register second level section */
    scapar_sec_register_offset(sec, &scapar, "sec-st1", offsetof(SEC_STT, st), SCAPAR_T_SEC, NULL);

    /* get section and handle member */
    s2 = scapar_sec_get_section(sec, "sec-st1");
    assert(s2 != NULL);

    scapar_sec_register_offset(s2, &scapar, "x", offsetof(SEC_ST1, x), SCAPAR_T_FLT, NULL);
    scapar_sec_register_offset(s2, &scapar, "d", offsetof(SEC_ST1, d), SCAPAR_T_INT, NULL);
    scapar_sec_register_offset(s2, &scapar, "v", offsetof(SEC_ST1, v), SCAPAR_T_INT, NULL);
    scapar_sec_register_offset(s2, &scapar, "vd", offsetof(SEC_ST1, vd), SCAPAR_T_VEC_INT, NULL);

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

    scapar_sec_print_by_key(s2, "x");
    scapar_sec_print_by_key(s2, "d");
    scapar_sec_print_by_key(s2, "v");
    scapar_sec_print_by_key(s2, "vd");

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
