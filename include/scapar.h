
#ifndef SCAPAR_H
#define SCAPAR_H

#include "scapar-sec.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void scapar_init(SCAPAR *scapar, const char *fn);
void scapar_finalize(SCAPAR *scapar);

void scapar_set_comment_mark(SCAPAR *scapar, char m);
void scapar_set_dup_check(SCAPAR *scapar, SCAPAR_BOOL check);
void scapar_set_case_sensitive(SCAPAR *scapar, SCAPAR_BOOL cs);
void scapar_set_max_line_length(SCAPAR *scapar, size_t m);
void scapar_set_sec_ending_mark(SCAPAR *scapar, char *m);

void scapar_sort(SCAPAR *scapar);

/* to register a keyword */
void scapar_register(SCAPAR *scapar, const char *key, void *addr, SCAPAR_TYPE type, void *info);

/* to register a user type */
void scapar_init_user_type(SCAPAR *scapar, SCAPAR_TYPE type, SCAPAR_UT_INFO ut);

/* special, scalar */
void scapar_register_int(SCAPAR *scapar, const char *key, SCAPAR_INT *addr);
void scapar_register_float(SCAPAR *scapar, const char *key, SCAPAR_FLT *addr);
void scapar_register_string(SCAPAR *scapar, const char *key, char **addr);

/* vector, general */
void scapar_register_vec(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr, void *info);

/* vector, special */
void scapar_register_vec_int(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr);
void scapar_register_vec_float(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr);
void scapar_register_vec_string(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr);

/* table, general table */
void scapar_register_table(SCAPAR *scapar, const char *key, SCAPAR_TABLE *addr, void *info);

/* user types */
void scapar_register_user_type(SCAPAR *scapar, const char *key, void *addr, SCAPAR_TYPE type, void *info);

/* section */
void scapar_register_section(SCAPAR *scapar, const char *key, void *addr);

/* parse */
void scapar_parse(SCAPAR *scapar);

/* print sparsed key words */
void scapar_print_by_key(SCAPAR *scapar, const char *key);

void scapar_print(SCAPAR *scapar);

/* get section */
SCAPAR_SEC * scapar_get_section(SCAPAR *scapar, const char *key);

SCAPAR_ITEM * scapar_query(SCAPAR *s, const char *key);

#ifdef __cplusplus
}
#endif
#endif
