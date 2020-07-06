
#include "scapar.h"

int main(void)
{
    char *in = "table.dat";
    SCAPAR scapar;

    /* table */
    SCAPAR_TABLE tab;

    /* vector of table */
    SCAPAR_VEC vtab;

    /* info */
    SCAPAR_INFO info;

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register table key words */
    scapar_info_init(&info);
    info.msize = 3;
    info.mtype = NULL;   /* float */
    scapar_register_table(&scapar, "tab", &tab, &info);

    /* 2. register vector of table key words */
    info.msize = 3;
    info.mtype = NULL;   /* float */
    info.type = SCAPAR_T_TABLE;

    scapar_register_vec(&scapar, "vtab", &vtab, &info);

    /* 3. parse */
    scapar_parse(&scapar);

    /* table */
    scapar_print_by_key(&scapar, "tab");

    /* vector of table */
    scapar_print_by_key(&scapar, "vtab");

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
