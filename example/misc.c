
#include "scapar.h"

static void func_conv(void *addr, FILE *f, char *p, SCAPAR_ITEM *kp)
{
    int *x;

    assert(addr != NULL);
    assert(p != NULL);
    (void)f;
    (void)kp;

    x = addr;
    *x= atoi(p);

    /* mark parsed */
    kp->parsed = SCAPAR_TRUE;
}

static void func_des(void *p, SCAPAR_ITEM *kp)
{
    (void)p;
    (void)kp;
    return;
}

static void func_print(void *addr, SCAPAR_ITEM *p)
{
    assert(addr != NULL);
    assert(p != NULL);

    /* if not parsed */
    if (!p->parsed) return;

    printf("user type, int, key: \"%s\", value: %d\n", p->key, *(int *)addr);
}

int main(void)
{
    char *in = "misc.dat";
    SCAPAR scapar;

    SCAPAR_IDX i, k;

    /* string */
    char *fn = NULL;

    /* scale */
    SCAPAR_INT dim = -1, verb = -1;
    SCAPAR_FLT km = -1.;

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

    /* table */
    SCAPAR_TABLE tri;

    /* table vector */
    SCAPAR_VEC mv;

    /* info */
    SCAPAR_INFO info;
    SCAPAR_UT_INFO ut;

    int user = 0;

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register scalar key words */
    scapar_register_int(&scapar, "dim", &dim);
    scapar_register_float(&scapar, "km", &km);
    scapar_register_string(&scapar, "fn", &fn);
    scapar_register_int(&scapar, "verb", &verb);

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

    /* 2. register table key words */
    scapar_info_init(&info);
    info.msize = 3;
    info.mtype = NULL;   /* float */
    scapar_register_table(&scapar, "tri", &tri, &info);

    /* 2. register vector of table key words */
    info.msize = 3;
    info.mtype = NULL;   /* float */
    info.type = SCAPAR_T_TABLE;

    scapar_register_vec(&scapar, "mv", &mv, &info);

    /* 2. register user defined type (optional) */
    ut.conv = func_conv;
    ut.destroy = func_des;
    ut.print = func_print;

    scapar_init_user_type(&scapar, SCAPAR_T_USER0, ut);
    scapar_register_user_type(&scapar, "user", &user, SCAPAR_T_USER0, NULL);

    /* print initial values */
    scapar_printf("initial values:\n");
    scapar_printf("dim: %"IFMT"\n", dim);
    scapar_printf("verb: %"IFMT"\n", verb);
    scapar_printf("km: %"FFMT"\n", km);

    if (fn != NULL) scapar_printf("fn: %s\n", fn);

    scapar_printf("user: %"IFMT"\n", user);

    /* 3. parse */
    scapar_parse(&scapar);

    /* print info from keywords file */
    scapar_printf("\nvalues read from input file:\n");
    scapar_print_by_key(&scapar, "dim");
    scapar_print_by_key(&scapar, "verb");
    scapar_print_by_key(&scapar, "km");
    if (fn != NULL) scapar_printf("fn: %s\n", fn);

    /* table */
    scapar_print_by_key(&scapar, "tri");

    /* vector of table */
    scapar_print_by_key(&scapar, "mv");

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

    /* user */
    scapar_printf("\nparsed value for user (user-defined)\n");
    scapar_print_by_key(&scapar, "user");

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
