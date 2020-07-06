
#include "scapar.h"

/* 1M */
static SCAPAR_INT buf_size_default = 1048576;

SCAPAR_INFO SCAPAR_INFO_INT_     = {SCAPAR_T_INT,  0, 0, NULL};
SCAPAR_INFO SCAPAR_INFO_FLT_     = {SCAPAR_T_FLT,  0, 0, NULL};
SCAPAR_INFO SCAPAR_INFO_STR_     = {SCAPAR_T_STR,  0, 0, NULL};

SCAPAR_INFO SCAPAR_INFO_VEC_INT_ = {SCAPAR_T_VEC_INT,  0, 0, NULL};
SCAPAR_INFO SCAPAR_INFO_VEC_FLT_ = {SCAPAR_T_VEC_FLT,  0, 0, NULL};
SCAPAR_INFO SCAPAR_INFO_VEC_STR_ = {SCAPAR_T_VEC_STR,  0, 0, NULL};

void scapar_init(SCAPAR *scapar, const char *fn)
{
    if (scapar == NULL) {
        scapar_error(1, "scapar: scapar_create, input cannot be NULL\n");
    }

    /* init */
    bzero(scapar, sizeof(*scapar));

    if ((scapar->fh = fopen(fn, "r")) == NULL) {
        scapar_error(1, "scapar: cannot open file %s.\n", fn);
    }

    /* special mark */
    scapar->magic = SCAPAR_MAGIC_NUMBER;

    /* default attr */
    scapar->cmt_mark = '#';
    scapar->se_mark = strdup("/");
    scapar->buf_size = buf_size_default;
}

void scapar_set_comment_mark(SCAPAR *scapar, char m)
{
    /* magic check */
    scapar_magic_check(scapar);

    if (isspace(m)) {
        scapar_error(1, "scapar: comment mark cannot be space.\n");
    }

    /* set */
    scapar->cmt_mark = m;
}

void scapar_set_dup_check(SCAPAR *scapar, SCAPAR_BOOL check)
{
    /* magic check */
    scapar_magic_check(scapar);

    /* set */
    scapar->dup_check = check;
}

void scapar_set_case_sensitive(SCAPAR *scapar, SCAPAR_BOOL cs)
{
    /* magic check */
    scapar_magic_check(scapar);

    /* set */
    scapar->cs_mark = cs;
}

void scapar_set_max_line_length(SCAPAR *scapar, size_t m)
{
    /* magic check */
    scapar_magic_check(scapar);

    /* set */
    scapar->buf_size = m;
}

void scapar_set_sec_ending_mark(SCAPAR *scapar, char *m)
{
    char *em;
    int i, k;

    assert(m != NULL);

    /* magic check */
    scapar_magic_check(scapar);

    /* remove leading and tailing spaces */
    em = strdup(m);
    scapar_str_remove_spaces(em);

    k = strlen(em);
    if (k == 0) {
        scapar_error(1, "scapar: illegal section ending mark: %s\n", m);
    }
    else {
        for (i = 0; i < k; i++) {
            if (isspace(em[i])) {
                scapar_error(1, "scapar: illegal section ending mark: %s\n", m);
            }
        }
    }

    /* set */
    free(scapar->se_mark);
    scapar->se_mark = em;
}

void scapar_finalize(SCAPAR *scapar)
{
    if (scapar == NULL) return;

    /* magic check */
    scapar_magic_check(scapar);

    fclose(scapar->fh);
    free(scapar->se_mark);

    scapar_sec_finalize(&scapar->s, 0, SCAPAR_TRUE);
    scapar->sorted = SCAPAR_FALSE;
}

void scapar_init_user_type(SCAPAR *scapar, SCAPAR_TYPE type, SCAPAR_UT_INFO ut)
{
    assert(scapar != NULL);
    assert(type >= SCAPAR_T_USER0);
    assert(type <= SCAPAR_T_USER31);
    assert(ut.conv != NULL);
    assert(ut.destroy != NULL);

    /* magic check */
    scapar_magic_check(scapar);

    scapar_sec_init_user_type(&scapar->s, type, ut);
}

void scapar_sort(SCAPAR *scapar)
{

    scapar_pointer_check("scapar_sort", scapar, "scapar");

    if (scapar->sorted) return;

    /* sort */
    scapar_sec_sort(&scapar->s);

    /* mark */
    scapar->sorted = SCAPAR_TRUE;
}

