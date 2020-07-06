
#include "scapar.h"

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

} SEC_STT;


int main(void)
{
    char *in = "large.dat";    /* 75 million lines of data to be parsed */
    SCAPAR scapar;
    SCAPAR_VEC v;
    SCAPAR_SEC *sec;
    SCAPAR_ITEM *item;
    SCAPAR_INFO info;
    SCAPAR_INT k;
    double tm;

    /* unzip */
    scapar_printf("unzip file: large.zip\n");
    system("unzip -u large.zip");
    scapar_printf("parsing started: the file has 75 million lines of data: ");

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register section */
    scapar_info_init(&info);
    info.type = SCAPAR_T_SEC;
    info.tsize = sizeof(SEC_STT);

    scapar_register_vec(&scapar, "sec-stt", &v, &info);

    /* get section and handle member */
    item = scapar_query(&scapar, "sec-stt");
    assert(item != NULL);

    sec = item->sec;
    assert(sec != NULL);

    /* register section info */
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

    /* 3. parse */
    tm = scapar_get_time();
    scapar_parse(&scapar);
    tm = scapar_get_time() - tm;

    scapar_printf("\n");

    /* info */
    k = scapar_vec_get_size(&v);
    if (k > 0) {
        scapar_printf("parsed %"IFMT" sections, 75,000,000 lines of data,  in %g seconds\n", k, tm);
    }

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
