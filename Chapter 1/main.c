#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct A_stm_ *A_stm;
typedef struct A_exp_ *A_exp;
typedef struct A_expList_ *A_expList;
typedef enum {A_plus,A_minus,A_times,A_div} A_binop;
typedef struct table *Table_;

struct A_stm_ {enum {A_compoundStm, A_assignStm, A_printStm} kind;
    union {struct {A_stm stm1, stm2;} compound;
        struct {string id; A_exp exp;} assign;
        struct {A_expList exps;} print;
    } u;
};
A_stm A_CompoundStm(A_stm stm1, A_stm stm2);
A_stm A_AssignStm(string id, A_exp exp);
A_stm A_PrintStm(A_expList exps);

struct A_exp_ {enum {A_idExp, A_numExp, A_opExp, A_eseqExp} kind;
    union {string id;
        int num;
        struct {A_exp left; A_binop oper; A_exp right;} op;
        struct {A_stm stm; A_exp exp;} eseq;
    } u;
};
A_exp A_IdExp(string id);
A_exp A_NumExp(int num);
A_exp A_OpExp(A_exp left, A_binop oper, A_exp right);
A_exp A_EseqExp(A_stm stm, A_exp exp);
struct A_expList_ {enum {A_pairExpList, A_lastExpList} kind;
    union {
        struct {A_exp head; A_expList tail;} pair;
        A_exp last;
    } u;
};

A_expList A_PairExpList(A_exp head, A_expList tail);
A_expList A_LastExpList(A_exp last);

struct table {string id; int value; Table_ tail;};
Table_ *Table (string id, int value, Table_ tail){
    Table_ t = malloc(sizeof(*t));
    t->id = malloc(strlen(id) + 1);
    t->id = id;
    t->value = value;
    t->tail = tail;
    return t;
};

struct IntAndTable {
    int i; Table_ t;
};


void *check_malloc(int len) {
    void *p = malloc(len);
    assert(p);
    return p;
}

A_stm A_CompoundStm(A_stm stm1, A_stm stm2) {
    A_stm s = check_malloc((sizeof(*s)));
    s->kind = A_compoundStm;
    s->u.compound.stm1 = stm1;
    s->u.compound.stm2 = stm2;
    return s;
}

A_stm A_AssignStm(string id, A_exp exp){
    A_stm s = check_malloc(sizeof(*s));
    s->kind = A_assignStm;
    s->u.assign.id = malloc(strlen(id) + 1);
    s->u.assign.id = id;
    s->u.assign.exp = exp;
    return s;
}

A_stm A_PrintStm(A_expList exps) {
    A_stm s = check_malloc(sizeof(*s));
    s->kind = A_printStm;
    s->u.print.exps = exps;
    return s;
}

A_exp A_IdExp(string id){
    A_exp e = check_malloc(sizeof(*e));
    e->kind = A_idExp;
    e->u.id = malloc(strlen(id) + 1);
    e->u.id = id;
    return e;
}

A_exp A_NumExp(int num){
    A_exp e = check_malloc(sizeof(*e));
    e->kind = A_numExp;
    e->u.num = num;
    return e;
}

A_exp A_OpExp(A_exp left, A_binop oper, A_exp right){
    A_exp e = check_malloc(sizeof(*e));
    e->kind = A_opExp;
    e->u.op.left = left;
    e->u.op.oper = oper;
    e->u.op.right = right;
    return e;
}

A_exp A_EseqExp(A_stm stm, A_exp exp){
    A_exp e = check_malloc(sizeof(*e));
    e->kind = A_eseqExp;
    e->u.eseq.stm = stm;
    e->u.eseq.exp = exp;
    return e;
}

A_expList A_PairExpList(A_exp head, A_expList tail){
    A_expList e = check_malloc(sizeof(*e));
    e->kind = A_pairExpList;
    e->u.pair.head = head;
    e->u.pair.tail = tail;
    return e;
}

