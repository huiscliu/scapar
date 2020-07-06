
#ifndef SCAPAR_TYPES_H
#define SCAPAR_TYPES_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "scapar-config.h"

/* basic type */
#if SCAPAR_USE_LONG_DOUBLE
typedef long double       SCAPAR_FLT;
#else
typedef double            SCAPAR_FLT;
#endif

#if SCAPAR_USE_LONG_LONG
typedef long long int     SCAPAR_INT;
#elif SCAPAR_USE_LONG
typedef long int          SCAPAR_INT;
#else
typedef int               SCAPAR_INT;
#endif

/* internal, for index */
#if SCAPAR_USE_LONG_LONG
typedef long long int     SCAPAR_IDX;
#else
typedef long int          SCAPAR_IDX;
#endif

typedef int               SCAPAR_BOOL;

#if HAVE_STDINT_H
#include <stdint.h>
typedef intptr_t          SCAPAR_ADDR;
#else
typedef long int          SCAPAR_ADDR;
#endif

#undef  SCAPAR_TRUE
#define SCAPAR_TRUE                    (1)
#undef  SCAPAR_FALSE
#define SCAPAR_FALSE                   (0)

#define SCAPAR_MAGIC_NUMBER            (0x2619EFD)
#define SCAPAR_TABLE_MAGIC_NUMBER      (0x2619EFB)
#define SCAPAR_VEC_MAGIC_NUMBER        (0x2619EFE)
#define SCAPAR_SEC_MAGIC_NUMBER        (0x2619EEC)

typedef enum SCAPAR_TYPE_
{
    SCAPAR_T_UNDEF = 0,       /* undefine */

    /* basic */
    SCAPAR_T_STR,             /* string */
    SCAPAR_T_INT,             /* int */
    SCAPAR_T_FLT,             /* float */

    /* a table */
    SCAPAR_T_TABLE,           /* table */

    /* vector */
    SCAPAR_T_VEC,             /* vector, general */
    SCAPAR_T_VEC_INT,         /* int vector */
    SCAPAR_T_VEC_FLT,         /* float vector */
    SCAPAR_T_VEC_STR,         /* string vector */

    SCAPAR_T_SEC,             /* section, may contain all types above */

    /* user defined */
    SCAPAR_T_USER0,           /* user defined type 0 */
    SCAPAR_T_USER1,           /* user defined type 1 */
    SCAPAR_T_USER2,           /* user defined type 2 */
    SCAPAR_T_USER3,           /* user defined type 3 */
    SCAPAR_T_USER4,           /* user defined type 4 */
    SCAPAR_T_USER5,           /* user defined type 5 */
    SCAPAR_T_USER6,           /* user defined type 6 */
    SCAPAR_T_USER7,           /* user defined type 7 */
    SCAPAR_T_USER8,           /* user defined type 8 */
    SCAPAR_T_USER9,           /* user defined type 9 */
    SCAPAR_T_USER10,          /* user defined type 10 */
    SCAPAR_T_USER11,          /* user defined type 11 */
    SCAPAR_T_USER12,          /* user defined type 12 */
    SCAPAR_T_USER13,          /* user defined type 13 */
    SCAPAR_T_USER14,          /* user defined type 14 */
    SCAPAR_T_USER15,          /* user defined type 15 */
    SCAPAR_T_USER16,
    SCAPAR_T_USER17,
    SCAPAR_T_USER18,
    SCAPAR_T_USER19,
    SCAPAR_T_USER20,
    SCAPAR_T_USER21,
    SCAPAR_T_USER22,
    SCAPAR_T_USER23,
    SCAPAR_T_USER24,
    SCAPAR_T_USER25,
    SCAPAR_T_USER26,
    SCAPAR_T_USER27,
    SCAPAR_T_USER28,
    SCAPAR_T_USER29,
    SCAPAR_T_USER30,
    SCAPAR_T_USER31,

} SCAPAR_TYPE;

