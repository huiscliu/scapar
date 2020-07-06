
#include "scapar-vec.h"

SCAPAR_BOOL scapar_vec_initialized(const SCAPAR_VEC *vec)
{
    if (vec == NULL) return SCAPAR_FALSE;

    if (vec->magic == SCAPAR_VEC_MAGIC_NUMBER) {
        return SCAPAR_TRUE;
    }
    else {
        return SCAPAR_FALSE;
    }
}

void scapar_vec_init(SCAPAR_VEC *vec, SCAPAR_TYPE type, SCAPAR_INT tsize, const char *key)
{
    if (vec == NULL) return;
    bzero(vec, sizeof(SCAPAR_VEC));

    /* data type */
    vec->type = type;

    assert(key != NULL);
    vec->key = strdup(key);

    /* sizeof type */
    if (type == SCAPAR_T_INT) {
        vec->tsize = sizeof(SCAPAR_INT);
    }
    else if (type == SCAPAR_T_FLT) {
        vec->tsize = sizeof(SCAPAR_FLT);
    }
    else if (type == SCAPAR_T_STR) {
        vec->tsize = sizeof(char *);
    }
    else if (type == SCAPAR_T_TABLE) {
        vec->tsize = sizeof(SCAPAR_TABLE);
    }
    else if (type == SCAPAR_T_VEC_INT || type == SCAPAR_T_VEC_FLT || type == SCAPAR_T_VEC_STR) {
        vec->tsize = sizeof(SCAPAR_VEC);
    }
    else if (type == SCAPAR_T_SEC) {
        assert(tsize > 0);

        vec->tsize = tsize;
    }
    else {
        vec->tsize = tsize;
        scapar_error(1, "scapar: data type is not supported yet.\n");
    }

    vec->magic = SCAPAR_VEC_MAGIC_NUMBER;
}

void scapar_vec_destroy(SCAPAR_VEC *vec)
{
    SCAPAR_IDX i;

    if (vec == NULL) return;

    /* not parse yet */
    if (!scapar_vec_initialized(vec)) return;

    if (vec->type == SCAPAR_T_STR) {
        for (i = 0; i < vec->size; i++) {
            free(((char **)vec->data)[i]);
        }
    }
    else if (vec->type == SCAPAR_T_TABLE) {
        SCAPAR_TABLE *p = vec->data;

        for (i = 0; i < vec->size; i++) scapar_table_destroy(p + i);
    }
    else if (vec->type == SCAPAR_T_VEC_INT || vec->type == SCAPAR_T_VEC_FLT || vec->type == SCAPAR_T_VEC_STR) {
        SCAPAR_VEC *p = vec->data;

        for (i = 0; i < vec->size; i++) scapar_vec_destroy(p + i);
    }
    else if (vec->type == SCAPAR_T_SEC) {
        void *d;

        assert(vec->tsize > 0);
        for (i = 0; i < vec->size; i++) {
            d = vec->data + i * vec->tsize;
            scapar_sec_finalize(vec->sec, d, SCAPAR_FALSE);
        }
    }

    scapar_free(vec->data);
    free(vec->key);
    bzero(vec, sizeof(SCAPAR_VEC));
}

/* add entry */
void scapar_vec_add_entry(SCAPAR_VEC *v, void *e)
{
    assert(v != NULL);
    assert(e != NULL);

    /* not init yet */
    if (!scapar_vec_initialized(v)) {
        scapar_error(1, "vector not initialized yet\n");
    }

    if (v->size >= v->alloc) {
        v->alloc += 16;

        assert(v->tsize > 0);
        v->data = scapar_realloc(v->data, v->alloc * v->tsize);
    }

    if (v->type == SCAPAR_T_INT) {
        ((SCAPAR_INT *)v->data)[v->size++] = *(SCAPAR_INT *)e;
    }
    else if (v->type == SCAPAR_T_FLT) {
        ((SCAPAR_FLT *)v->data)[v->size++] = *(SCAPAR_FLT *)e;
    }
    else if (v->type == SCAPAR_T_STR) {
        ((char **)v->data)[v->size++] = strdup(e);
    }
    else if (v->type == SCAPAR_T_TABLE) {
        ((SCAPAR_TABLE *)v->data)[v->size++] = *(SCAPAR_TABLE *)e;
    }
    else if (v->type == SCAPAR_T_VEC_INT || v->type == SCAPAR_T_VEC_FLT || v->type == SCAPAR_T_VEC_STR) {
        ((SCAPAR_VEC *)v->data)[v->size++] = *(SCAPAR_VEC *)e;
    }
    else if (v->type == SCAPAR_T_SEC) {
        memcpy(v->data + v->size * v->tsize, e, v->tsize);

        v->size += 1;
    }
    else {
        scapar_error(1, "scapar: data type is not supported yet.\n");
    }
}

/* get value */
SCAPAR_IDX scapar_vec_get_size(SCAPAR_VEC *v)
{
    assert(v != NULL);

    if (!scapar_vec_initialized(v)) {
        return 0;
    }

    return v->size;
}

SCAPAR_INT scapar_vec_int_get_value(SCAPAR_VEC *v, SCAPAR_IDX n)
{
    assert(v != NULL);

    if (!scapar_vec_initialized(v)) {
        scapar_error(1, "vector not initialized yet\n");
    }

    assert(n >= 0);
    assert(n < v->size);

    assert(v->type == SCAPAR_T_INT);
    return ((SCAPAR_INT *)v->data)[n];
}