A_expList A_LastExpList(A_exp last){
    A_expList e = check_malloc(sizeof(*e));
    e->kind = A_lastExpList;
    e->u.last = last;
    return e;
}

A_stm prog(void) {
    return
            A_CompoundStm(A_AssignStm("a",
                                      A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
                          A_CompoundStm(A_AssignStm("b",
                                                    A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),
                                                                                       A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
                                                                                                             A_NumExp(1))))),
                                                              A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
                                        A_PrintStm(A_LastExpList(A_IdExp("b")))));
}

int maxargs(A_stm);
int maxargs_exp(A_exp);

int maxargs_exp(A_exp exp){
    switch (exp->kind) {
        case A_idExp: return 0;
        case A_numExp: return 0;
        case A_opExp: return maxargs_exp(exp->u.op.left) + maxargs_exp(exp->u.op.right);
        case A_eseqExp: return maxargs(exp->u.eseq.stm) + maxargs_exp(exp->u.eseq.exp);
        default: return 0;
    };
}

int maxargs(A_stm stm){
    switch (stm->kind) {
        case A_printStm: return 1;
        case A_assignStm: return maxargs_exp(stm->u.assign.exp);
        case A_compoundStm: return maxargs(stm->u.compound.stm1) + maxargs(stm->u.compound.stm2);
        default: return 0;
    }
}

struct IntAndTable interpExpList(A_expList e_list, Table_ t);
Table_ interpStm(A_stm stm, Table_ t);

struct IntAndTable interpExp(A_exp e, Table_ t) {
    struct IntAndTable result;
    switch (e->kind){
        case A_idExp:
        {
            Table_ current = malloc(sizeof(*current));
            current = t;
            while (current)
            {
                if (current->id == e->u.id)
                {
                    result.i = current->value;
                    result.t = t;
                    return result;
                }
            }
            assert(0);
        }
        case A_numExp: {
            result.i = e->u.num;
            result.t = t;
            return result;
        }
        case A_opExp: {
            struct IntAndTable leftResult = interpExp(e->u.op.left, t);
            struct IntAndTable rightResult = interpExp(e->u.op.right, leftResult.t);
            result.t = rightResult.t;
            switch (e->u.op.oper) {
                case A_plus: result.i = leftResult.i + rightResult.i; break;
                case A_minus: result.i = leftResult.i - rightResult.i; break;
                case A_times: result.i = leftResult.i * rightResult.i; break;
                case A_div: result.i = leftResult.i / rightResult.i; break;
            }
            return result;
        }
        case A_eseqExp: {
            Table_ result = interpStm(e->u.eseq.stm, t);
            return interpExp(e->u.eseq.exp, result);
        }
    }
}

Table_ interpStm(A_stm stm, Table_ t) {
    switch (stm->kind) {
        case A_compoundStm: {
            Table_ returned = interpStm(stm->u.compound.stm1, t);
            return interpStm(stm->u.compound.stm2, returned);
        }
        case A_printStm: {
            A_expList list = malloc(sizeof(*list));
            list = stm->u.print.exps;
            switch (list->kind) {
                case A_lastExpList:{
                    struct IntAndTable result = interpExp(list->u.last, t);
                    printf("%d\n", result.i);
                    return result.t;
                }
                case A_pairExpList:{
                    struct IntAndTable result;
                    while (list) {
                        result = interpExp(list->u.pair.head, t);
                        printf("%d ", result.i);
                        list = list->u.pair.tail;
                    };
                    printf("\n");
                    return result.t;
                }
            }
            break;
        }
        case A_assignStm: {
            struct IntAndTable result = interpExp(stm->u.assign.exp, t);
            return Table(stm->u.assign.id, result.i, t);
        }
    }
}

int main() {
    A_stm prog_ = prog();
    printf("maxargs(prog_): %d\n", maxargs(prog_));
    interpStm(prog_, NULL);
    return 0;
}
