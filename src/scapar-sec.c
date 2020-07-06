
#include "scapar-sec.h"

SCAPAR_BOOL scapar_sec_initialized(const SCAPAR_SEC *sec)
{
    if (sec == NULL) return SCAPAR_FALSE;

    if (sec->magic == SCAPAR_SEC_MAGIC_NUMBER) {
        return SCAPAR_TRUE;
    }
    else {
        return SCAPAR_FALSE;
    }
}

void scapar_sec_init(SCAPAR_SEC *s)
{
    if (s == NULL) return;

    bzero(s, sizeof(*s));

    s->magic = SCAPAR_SEC_MAGIC_NUMBER;
}

void scapar_sec_finalize(SCAPAR_SEC *s, void *base, SCAPAR_BOOL destroy)
{
    SCAPAR_IDX i;
    SCAPAR_TYPE type;

    if (s == NULL) return;

    if (s->size > 0) {
        for (i = 0; i < s->size; i++) {
            SCAPAR_ITEM *kp;
            void *addr;

            kp = s->item + i;
            if (kp->info != NULL && destroy) {
                scapar_info_destroy(kp->info);
                scapar_free(kp->info);
            }

            /* key */
            if (destroy) free(kp->key);

            /* real addr */
            addr = base + kp->offset;

            /* type */
            type = kp->type;

            /* check parsed */
            if (type == SCAPAR_T_VEC) {
                SCAPAR_VEC *pv = addr;

                scapar_vec_destroy(pv);

                /* special case, section */
                if (kp->sec != NULL) {
                    scapar_sec_finalize(kp->sec, addr, destroy);
                    scapar_free(kp->sec);
                    kp->sec = NULL;
                }
            }
            else if (type == SCAPAR_T_SEC) {
                scapar_sec_finalize(kp->sec, addr, destroy);

                if (destroy) scapar_free(kp->sec);
            }
            else if (type == SCAPAR_T_STR) {
                char **cp = addr;

                /* if not parsed */
                if (!kp->parsed) continue;

                scapar_free(*cp);
            }
            else if (type == SCAPAR_T_TABLE) {
                scapar_table_destroy(addr);
            }
            else if (type >= SCAPAR_T_USER0 && type <= SCAPAR_T_USER31) {
                int k = type - SCAPAR_T_USER0;

                assert(s->destroy[k] != NULL);
                s->destroy[k](addr, kp);
            }
            else if (type == SCAPAR_T_INT || type == SCAPAR_T_FLT) {
                continue;
            }
            else {
                scapar_error(1, "scapar: type not supported yet.\n");
            }
        }
    }

    if (s->alloc > 0 && destroy) {
        scapar_free(s->item);
        s->size = 0;
        s->alloc = 0;
        s->item = NULL;
    }
}

SCAPAR_ITEM * scapar_sec_query(const SCAPAR_SEC *s, const char *key)
{
    SCAPAR_ITEM *p;
    SCAPAR_ITEM ky;

    scapar_pointer_check("scapar_sec_query", s, "s");
    scapar_pointer_check("scapar_sec_query", key, "key");
    assert(s->size > 0);

    /* check */
    if (!s->sorted) {
        scapar_error(1, "scapar: not sorted yet.\n");
    }

    ky.key = (char *)key;
    p = bsearch(&ky, s->item, s->size, sizeof(*s->item), scapar_item_key_comp);

    return p;
}

void scapar_sec_sort(SCAPAR_SEC *s)
{
    SCAPAR_IDX i;

    /* special cases */
    if (s == NULL) return;

    if (s->sorted) return;

    if (s->size <= 1) {
        s->sorted = SCAPAR_TRUE;
        return;
    }

    /* qsort */
    qsort(s->item, s->size, sizeof(*s->item), scapar_item_key_comp);

    /* check section */
    for (i = 0; i < s->size; i++) {
        if (s->item[i].type == SCAPAR_T_SEC) {
            assert(s->item[i].sec != NULL);

            scapar_sec_sort(s->item[i].sec);
        }
    }

    /* mark */
    s->sorted = SCAPAR_TRUE;
}

static void scapar_sec_adjust_space(SCAPAR_SEC *s)
{
    if (s == NULL) {
        return;
    }

    if (s->size >= s->alloc) {
        s->alloc += 16;
        s->item = scapar_realloc(s->item, sizeof(*s->item) * s->alloc);

        /* initialize, all zero */
        bzero(s->item + s->size, sizeof(*s->item) * (s->alloc - s->size));
    }
}

