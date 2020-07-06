
#ifndef SCAPAR_TABLE_H
#define SCAPAR_TABLE_H

#include "scapar-utils.h"
#include "scapar-vec-op.h"

#ifdef __cplusplus
extern "C" {
#endif

/* table */
SCAPAR_BOOL scapar_table_initialized(const SCAPAR_TABLE *t);
void scapar_table_create(SCAPAR_TABLE *v, const char *key, SCAPAR_IDX cols, const SCAPAR_TYPE *type);
void scapar_table_destroy(SCAPAR_TABLE *v);
int  scapar_table_add_entry(SCAPAR_TABLE *v, char *buf, SCAPAR_INT lno, char cmt);

/* get info */
SCAPAR_IDX scapar_table_get_num_cols(SCAPAR_TABLE *t);
SCAPAR_IDX scapar_table_get_num_entries(SCAPAR_TABLE *t);
char * scapar_table_get_key(SCAPAR_TABLE *t);
SCAPAR_TYPE scapar_table_get_data_type(SCAPAR_TABLE *t, SCAPAR_IDX n);
void * scapar_table_get_value(SCAPAR_TABLE *t, SCAPAR_IDX row, SCAPAR_IDX col);
void * scapar_table_get_col(const SCAPAR_TABLE *t, SCAPAR_IDX col);

/* print */
void scapar_table_print(const SCAPAR_TABLE *v);

/* internal */
void scapar_parse_table_from_fh(SCAPAR_TABLE *table, FILE *f, char cmt_mark, char *line, SCAPAR_INT ls, size_t *ln_no);

#ifdef __cplusplus
}
#endif

#endif
