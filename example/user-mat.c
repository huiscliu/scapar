
#include "scapar.h"

/*
 * Assume a matrix of size 4 has the following form:
 *
 *	a_{00}   a_{01}   0.0      a_{03}
 *	0.0      a_{11}   a_{12}    0.0
 *	a_{20}   0.0      a_{22}   a_{23}
 *	0.0      0.0      a_{32}   a_{33}
 *
 * The sample file format is as follows (0-based C format):
 *
 *	4
 *	0
 *	3
 *	5
 *	8
 *	10
 *	0
 *	1
 *	3
 *	1
 *	2
 *	0
 *	2
 *	3
 *	2
 *	3
 *	a_{00}  
 *	a_{01}
 *	a_{03}
 *	a_{11}
 *	a_{12}
 *	a_{20} 
 *	a_{22}
 *	a_{23}
 *	a_{32}
 *	a_{33}
 *
 * The first line is the size of the matrix, followed by integer arrays ia
 * of size n+1, and ja of size nnz(the total number of nonzero entries in
 * the matrix, nnz=ia[n]-ia[0]). A real array a of size nnz is stored last
 * in the file.  ia is an array of pointer to the start of each row in
 * array ja and a. Array ja stores the column indices (0-based) of entries
 * on each row. Array a stores the corresponding entry values.*/

typedef struct mat_csr_
{
    SCAPAR_INT n;

    SCAPAR_VEC Ap;
    SCAPAR_VEC Aj;
    SCAPAR_VEC Ax;

} mat_csr;

/* ignore error checking here */
static void func_conv(void *addr, FILE *f, char *p, SCAPAR_ITEM *kp)
{
    SCAPAR_INT i, k;
    char buf[512];
    mat_csr *A;

    assert(addr != NULL);
    (void)p;
    (void)kp;

    A = addr;

    /* n */
    scapar_fgets(buf, 511, f, NULL);
    A->n = scapar_atoi(buf);

    /* AP */
    scapar_vec_init(&A->Ap, SCAPAR_T_INT, -1, "Ap");

    k = A->n;
    for (i = 0; i <= k; i++) {
        SCAPAR_INT tp;

        scapar_fgets(buf, 511, f, NULL);
        tp = scapar_atoi(buf);
        scapar_vec_add_entry(&A->Ap, &tp);
    }

    /* Aj */
    scapar_vec_init(&A->Aj, SCAPAR_T_INT, -1, "Aj");

    k = scapar_vec_int_get_value(&A->Ap, A->n);
    for (i = 0; i < k; i++) {
        SCAPAR_INT tp;

        scapar_fgets(buf, 511, f, NULL);
        tp = scapar_atoi(buf);
        scapar_vec_add_entry(&A->Aj, &tp);
    }

    /* Ax */
    scapar_vec_init(&A->Ax, SCAPAR_T_FLT, -1, "Ax");
    for (i = 0; i < k; i++) {
        SCAPAR_FLT tp;

        scapar_fgets(buf, 511, f, NULL);
        tp = scapar_atof(buf);
        scapar_vec_add_entry(&A->Ax, &tp);
    }

    /* mark parsed */
    kp->parsed = SCAPAR_TRUE;
}

static void func_des(void *p, SCAPAR_ITEM *kp)
{
    mat_csr *A;
    
    assert(p != NULL);
    (void)kp;

    A = p;
    scapar_vec_destroy(&A->Ap);
    scapar_vec_destroy(&A->Aj);
    scapar_vec_destroy(&A->Ax);

    return;
}

static void func_print(void *addr, SCAPAR_ITEM *p)
{
    mat_csr *A;

    assert(addr != NULL);
    assert(p != NULL);

    /* if not parsed */
    if (!p->parsed) return;

    A = addr;
    scapar_printf("mat_car: row: %"IFMT"\n", A->n);

    scapar_printf("mat_car: Ap\n");
    scapar_vec_print(&A->Ap);

    scapar_printf("\nmat_car: Aj\n");
    scapar_vec_print(&A->Aj);

    scapar_printf("\nmat_car: Ax\n");
    scapar_vec_print(&A->Ax);
}

int main(void)
{
    char *in = "user-mat.dat";
    SCAPAR scapar;

    SCAPAR_UT_INFO ut;  /* info for user-defined types */
    mat_csr A;

    /* init */
    bzero(&A, sizeof(A));

    /* 1. init */
    scapar_init(&scapar, in);
    scapar_set_dup_check(&scapar, 1);

    /* 2. register user defined type (optional) */
    ut.conv = func_conv;
    ut.destroy = func_des;
    ut.print = func_print;

    scapar_init_user_type(&scapar, SCAPAR_T_USER0, ut);
    scapar_register_user_type(&scapar, "mat_csr", &A, SCAPAR_T_USER0, NULL);

    /* 3. parse */
    scapar_parse(&scapar);

    /* output */
    scapar_print_by_key(&scapar, "mat_csr");

    /* 4. detroy */
    scapar_finalize(&scapar);

    return 0;
}
