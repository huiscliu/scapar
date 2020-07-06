
#include "scapar-utils.h"

#if HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif

#if USE_UNIX

#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif

#endif

void * scapar_malloc(size_t size)
{
    void *ptr;

    if (size == 0) return NULL;

    ptr = malloc(size);
    if (ptr == NULL && size != 0) {
        scapar_error(1, "scapar: failed to malloc %u bytes memory.\n", size);
    }

    return ptr;
}

void * scapar_calloc(size_t nmemb, size_t size)
{
    void *ptr;

    if (nmemb == 0 || size == 0) return NULL;

    ptr = calloc(nmemb, size);
    if (ptr == NULL && nmemb != 0 && size != 0) {
        scapar_error(1, "scapar: failed to calloc %d bytes memory.\n", size * nmemb);
    }

    return ptr;
}

void * scapar_realloc(void *ptr, size_t size)
{
    void *p = realloc(ptr, size);

    if (p == NULL && size != 0) {
        scapar_error(1, "scapar: failed to reallocate %u bytes memory at %p.\n", size, ptr);
    }

    return p;
}

void scapar_free(void *ptr)
{
    if (ptr == NULL) return;

    free(ptr);
}

int scapar_warning(const char *fmt, ...)
{
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = vfprintf(stdout, fmt, ap);
    fflush(stdout);
    va_end(ap);

    return ret;
}

void scapar_error(int code, const char *fmt, ...)
{
    va_list ap;

    fprintf(stdout, "** ERROR: ");
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fflush(stdout);
    va_end(ap);

    if (code == 0) return;

    exit(code);
}

int scapar_printf(const char *fmt, ...)
{
    va_list ap;
    int ret = 0;

    va_start(ap, fmt);
    ret = vfprintf(stdout, fmt, ap);
    fflush(stdout);
    va_end(ap);

    return ret;
}

void scapar_pointer_check(const char *func, const void *key, const char *kn)
{
    if (key == NULL) {
        scapar_error(1, "scapar: %s, input \"%s\" cannot be NULL\n", func, kn);
    }
}

char * scapar_fgets(char *s, int size, FILE *stream, size_t *lno)
{
    char *p;

    assert(size >= 0);
    scapar_pointer_check("scapar_fgets", s, "s");
    scapar_pointer_check("scapar_fgets", stream, "stream");

    /* safe-guard */
    s[0] = '\0';

    p = fgets(s, size, stream);
    
    /* return line no */
    if (lno != NULL) *lno += 1;

    return p;
}

SCAPAR_FLT scapar_atof(const char *ptr)
{
    double t;
    char *end;

    scapar_pointer_check("scapar_atof", ptr, "ptr");
#if SCAPAR_USE_LONG_DOUBLE
    t = strtold(ptr, &end);
#else
    t = strtod(ptr, &end);
#endif

    if (end == ptr || (*end != '\0' && isspace(*end) == 0)) {
        scapar_error(1, "scapar: ptr: \"%s\" is not a float number.\n", ptr);
    }

    return t;
}

SCAPAR_INT scapar_atoi(const char *ptr)
{
#if SCAPAR_USE_LONG_LONG
    long long t;
#elif SCAPAR_USE_LONG
    long t;
#else
    int t;
#endif
    char *end;

    scapar_pointer_check("scapar_atoi", ptr, "ptr");

#if SCAPAR_USE_LONG_LONG
    t = strtoll(ptr, &end, 10);
#elif SCAPAR_USE_LONG
    t = strtol(ptr, &end, 10);
#else
    t = strtol(ptr, &end, 10);
#endif

    if (end == ptr || (*end != '\0' && isspace(*end) == 0)) {
        scapar_error(1, "scapar: ptr: \"%s\" is not an integer.\n", ptr);
    }

    return t;
}

void scapar_str_remove_spaces(char *str)
{
    char *p = str;
    int i, ps;

    if (str == NULL) return;

    /* leading space */
    while (isspace(*p)) {
        p++;
    }

    if (p != str) {
        ps = strlen(p);
        for (i = 0; i <= ps; i++) str[i] = p[i];
    }

    /* remove tail spaces */
    ps = strlen(str);
    for (i = ps - 1; i >= 0; i--) {
        if (isspace(str[i])) {
            str[i] = '\0';
        }
        else {
            break;
        }
    }
}

