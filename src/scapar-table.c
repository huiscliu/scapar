
#include "scapar-table.h"

SCAPAR_BOOL scapar_table_initialized(const SCAPAR_TABLE *t)
{
    if (t == NULL) return SCAPAR_FALSE;

    if (t->magic == SCAPAR_TABLE_MAGIC_NUMBER) {
        return SCAPAR_TRUE;
    }
    else {
        return SCAPAR_FALSE;
    }
}

void scapar_table_create(SCAPAR_TABLE *v, const char *key, SCAPAR_IDX cols, const SCAPAR_TYPE *type)
{
    SCAPAR_IDX i;

    assert(cols > 0);
    assert(key != NULL);
    assert(v != NULL);

    if (v->magic == SCAPAR_TABLE_MAGIC_NUMBER) {
        scapar_table_destroy(v);
    }

    bzero(v, sizeof(*v));
    v->data = scapar_calloc(1, sizeof(*v->data) * cols);
    v->num_entries = 0;
    v->alloc = 32;
    v->num_cols = cols;
    v->key = strdup(key);
    v->magic = SCAPAR_TABLE_MAGIC_NUMBER;

    if (type != NULL) {
        v->type = scapar_malloc(cols * sizeof(SCAPAR_TYPE));;

        for (i = 0; i < cols; i++) {
            v->type[i] = type[i];

            assert(type[i] == SCAPAR_T_STR || type[i] == SCAPAR_T_INT || type[i] == SCAPAR_T_FLT);
        }
    }
    else { /* default, all floats */
        v->type = scapar_malloc(cols * sizeof(SCAPAR_TYPE));;

        for (i = 0; i < cols; i++) {
            v->type[i] = SCAPAR_T_FLT;
        }
    }

    /* init internal vectors */
    for (i = 0; i < cols; i++) {
        scapar_vec_init(v->data + i, v->type[i], -1, key);
    }
}

void scapar_table_destroy(SCAPAR_TABLE *v)
{
    SCAPAR_IDX i;

    if (v == NULL || v->num_cols <= 0) return;

    if (!scapar_table_initialized(v)) return;

    for (i = 0; i < v->num_cols; i++) {
        scapar_vec_destroy(v->data + i);
    }

    scapar_free(v->type);
    scapar_free(v->data);
    free(v->key);
}

/* if continue, 1 not end, 0 end */
int scapar_table_add_entry(SCAPAR_TABLE *v, char *buf, SCAPAR_INT lno, char cmt)
{
    int i, ps, rtn = 1;
    char *p, *bak;

    /* if discard */
    if (scapar_check_input_is_comment(buf, cmt)) return 1;

    /* if quit, empty line means table ends */
    if (scapar_check_input_is_empty(buf)) return 0;

    if (!scapar_table_initialized(v)) {
        scapar_error(1, "table not initialized yet\n");
    }

    ps = strlen(buf);
    if (ps == 1) {
        if (buf[ps - 1] == '\n' || buf[ps - 1] == '\r') buf[ps - 1] = '\0';
    }
    else if (ps >= 2) {
        if (buf[ps - 1] == '\n' || buf[ps - 1] == '\r') buf[ps - 1] = '\0';

        if (buf[ps - 2] == '\n' || buf[ps - 2] == '\r') buf[ps - 2] = '\0';
    }

    /* if continue */
    ps = strlen(buf);
    if (buf[ps - 1] == '/') {
        buf[ps - 1] = ' ';
        rtn = 0;
    }

    if (v->num_cols < 1 || v->num_entries < 0) {
        scapar_error(1, "scapar: table format is wrong\n");
        return 0;
    }

    bak = strdup(buf);
    p = strtok(buf, " \t");

    /* add cols */
    for (i = 0; i < v->num_cols; i++) {
        if (p == NULL) {
            scapar_error(1, "scapar: table \"%s\" missing data, line: %"IFMT", \"%s\"\n", v->key, lno, bak);
        }

        if (v->type[i] == SCAPAR_T_FLT) {
            SCAPAR_FLT t = scapar_atof(p);

            scapar_vec_add_entry(v->data + i, &t);
        }
        else if (v->type[i] == SCAPAR_T_INT) {
            SCAPAR_INT t = scapar_atoi(p);

            scapar_vec_add_entry(v->data + i, &t);
        }
        else if (v->type[i] == SCAPAR_T_STR) {
            scapar_vec_add_entry(v->data + i, p);
        }

        p = strtok(NULL, " \t");
    }

    /* increase */
    v->num_entries++;

    free(bak);

    /* if continue */
    return rtn;
}

