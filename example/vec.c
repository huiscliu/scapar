
#include "scapar.h"

int main(void)
{
    char *in = "vec.dat";
    SCAPAR scapar;

    SCAPAR_IDX i, k;

    /* vector */
    SCAPAR_VEC vi;
    SCAPAR_VEC vf;
    SCAPAR_VEC vs;

    SCAPAR_VEC svi;
    SCAPAR_VEC svf;
    SCAPAR_VEC svs;

    SCAPAR_VEC vvi;
    SCAPAR_VEC vvf;
    SCAPAR_VEC vvs;

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register vector key words (vector of a type) */
    scapar_register_vec(&scapar, "vi", &vi, SCAPAR_INFO_INT);
    scapar_register_vec(&scapar, "vf", &vf, SCAPAR_INFO_FLT);
    scapar_register_vec(&scapar, "vs", &vs, SCAPAR_INFO_STR);

    scapar_register_vec_int(&scapar, "svi", &svi);
    scapar_register_vec_float(&scapar, "svf", &svf);
    scapar_register_vec_string(&scapar, "svs", &svs);

    /* 2. register a vector of vector, (vector of vector of type) */
    scapar_register_vec(&scapar, "vvi", &vvi, SCAPAR_INFO_VEC_INT);
    scapar_register_vec(&scapar, "vvf", &vvf, SCAPAR_INFO_VEC_FLT);
    scapar_register_vec(&scapar, "vvs", &vvs, SCAPAR_INFO_VEC_STR);

    /* 3. parse */
    scapar_parse(&scapar);

    /* vi */
    scapar_printf("parsed value for vi (vector of int) \n");
    scapar_print_by_key(&scapar, "vi");

    /* another way, vi, k = vi.size */
    k = scapar_vec_get_size(&vi);

    if (k > 0) {
        scapar_printf("vi: ");
        for (i = 0; i < k; i++) scapar_printf(" %"IFMT" ", scapar_vec_int_get_value(&vi, i));
        printf("\n");
    }

    /* vf, k = vf.size */
    scapar_printf("\nparsed value for vf (vector of float)\n");
    k = scapar_vec_get_size(&vf);

    if (k > 0) {
        scapar_printf("vf: ");
        for (i = 0; i < k; i++) scapar_printf(" %"FFMT, scapar_vec_float_get_value(&vf, i));
        printf("\n");
    }

    /* vf */
    scapar_print_by_key(&scapar, "vf");

    /* vs */
    scapar_printf("\nparsed value for vs (vector of string)\n");
    scapar_print_by_key(&scapar, "vs");

    /* svi */
    scapar_printf("\nparsed value for svi (vector of int)\n");
    scapar_print_by_key(&scapar, "svi");

    /* svf */
    scapar_printf("\nparsed value for svf (vector of float)\n");
    scapar_print_by_key(&scapar, "svf");

    /* svs */
    scapar_printf("\nparsed value for svs (vector of string)\n");
    scapar_print_by_key(&scapar, "svs");

    /* vvi */
    scapar_printf("\nparsed value for vvi (vector of vector of int)\n");
    scapar_print_by_key(&scapar, "vvi");

    /* vvf */
    scapar_printf("\nparsed value for vvf (vector of vector of float)\n");
    scapar_print_by_key(&scapar, "vvf");

    /* vvs */
    scapar_printf("\nparsed value for vvs (vector of vector of string)\n");
    scapar_print_by_key(&scapar, "vvs");

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