void scapar_sec_init_user_type(SCAPAR_SEC *s, SCAPAR_TYPE type, SCAPAR_UT_INFO ut)
{
    int k;

    assert(s != NULL);

    assert(type >= SCAPAR_T_USER0);
    assert(type <= SCAPAR_T_USER31);
    assert(ut.conv != NULL);
    assert(ut.destroy != NULL);

    k = type - SCAPAR_T_USER0;

    s->conv[k] = ut.conv;
    s->destroy[k] = ut.destroy;
    s->print[k] = ut.print;
}

SCAPAR_SEC * scapar_sec_get_section(SCAPAR_SEC *s, const char *key)
{
    SCAPAR_ITEM *p;
    SCAPAR_ITEM ky;

    scapar_pointer_check("scapar_sec_get_section", s, "s");
    scapar_pointer_check("scapar_sec_get_section", key, "key");

    assert(s != NULL);
    assert(s->size > 0);

    /* check */
    if (!s->sorted) {
        scapar_sec_sort(s);
    }

    ky.key = (char *)key;
    p = bsearch(&ky, s->item, s->size, sizeof(*s->item), scapar_item_key_comp);

    if (p->type == SCAPAR_T_SEC) {
        assert(p->sec != NULL);

        return p->sec;
    }
    else {
        return NULL;
    }
}

void scapar_sec_register(SCAPAR_SEC *s, SCAPAR *scapar, const char *key, void *addr, SCAPAR_TYPE type, SCAPAR_INFO *info)
{
    SCAPAR_IDX i;
    SCAPAR_ITEM *kp;
    char *ky;
    void *r;

    /* magic check */
    scapar_magic_check(scapar);

    /* pointer check */
    scapar_pointer_check("scapar_register_keyword", scapar, "scapar");
    scapar_pointer_check("scapar_register_keyword", key, "key");

    assert(key != NULL);
    ky = strdup(key);

    if (!scapar->cs_mark) {
        scapar_string_to_lower(ky);
    }

    /* addr */
    r = (void *)(s->base + ((SCAPAR_ADDR) addr));

    /* query */
    if (scapar->dup_check && s->size > 0) {
        kp = scapar_sec_query(s, ky);

        if (kp != NULL) {
            scapar_error(1, "scapar: keyword: \"%s\" has been registered.\n", ky);
            return;
        }
    }

    /* increase section space, increase mem */
    scapar_sec_adjust_space(s);

    /* special case */
    if (type == SCAPAR_T_VEC_INT) {
        type = SCAPAR_T_VEC;
        info = SCAPAR_INFO_INT;
    }
    else if (type == SCAPAR_T_VEC_FLT) {
        type = SCAPAR_T_VEC;
        info = SCAPAR_INFO_FLT;
    }
    else if (type == SCAPAR_T_VEC_STR) {
        type = SCAPAR_T_VEC;
        info = SCAPAR_INFO_STR;
    }

    /* init */
    i = s->size;
    s->item[i].key = strdup(ky);

    /* fix illegal key, in case, remove spaces */
    scapar_str_remove_spaces(s->item[i].key);

    s->item[i].offset = (SCAPAR_ADDR) addr;
    s->item[i].type = type;
    s->item[i].parsed = SCAPAR_FALSE;

    /* save info */
    s->item[i].info = scapar_info_copy(info);

    /* special case */
    if (type == SCAPAR_T_SEC) {
        s->item[i].sec = scapar_calloc(1, sizeof(*s->item[i].sec));

        s->item[i].sec->base = (SCAPAR_ADDR) r;
    }

    /* save section vector info */
    if (type == SCAPAR_T_VEC) {
        assert(info != NULL);

        if (info->type == SCAPAR_T_SEC) {
            s->item[i].sec = scapar_calloc(1, sizeof(*s->item[i].sec));

            /* use offset only */
            s->item[i].sec->base = 0;
        }
    }

    s->size += 1;
    s->sorted = SCAPAR_FALSE;

    /* sorting for checking dup */
    if (scapar->dup_check) {
        scapar_sec_sort(s);
    }

    free(ky);
}

void scapar_sec_register_offset(SCAPAR_SEC *s, SCAPAR *scapar, const char *key, SCAPAR_ADDR offset,
        SCAPAR_TYPE type, SCAPAR_INFO *info)
{
    scapar_sec_register(s, scapar, key, (void *)offset, type, info);
}

static SCAPAR_INT scapar_get_next_value_from_buf(const char *b, char *v, char **ret)
{
    SCAPAR_INT d;
    const char *p;

    assert(b != NULL && v != NULL && ret != NULL);

    p = b;
    while (isblank(*p)) {
        p++;
    }

    if (*p == '\0') {
        *ret = (char *)p;
        return 0;
    }

    d = 0;
    while (isblank(*p) == 0 && *p != '\0') {
        v[d++] = *(p++);
    }

    v[d] = '\0';

    *ret = (char *)p;
    return d;
}