SCAPAR_IDX scapar_table_get_num_cols(SCAPAR_TABLE *t)
{
    assert(t != NULL);

    if (!scapar_table_initialized(t)) {
        scapar_error(1, "table not initialized yet\n");
    }

    return t->num_cols;
}

SCAPAR_IDX scapar_table_get_num_entries(SCAPAR_TABLE *t)
{
    assert(t != NULL);

    if (!scapar_table_initialized(t)) {
        return 0;
    }

    return t->num_entries;
}

char * scapar_table_get_key(SCAPAR_TABLE *t)
{
    assert(t != NULL);

    if (!scapar_table_initialized(t)) {
        scapar_error(1, "table not initialized yet\n");
    }

    return t->key;
}

SCAPAR_TYPE scapar_table_get_data_type(SCAPAR_TABLE *t, SCAPAR_IDX n)
{
    assert(t != NULL);

    if (!scapar_table_initialized(t)) {
        scapar_error(1, "table not initialized yet\n");
    }

    assert(n >= 0);
    assert(n < t->num_cols);

    if (t->type == NULL) {
        return SCAPAR_T_FLT;
    }
    else {
        return t->type[n];
    }
}

void * scapar_table_get_value(SCAPAR_TABLE *v, SCAPAR_IDX row, SCAPAR_IDX col)
{
    assert(v != NULL);

    if (!scapar_table_initialized(v)) {
        scapar_error(1, "table not initialized yet\n");
    }

    assert(row >= 0);
    assert(row < v->num_entries);
    assert(col >= 0);
    assert(col < v->num_cols);

    return scapar_vec_get_value(v->data + col, row);
}

void * scapar_table_get_col(const SCAPAR_TABLE *t, SCAPAR_IDX col)
{
    assert(t != NULL);

    if (!scapar_table_initialized(t)) {
        scapar_error(1, "table not initialized yet\n");
    }

    assert(col >= 0);
    assert(col < t->num_cols);

    return t->data + col;
}

void scapar_table_print(const SCAPAR_TABLE *v)
{
    SCAPAR_IDX i, j;
    SCAPAR_IDX cols;

    if (v == NULL) return;

    if (!scapar_table_initialized(v)) {
        return;
    }

    cols = v->num_cols;
    scapar_printf("scapar: table: \"%s\", cols: %d entries: %d\n", v->key, v->num_cols, v->num_entries);
    for (j = 0; j < v->num_entries; j++) {
        for (i = 0; i < cols; i++) {
            SCAPAR_VEC *c = scapar_table_get_col(v, i);
            void *vl = scapar_vec_get_value(c, j);

            if (v->type[i] == SCAPAR_T_FLT) { /* default FLOAT */
                scapar_printf("%"FFMT" ", *((SCAPAR_FLT *)vl));
            }
            else if (v->type[i] == SCAPAR_T_INT) {
                scapar_printf("%"IFMT" ", *(SCAPAR_INT *)vl);
            }
            else if (v->type[i] == SCAPAR_T_STR) {
                if (vl != NULL) scapar_printf("%s ", vl);
            }
            else {
                scapar_error(1, "scapar: table type is wrong\n");
            }
        }

        scapar_printf("\n");
    }
}

void scapar_parse_table_from_fh(SCAPAR_TABLE *table, FILE *f, char cmt_mark, char *line, SCAPAR_INT ls, size_t *ln_no)
{
    char *p, *ln;
    SCAPAR_INT rtn;

    while (1) {
        if ((ln = scapar_fgets(line, ls, f, ln_no)) != NULL) {
            scapar_str_remove_spaces(line);
            p = line;

            rtn = scapar_table_add_entry(table, p, *ln_no, cmt_mark);
            if (rtn) {
                continue;
            }
            else {
                break;
            }
        }
        else {
            break;
        }
    }
}