SCAPAR_FLT scapar_vec_float_get_value(SCAPAR_VEC *v, SCAPAR_IDX n)
{
    assert(v != NULL);

    if (!scapar_vec_initialized(v)) {
        scapar_error(1, "vector not initialized yet\n");
    }

    assert(n >= 0);
    assert(n < v->size);

    assert(v->type == SCAPAR_T_FLT);
    return ((SCAPAR_FLT *)v->data)[n];
}

char * scapar_vec_string_get_value(SCAPAR_VEC *v, SCAPAR_IDX n)
{
    assert(v != NULL);

    if (!scapar_vec_initialized(v)) {
        scapar_error(1, "vector not initialized yet\n");
    }

    assert(n >= 0);
    assert(n < v->size);

    assert(v->type == SCAPAR_T_STR);
    return ((char **)v->data)[n];
}

SCAPAR_TABLE scapar_vec_table_get_value(SCAPAR_VEC *v, SCAPAR_IDX n)
{
    assert(v != NULL);

    if (!scapar_vec_initialized(v)) {
        scapar_error(1, "vector not initialized yet\n");
    }

    assert(n >= 0);
    assert(n < v->size);

    assert(v->type == SCAPAR_T_TABLE);
    return ((SCAPAR_TABLE *)v->data)[n];
}

SCAPAR_VEC scapar_vec_vec_get_value(SCAPAR_VEC *v, SCAPAR_IDX n)
{
    assert(v != NULL);

    if (!scapar_vec_initialized(v)) {
        scapar_error(1, "vector not initialized yet\n");
    }

    assert(n >= 0);
    assert(n < v->size);

    assert(v->type == SCAPAR_T_VEC_INT || v->type == SCAPAR_T_VEC_FLT || v->type == SCAPAR_T_VEC_STR);
    return ((SCAPAR_VEC *)v->data)[n];
}

/* get value general */
void * scapar_vec_get_value(SCAPAR_VEC *v, SCAPAR_IDX n)
{
    assert(v != NULL);

    if (!scapar_vec_initialized(v)) {
        scapar_error(1, "vector not initialized yet\n");
    }

    assert(n >= 0);
    assert(n < v->size);

    return v->data + n * v->tsize;
}

void scapar_vec_print(SCAPAR_VEC *v)
{
    SCAPAR_IDX i;

    /* not parse yet */
    if (!scapar_vec_initialized(v)) return;

    if (v->type == SCAPAR_T_INT) {
        SCAPAR_INT *p;

        p = v->data;
        scapar_printf("scapar: key: \"%s\", vector of int, size: %d, values:", v->key, v->size);

        for (i = 0; i < v->size; i++) {
            scapar_printf(" %"IFMT, p[i]);
        }

        scapar_printf("\n");
    }
    else if (v->type == SCAPAR_T_FLT) {
        SCAPAR_FLT *p;

        p = v->data;
        scapar_printf("scapar: key: \"%s\", vector of float, size: %d, values:", v->key, v->size);

        for (i = 0; i < v->size; i++) {
            scapar_printf(" %"FFMT, p[i]);
        }

        scapar_printf("\n");
    }
    else if (v->type == SCAPAR_T_STR) {
        char **p;

        p = v->data;
        scapar_printf("scapar: key: \"%s\", vector of string, size: %d, values:", v->key, v->size);

        for (i = 0; i < v->size; i++) {
            scapar_printf(" \"%s\"", p[i]);
        }

        scapar_printf("\n");
    }
    else if (v->type == SCAPAR_T_TABLE) {
        SCAPAR_TABLE *p;

        p = v->data;
        scapar_printf("scapar: key: \"%s\", vector of table, size: %d:\n", v->key, v->size);

        for (i = 0; i < v->size; i++) {
            scapar_table_print(p + i);
            scapar_printf("\n");
        }
    }
    else if (v->type == SCAPAR_T_VEC_INT) {
        SCAPAR_VEC *p;

        p = v->data;
        scapar_printf("scapar: key: \"%s\", vector of vector of int, size: %d:\n", v->key, v->size);

        for (i = 0; i < v->size; i++) {
            scapar_vec_print(p + i);
        }
    }
    else if (v->type == SCAPAR_T_VEC_FLT) {
        SCAPAR_VEC *p;

        p = v->data;
        scapar_printf("scapar: key: \"%s\", vector of vector of float, size: %d:\n", v->key, v->size);

        for (i = 0; i < v->size; i++) {
            scapar_vec_print(p + i);
        }
    }
    else if (v->type == SCAPAR_T_VEC_STR) {
        SCAPAR_VEC *p;

        p = v->data;
        scapar_printf("scapar: key: \"%s\", vector of vector of string, size: %d:\n", v->key, v->size);

        for (i = 0; i < v->size; i++) {
            scapar_vec_print(p + i);
        }
    }
    else if (v->type == SCAPAR_T_SEC) {
        void *d;
        SCAPAR_ADDR tp;

        for (i = 0; i < v->size; i++) {
            scapar_printf("scapar section: %"IFMT"\n", i);

            d = v->data + i * v->tsize;

            /* hack, starting memory location */
            tp = v->sec->base;
            v->sec->base = (SCAPAR_ADDR) d;
            scapar_sec_print(v->sec);
            v->sec->base = tp;

            scapar_printf("\n");
        }
    }
    else {
        scapar_error(1, "scapar: parsing module not implemented yet.\n");
    }
}