static void scapar_sec_restore_parse_mark(SCAPAR_SEC *s)
{
    SCAPAR_IDX i;

    if (s == NULL) return;

    if (s->size > 0) {
        for (i = 0; i < s->size; i++) {
            s->item[i].parsed = SCAPAR_FALSE;
        }
    }
}

void scapar_sec_parse_from_fh(SCAPAR_SEC *s, void *base, char **line, char **line2, size_t *len,
        FILE *fh, SCAPAR *scapar)
{
    char *ln, *p;
    char str[512];
    char sn[512];
    SCAPAR_ITEM *kp;
    SCAPAR_TYPE type;
    void *r;

    assert(len != NULL);
    assert(*len > 0);

    assert(line2 != NULL);
    assert(line != NULL);
    assert(*line != NULL);

    if (!s->sorted) {
        scapar_sec_sort(s);
    }

    while ((ln = scapar_fgets(*line, *len, fh, &scapar->ln_no)) != NULL) {
        SCAPAR_INT ps = -1;

        while (1) {
            int end = 1;

            /* remove \n and \r */
            if (ps < 0) ps = strlen(*line);

            /* empty line */
            if (ps <= 0) break;

            if (ps == 1) {
                if ((*line)[ps - 1] == '\n' || (*line)[ps - 1] == '\r') {
                    (*line)[ps - 1] = '\0';
                    ps--;
                }
            }
            else if (ps >= 2) {
                if ((*line)[ps - 1] == '\n' || (*line)[ps - 1] == '\r') {
                    (*line)[ps - 1] = '\0';
                    ps--;
                }

                if ((*line)[ps - 2] == '\n' || (*line)[ps - 2] == '\r') {
                    (*line)[ps - 2] = '\0';
                    ps--;
                }
            }

            /* \ means continue */
            if (ps >= 1 && (*line)[ps - 1] == '\\') {
                (*line)[ps - 1] = ' ';
                end = 0;
            }

            if (end) {
                break;
            }
            else {
                if (*line2 == NULL) *line2 = scapar_malloc(scapar->buf_size);

                if ((ln = scapar_fgets(*line2, scapar->buf_size, fh, &scapar->ln_no)) != NULL) {
                    /* if not comment or empty line */
                    if (scapar_check_input_is_comment(*line2, scapar->cmt_mark)) {
                        size_t ct = ps + 2;

                        /* check space */
                        if (ct >= *len) {
                            *len += scapar->buf_size;
                            *line = scapar_realloc(*line, *len);
                        }

                        strcat(*line, "\\");
                        ps += 1;
                    }
                    else if (scapar_check_input_is_empty(*line2)) {
                        break;
                    }
                    else {
                        SCAPAR_INT k = strlen(*line2);
                        size_t ct = ps + 1 + k;

                        if (ct >= *len) {
                            *len += scapar->buf_size;
                            *line = scapar_realloc(*line, *len);
                        }

                        strcat(*line, *line2);
                        ps += k;
                    }
                }
            }
        }

        /* get first non-space char */
        scapar_str_remove_spaces(*line);

        /* check after read complete line */
        if (scapar_check_if_discard(*line, scapar->cmt_mark)) continue;

        /* if end section */
        if (scapar_check_input_is_section_ending(*line, scapar->se_mark)) return;

        p = *line;

        /* get key word */
        ps = 0;
        while (1) {
            if (*p == ':' || isspace(*p) || *p == '\0' || *p == '=') {
                p++;
                str[ps] = '\0';
                break;
            }

            str[ps++] = *(p++);
        }

        /* query */
        if (!scapar->cs_mark) {
            scapar_string_to_lower(str);
        }

        kp = scapar_sec_query(s, str);

        if (kp == NULL) {
            scapar_error(1, "scapar: un-recognized keyword: \"%s\", line: %lu.\n", str, scapar->ln_no);
            continue;
        }

        /* skip sep and spaces */
        while (isspace(*p) || *p == ':') {
            p++;
        }

        /* addr */
        r = (void *)(base + kp->offset);

        type = kp->type;
        if (type == SCAPAR_T_INT) {
            if (kp->parsed) {
                scapar_error(1, "scapar: duplicate keyword: \"%s\", line: %lu.\n", str, scapar->ln_no);
            }

            kp->parsed = SCAPAR_TRUE;

            *(SCAPAR_INT *)r = scapar_atoi(p);
        }
        else if (type == SCAPAR_T_FLT) {
            if (kp->parsed) {
                scapar_error(1, "scapar: duplicate keyword: \"%s\", line: %lu.\n", str, scapar->ln_no);
            }

            kp->parsed = SCAPAR_TRUE;

            *(SCAPAR_FLT *)r = scapar_atof(p);
        }
        else if (type == SCAPAR_T_STR) {
            if (kp->parsed) {
                scapar_error(1, "scapar: duplicate keyword: \"%s\", line: %lu.\n", str, scapar->ln_no);
            }

            kp->parsed = SCAPAR_TRUE;

            *(char **)r = strdup(p);
        }
        else if (type == SCAPAR_T_VEC) {
            SCAPAR_VEC *v = r;
            SCAPAR_INFO *in = kp->info;

            /* assemble */
            assert(v != NULL);
            assert(in != NULL);

            if (kp->parsed && (v->type == SCAPAR_T_INT || v->type ==  SCAPAR_T_FLT || v->type == SCAPAR_T_STR)) {
                scapar_error(1, "scapar: duplicate keyword: \"%s\", line: %lu.\n", str, &scapar->ln_no);
            }

            /* mark */
            if (!kp->parsed) {
                kp->parsed = SCAPAR_TRUE;
            }

            if (!scapar_vec_initialized(v)) {
                /* init */
                v->type = in->type;

                scapar_vec_init(v, v->type, in->tsize, kp->key);
            }

            /* parse */
            if (v->type == SCAPAR_T_INT) {
                while ((ps = scapar_get_next_value_from_buf(p, str, &p)) > 0) {
                    SCAPAR_INT tp = scapar_atoi(str);

                    scapar_vec_add_entry(v, &tp);
                }
            }
            else if (v->type == SCAPAR_T_FLT) {
                while ((ps = scapar_get_next_value_from_buf(p, str, &p)) > 0) {
                    SCAPAR_FLT tp = scapar_atof(str);

                    scapar_vec_add_entry(v, &tp);
                }
            }
            else if (v->type == SCAPAR_T_STR) {
                while ((ps = scapar_get_next_value_from_buf(p, str, &p)) > 0) {
                    scapar_vec_add_entry(v, str);
                }
            }
            else if (v->type == SCAPAR_T_TABLE) {
                SCAPAR_TABLE *table = scapar_calloc(1, sizeof(*table));

                sprintf(sn, "%s_%"XFMT, kp->key, v->size + 1);

                /* create table */
                assert(kp->info != NULL);
                scapar_table_create(table, sn, kp->info->msize, kp->info->mtype);
                scapar_parse_table_from_fh(table, fh, scapar->cmt_mark, *line, *len, &scapar->ln_no);

                scapar_vec_add_entry(v, table);
                scapar_free(table);
            }
            else if (v->type == SCAPAR_T_VEC_INT) {
                SCAPAR_VEC *vec = scapar_calloc(1, sizeof(*vec));

                sprintf(sn, "%s_%"XFMT, kp->key, v->size + 1);

                /* init a vector */
                scapar_vec_init(vec, SCAPAR_T_INT, -1, sn);

                while ((ps = scapar_get_next_value_from_buf(p, str, &p)) > 0) {
                    SCAPAR_INT tp = scapar_atoi(str);

                    scapar_vec_add_entry(vec, &tp);
                }

                /* add to v */
                scapar_vec_add_entry(v, vec);
                scapar_free(vec);
            }
            else if (v->type == SCAPAR_T_VEC_FLT) {
                SCAPAR_VEC *vec = scapar_calloc(1, sizeof(*vec));

                sprintf(sn, "%s_%"XFMT, kp->key, v->size + 1);

                /* init a vector */
                scapar_vec_init(vec, SCAPAR_T_FLT, -1, sn);

                while ((ps = scapar_get_next_value_from_buf(p, str, &p)) > 0) {
                    SCAPAR_FLT tp = scapar_atof(str);

                    scapar_vec_add_entry(vec, &tp);
                }

                /* add to v */
                scapar_vec_add_entry(v, vec);
                scapar_free(vec);
            }
            else if (v->type == SCAPAR_T_VEC_STR) {
                SCAPAR_VEC *vec = scapar_calloc(1, sizeof(*vec));

                sprintf(sn, "%s_%"XFMT, kp->key, v->size + 1);

                /* init a vector */
                scapar_vec_init(vec, SCAPAR_T_STR, -1, sn);

                while ((ps = scapar_get_next_value_from_buf(p, str, &p)) > 0) {
                    scapar_vec_add_entry(vec, str);
                }

                /* add to v */
                scapar_vec_add_entry(v, vec);
                scapar_free(vec);
            }
            else if (v->type == SCAPAR_T_SEC) {
                void *d;

                assert(kp->info != NULL);
                assert(kp->info->tsize > 0);
                v->sec = kp->sec;

                /* d is a section */
                d = scapar_calloc(1, kp->info->tsize);

                /* restore parsed mark */
                scapar_sec_restore_parse_mark(kp->sec);

                /* parse */
                scapar_sec_parse_from_fh(kp->sec, d, line, line2, len, fh, scapar);

                scapar_vec_add_entry(v, d);
                scapar_free(d);
            }
            else {
                scapar_error(1, "scapar: parsing type not implemented yet.\n");
            }
        }
        else if (type == SCAPAR_T_TABLE) {
            SCAPAR_TABLE *table = (SCAPAR_TABLE *)r;
            SCAPAR_INFO in;

            if (kp->parsed) {
                scapar_error(1, "scapar: duplicate keyword: \"%s\", line: %lu.\n", str, scapar->ln_no);
            }

            /* mark parsed */
            kp->parsed = SCAPAR_TRUE;

            /* create table */
            assert(kp->info != NULL);
            in = *(SCAPAR_INFO *)kp->info;
            scapar_table_create(r, kp->key, in.msize, in.mtype);

            /* parse */
            scapar_parse_table_from_fh(table, fh, scapar->cmt_mark, *line, *len, &scapar->ln_no);
        }
        else if (type == SCAPAR_T_SEC) {
            assert(kp->sec != NULL);
            scapar_sec_parse_from_fh(kp->sec, r, line, line2, len, fh, scapar);

            /* mark parsed */
            kp->parsed = SCAPAR_TRUE;
        }
        else if (type >= SCAPAR_T_USER0 && type <= SCAPAR_T_USER31) {
            int k = type - SCAPAR_T_USER0;

            assert(s->conv[k] != NULL);
            s->conv[k](r, fh, p, kp);

            /* mark parsed */
            kp->parsed = SCAPAR_TRUE;
        }
        else {
            scapar_error(1, "scapar: keyword type not implemented yet.\n");
            break;
        }
    }
}

