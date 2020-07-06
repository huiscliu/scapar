
#ifndef SCAPAR_VEC_OP_H
#define SCAPAR_VEC_OP_H

#include "scapar-utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* management */
SCAPAR_BOOL scapar_vec_initialized(const SCAPAR_VEC *vec);
void scapar_vec_init(SCAPAR_VEC *vec, SCAPAR_TYPE type, SCAPAR_INT tsize, const char *key);
void scapar_vec_destroy(SCAPAR_VEC *vec);

/* add entry */
void scapar_vec_add_entry(SCAPAR_VEC *v, void *e);

/* get info */
SCAPAR_IDX scapar_vec_get_size(SCAPAR_VEC *v);

/* get value */
SCAPAR_INT scapar_vec_int_get_value(SCAPAR_VEC *v, SCAPAR_IDX n);
SCAPAR_FLT scapar_vec_float_get_value(SCAPAR_VEC *v, SCAPAR_IDX n);
char * scapar_vec_string_get_value(SCAPAR_VEC *v, SCAPAR_IDX n);

SCAPAR_TABLE scapar_vec_table_get_value(SCAPAR_VEC *v, SCAPAR_IDX n);
SCAPAR_VEC scapar_vec_vec_get_value(SCAPAR_VEC *v, SCAPAR_IDX n);

/* get value general */
void * scapar_vec_get_value(SCAPAR_VEC *v, SCAPAR_IDX n);

/* standard print */
void scapar_vec_print(SCAPAR_VEC *v);

#ifdef __cplusplus
}
#endif

#endif