void scapar_register(SCAPAR *scapar, const char *key, void *addr, SCAPAR_TYPE type, void *info)
{
    /* magic check */
    scapar_magic_check(scapar);

    /* pointer check */
    scapar_pointer_check("scapar_register_keyword", scapar, "scapar");
    scapar_pointer_check("scapar_register_keyword", key, "key");
    scapar_pointer_check("scapar_register_keyword", addr, "addr");

    assert(key != NULL);
    assert(addr != NULL);

    scapar_sec_register(&scapar->s, scapar, key, addr, type, info);

    /* mark */
    scapar->sorted = SCAPAR_FALSE;

    /* if check dup */
    if (scapar->dup_check) {
        scapar_sort(scapar);
    }
}

void scapar_register_int(SCAPAR *scapar, const char *key, SCAPAR_INT *addr)
{
    scapar_register(scapar, key, addr, SCAPAR_T_INT, NULL);
}

void scapar_register_float(SCAPAR *scapar, const char *key, SCAPAR_FLT *addr)
{
    scapar_register(scapar, key, addr, SCAPAR_T_FLT, NULL);
}

void scapar_register_string(SCAPAR *scapar, const char *key, char **addr)
{
    scapar_register(scapar, key, addr, SCAPAR_T_STR, NULL);
}

void scapar_register_vec(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr, void *info)
{
    scapar_register(scapar, key, addr, SCAPAR_T_VEC, info);
}

void scapar_register_vec_int(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr)
{
    scapar_register(scapar, key, addr, SCAPAR_T_VEC_INT, NULL);
}

void scapar_register_vec_float(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr)
{
    scapar_register(scapar, key, addr, SCAPAR_T_VEC_FLT, NULL);
}

void scapar_register_vec_string(SCAPAR *scapar, const char *key, SCAPAR_VEC *addr)
{
    scapar_register(scapar, key, addr, SCAPAR_T_VEC_STR, NULL);
}

void scapar_register_table(SCAPAR *scapar, const char *key, SCAPAR_TABLE *addr, void *info)
{
    scapar_register(scapar, key, addr, SCAPAR_T_TABLE, info);
}

void scapar_register_user_type(SCAPAR *scapar, const char *key, void *addr, SCAPAR_TYPE type, void *info)
{
    scapar_register(scapar, key, addr, type, info);
}

void scapar_register_section(SCAPAR *scapar, const char *key, void *addr)
{
    scapar_register(scapar, key, addr, SCAPAR_T_SEC, NULL);
}

SCAPAR_SEC * scapar_get_section(SCAPAR *scapar, const char *key)
{
    return scapar_sec_get_section(&scapar->s, key);
}

void scapar_parse(SCAPAR *scapar)
{
    char *line;
    char *line2 = NULL;
    size_t ls;

    scapar_pointer_check("scapar_parse", scapar, "scapar");
    assert(scapar->s.size > 0);

    /* sort keys */
    scapar_sort(scapar);

    /* buffer size */
    ls = scapar->buf_size <= 512 ? 512 : scapar->buf_size;
    ls += 1;

    /* buf */
    line = scapar_malloc(sizeof(*line) * ls);

    /* parse */
    scapar_sec_parse_from_fh(&scapar->s, (void *)scapar->s.base, &line, &line2, &ls, scapar->fh, scapar);

    free(line);
    free(line2);

    /* mark */
    scapar->parsed = SCAPAR_TRUE;
}

void scapar_print_by_key(SCAPAR *scapar, const char *key)
{
    /* magic check */
    scapar_magic_check(scapar);

    scapar_pointer_check("scapar_print_by_key", scapar, "scapar");
    scapar_pointer_check("scapar_print_by_key", key, "key");

    if (scapar->s.size <= 0) return;

    /* if parsed */
    if (!scapar->parsed) {
        scapar_warning("scapar: not parsed yet, only initial value available.\n");
    }

    scapar_sec_print_by_key(&scapar->s, key);
}

void scapar_print(SCAPAR *scapar)
{
    assert(scapar != NULL);

    scapar_sec_print(&scapar->s);
}

SCAPAR_ITEM * scapar_query(SCAPAR *s, const char *key)
{
    SCAPAR_ITEM *p;
    SCAPAR_ITEM ky;

    scapar_pointer_check("scapar_query", s, "s");
    scapar_pointer_check("scapar_query", key, "key");

    /* check */
    if (!s->sorted) {
        scapar_sort(s);
    }

    ky.key = (char *)key;
    p = bsearch(&ky, s->s.item, s->s.size, sizeof(*s->s.item), scapar_item_key_comp);

    return p;
}