void scapar_sec_print_by_key(const SCAPAR_SEC *s, const char *key)
{
    SCAPAR_ITEM *kp;
    void *r;
    SCAPAR_TYPE type;

    if (s == NULL) return;

    if (s->size <= 0) return;

    /* get key word */
    kp = scapar_sec_query(s, key);

    if (kp == NULL) {
        scapar_warning("scapar: scapar_print_by_key, key \"%s\" has not been registered, ignored.\n", key);
        return;
    }

    if (!kp->parsed) {
        scapar_warning("scapar: scapar_print_by_key, key \"%s\" has not been parsed, ignored.\n", key);
        return;
    }

    /* addr */
    r  =  (void *)(s->base + kp->offset);

    type = kp->type;
    if (type == SCAPAR_T_INT) {
        SCAPAR_INT *p = r;

        scapar_printf("scapar: key: \"%s\", value: %"IFMT"\n", key, *p);
    }
    else if (type == SCAPAR_T_FLT) {
        SCAPAR_FLT *p = r;

        scapar_printf("scapar: key: \"%s\", value: %"FFMT"\n", key, *p);
    }
    else if (type == SCAPAR_T_STR) {
        char **p = r;

        scapar_printf("scapar: key: \"%s\", value: %s\n", key, *p);
    }
    else if (type == SCAPAR_T_VEC) {
        scapar_vec_print(r);
    }
    else if (type == SCAPAR_T_SEC) {
        scapar_sec_print(r);
    }
    else if (type == SCAPAR_T_TABLE) {
        SCAPAR_TABLE *p = r;

        scapar_printf("scapar: key: \"%s\", table\n", key);
        scapar_table_print(p);
        scapar_printf("\n");
    }
    else if (type >= SCAPAR_T_USER0 && type <= SCAPAR_T_USER31) {
        int k = type - SCAPAR_T_USER0;

        if (s->print[k] != NULL) {
            s->print[k](r, kp);
        }
    }
    else {
        scapar_error(1, "scapar: type not supported yet.\n");
    }
}

void scapar_sec_print(SCAPAR_SEC *s)
{
    SCAPAR_IDX i;

    assert(s != NULL);

    for (i = 0; i < s->size; i++) {
        SCAPAR_ITEM *kp;

        kp = s->item + i;
        scapar_sec_print_by_key(s, kp->key);
    }
}
