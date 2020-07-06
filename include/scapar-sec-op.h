
#ifndef SCAPAR_SEC_OP_H
#define SCAPAR_SEC_OP_H

#include "scapar-utils.h"

#ifdef __cplusplus
extern "C" {
#endif

SCAPAR_BOOL scapar_sec_initialized(const SCAPAR_SEC *sec);
void scapar_sec_init(SCAPAR_SEC *s);
void scapar_sec_finalize(SCAPAR_SEC *s, void *base, SCAPAR_BOOL destroy);

/* query */
SCAPAR_ITEM * scapar_sec_query(const SCAPAR_SEC *s, const char *key);

/* sort item, internal */
void scapar_sec_sort(SCAPAR_SEC *s);

/* init user type */
void scapar_sec_init_user_type(SCAPAR_SEC *s, SCAPAR_TYPE type, SCAPAR_UT_INFO ut);

void scapar_sec_register(SCAPAR_SEC *s, SCAPAR *scapar, const char *key, void *addr,
        SCAPAR_TYPE type, SCAPAR_INFO *info);

/* register a struct member */
void scapar_sec_register_offset(SCAPAR_SEC *s, SCAPAR *scapar, const char *key, SCAPAR_ADDR offset,
        SCAPAR_TYPE type, SCAPAR_INFO *info);

SCAPAR_SEC * scapar_sec_get_section(SCAPAR_SEC *s, const char *key);

void scapar_sec_parse_from_fh(SCAPAR_SEC *s, void *base, char **line, char **line2,
        size_t *len, FILE *fh, SCAPAR *scapar);

void scapar_sec_print_by_key(const SCAPAR_SEC *s, const char *key);

void scapar_sec_print(SCAPAR_SEC *s);

#ifdef __cplusplus
}
#endif
#endif