/* input is empty or comment: 1 is true and 0 is false */
int scapar_check_if_discard(const char *b, char m)
{
    const char *p;
    int ret;

    if (b == NULL) return 1;

    p = b;
    while (isspace(*p)) {
        p++;
    }

    if (*p == '\0' || *p == m) {
        ret = 1;
    }
    else {
        ret = 0;
    }

    return ret;
}

/* input is comment: 1 is true and 0 is false */
int scapar_check_input_is_comment(const char *b, char cmt)
{
    const char *p;
    int ret;

    if (b == NULL) return 0;

    p = b;
    while (isspace(*p)) {
        p++;
    }

    if (*p == cmt) {
        ret = 1;
    }
    else {
        ret = 0;
    }

    return ret;
}

/* input is section ending: 1 is true and 0 is false */
int scapar_check_input_is_section_ending(const char *b, char *cmt)
{
    const char *p;
    int ret;

    if (b == NULL) return 0;

    p = b;
    while (isspace(*p)) {
        p++;
    }

    if (strcmp(p, cmt) == 0) {
        ret = 1;
    }
    else {
        ret = 0;
    }

    return ret;
}

/* input is empty: 1 is true and 0 is false */
int scapar_check_input_is_empty(const char *b)
{
    const char *p;
    int ret;

    if (b == NULL) return 1;

    p = b;
    while (isspace(*p)) {
        p++;
    }

    if (*p == '\0') {
        ret = 1;
    }
    else {
        ret = 0;
    }

    return ret;
}

void scapar_string_to_lower(char *str)
{
    int i;
    int k;
    int la, uz;

    assert(str != NULL);

    k = strlen(str);

    la = 'A';
    uz = 'Z';
    for (i = 0; i < k; i++) {
        if (str[i] >= la && str[i] <= uz) {
            str[i] = tolower(str[i]);
        }
    }
}

int scapar_item_key_comp(const void *p, const void *n)
{
    const SCAPAR_ITEM *pp = p;
    const SCAPAR_ITEM *pn = n;

    if (strcmp(pp->key, pn->key) > 0) {
        return 1;
    }
    else if (strcmp(pp->key, pn->key) == 0) {
        return 0;
    }
    else {
        return -1;
    }
}

void scapar_magic_check(const SCAPAR *scapar)
{
    assert(scapar != NULL);
    if (scapar->magic != SCAPAR_MAGIC_NUMBER) {
        scapar_error(1, "scapar: not initialized yet.\n");
    }
}

void scapar_info_init(SCAPAR_INFO *in)
{
    if (in == NULL) return;

    bzero(in, sizeof(*in));
}

SCAPAR_INFO * scapar_info_copy(SCAPAR_INFO *in)
{
    SCAPAR_INFO *t = NULL;

    if (in == NULL) return NULL;

    t = scapar_calloc(1, sizeof(*t));

    /* init */
    memcpy(t, in, sizeof(*in));

    if (in->msize > 0 && in->mtype != NULL) {
        t->mtype = scapar_malloc(in->msize * sizeof(*in->mtype));
        memcpy(t->mtype, in->mtype, in->msize * sizeof(*in->mtype));
    }

    return t;
}

void scapar_info_destroy(SCAPAR_INFO *in)
{
    if (in == NULL) return;

    if (in->mtype != NULL) scapar_free(in->mtype);

    bzero(in, sizeof(*in));
}

#if USE_UNIX
SCAPAR_FLT scapar_get_mem(void)
{
    struct rusage RU;
    SCAPAR_FLT mem_current;

    /* getrusage() */
    getrusage(RUSAGE_SELF, &RU);
    mem_current = RU.ru_maxrss / (double)1024.;

    return mem_current;
}
#else /* not implement yet */
SCAPAR_FLT scapar_get_mem(void)
{
    return 0;
}
#endif

#if USE_UNIX
SCAPAR_FLT scapar_get_time(void)
{
    struct timeval tv;
    double t;

    gettimeofday(&tv, (struct timezone *)0);
    t = tv.tv_sec + (double)tv.tv_usec * 1e-6;

    return t;
}

#else
#include <windows.h>

SCAPAR_FLT scapar_get_time(void)
{
    LARGE_INTEGER timer;
    static LARGE_INTEGER fre;
    static int init = 0;
    double t;

    if (init != 1) {
        QueryPerformanceFrequency(&fre);
        init = 1;
    }

    QueryPerformanceCounter(&timer);

    t = timer.QuadPart * 1. / fre.QuadPart;

    return t;
}
#endif