typedef struct SCAPAR_INFO_
{
    SCAPAR_TYPE type;
    SCAPAR_INT  tsize;

    SCAPAR_INT  msize;
    SCAPAR_TYPE *mtype;

} SCAPAR_INFO;

struct SCAPAR_SEC_;

typedef struct SCAPAR_VEC_
{
    void *data;
    char *key;
    struct SCAPAR_SEC_ *sec;

    SCAPAR_TYPE type;
    SCAPAR_INT  tsize;
    SCAPAR_INFO info;

    SCAPAR_IDX size;
    SCAPAR_IDX alloc;
    SCAPAR_INT magic;

} SCAPAR_VEC;

/* table, ended by "empty line", in the end of table */
typedef struct SCAPAR_TABLE_
{
    SCAPAR_TYPE *type;
    SCAPAR_VEC  *data;
    char *key;

    SCAPAR_IDX num_cols;
    SCAPAR_IDX num_entries;
    SCAPAR_IDX alloc;
    SCAPAR_INT magic;

} SCAPAR_TABLE;

typedef struct SCAPAR_ITEM_
{
    SCAPAR_TYPE type;
    SCAPAR_BOOL parsed;

    char *key;
    SCAPAR_ADDR offset;
    SCAPAR_INFO *info;

    /* section */
    struct SCAPAR_SEC_ *sec;

} SCAPAR_ITEM;

typedef void (*SCAPAR_CONV)(void *, FILE *, char *, SCAPAR_ITEM *);
typedef void (*SCAPAR_DESTROY)(void *, SCAPAR_ITEM *);
typedef void (*SCAPAR_PRINT)(void *, SCAPAR_ITEM *);

typedef struct SCAPAR_UT_INFO_
{
    SCAPAR_CONV     conv;
    SCAPAR_DESTROY  destroy;
    SCAPAR_PRINT    print;

} SCAPAR_UT_INFO;

typedef struct SCAPAR_SEC_
{
    /* user defined function */
    SCAPAR_CONV conv[32];         /* convert user input to what user wants */
    SCAPAR_DESTROY destroy[32];   /* cleanup function */
    SCAPAR_PRINT print[32];       /* print function, optional */

    /* internal data */
    SCAPAR_ITEM *item;

    /* internal members */
    SCAPAR_IDX alloc;
    SCAPAR_IDX size;
    SCAPAR_ADDR base;
    SCAPAR_INT magic;

    SCAPAR_BOOL sorted;

} SCAPAR_SEC;

typedef struct SCAPAR_
{
    /* default section */
    SCAPAR_SEC s;

    /* internal members */
    SCAPAR_INT magic;

    FILE *fh;
    size_t ln_no;               /* line id */

    char cmt_mark;              /* comment mark, default: # */
    char *se_mark;              /* section ending mark, default: *, */
    SCAPAR_BOOL cs_mark;        /* if case sensitive, default false */
    size_t buf_size;            /* default buffer size */

    SCAPAR_BOOL sorted;
    SCAPAR_BOOL parsed;
    SCAPAR_BOOL dup_check;

} SCAPAR;

extern SCAPAR_INFO SCAPAR_INFO_INT_;
extern SCAPAR_INFO SCAPAR_INFO_FLT_;
extern SCAPAR_INFO SCAPAR_INFO_STR_;
extern SCAPAR_INFO SCAPAR_INFO_VEC_INT_;
extern SCAPAR_INFO SCAPAR_INFO_VEC_FLT_;
extern SCAPAR_INFO SCAPAR_INFO_VEC_STR_;

#define SCAPAR_INFO_INT (&SCAPAR_INFO_INT_)
#define SCAPAR_INFO_FLT (&SCAPAR_INFO_FLT_)
#define SCAPAR_INFO_STR (&SCAPAR_INFO_STR_)

#define SCAPAR_INFO_VEC_INT  (&SCAPAR_INFO_VEC_INT_)
#define SCAPAR_INFO_VEC_FLT  (&SCAPAR_INFO_VEC_FLT_)
#define SCAPAR_INFO_VEC_STR  (&SCAPAR_INFO_VEC_STR_)

#endif
