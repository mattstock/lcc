%{
#define INTTMP 0x0100ff00
#define INTVAR 0x40ff0000
#define FLTTMP 0x000f0ff0
#define FLTVAR 0xfff00000

#define INTRET 0x00000004
#define FLTRET 0x00000003

#define readsreg(p) \
        (generic((p)->op)==INDIR && (p)->kids[0]->op==VREG+P)
#define setsrc(d) ((d) && (d)->x.regnode && \
        (d)->x.regnode->set == src->x.regnode->set && \
        (d)->x.regnode->mask&src->x.regnode->mask)

#define relink(a, b) ((b)->x.prev = (a), (a)->x.next = (b))

#include "c.h"
#define NODEPTR_TYPE Node
#define OP_LABEL(p) ((p)->op)
#define LEFT_CHILD(p) ((p)->kids[0])
#define RIGHT_CHILD(p) ((p)->kids[1])
#define STATE_LABEL(p) ((p)->x.state)

extern char *stabprefix;

static void address(Symbol, Symbol, long);
static void blkfetch(int, int, int, int);
static void blkloop(int, int, int, int, int, int[]);
static void blkstore(int, int, int, int);
static void defaddress(Symbol);
static void defconst(int, int, Value);
static void defstring(int, char *);
static void defsymbol(Symbol);
static int imm16(Node p);
static void doarg(Node);
static void emit2(Node);
static void export(Symbol);
static void clobber(Node);
static void function(Symbol, Symbol [], Symbol [], int);
static void global(Symbol);
static void import(Symbol);
static void local(Symbol);
static void progbeg(int, char **);
static void progend(void);
static void segment(int);
static void space(int);
static void target(Node);
static int      bitcount       (unsigned);
static Symbol   argreg         (int, int, int, int, int);

static Symbol ireg[32], freg2[32], d6;
static Symbol iregw, freg2w;
static int tmpregs[] = {3, 9, 10};
static Symbol blkreg;
static Symbol prevg;

static int gnum = 8;

static int cseg;
%}
%start stmt
%term CNSTI1=1045
%term CNSTI2=2069
%term CNSTI4=4117
%term CNSTI8=8213
%term CNSTP4=4119
%term CNSTP8=8215
%term CNSTU1=1046
%term CNSTU2=2070
%term CNSTU4=4118
%term CNSTU8=8214
%term CNSTF4=4113
%term CNSTF8=8209
 
%term ARGB=41
%term ARGI4=4133
%term ARGI8=8229
%term ARGP4=4135
%term ARGP8=8231
%term ARGU4=4134
%term ARGU8=8230
%term ARGF4=4129
%term ARGF8=8225

%term ASGNB=57
%term ASGNI1=1077
%term ASGNI2=2101
%term ASGNI4=4149
%term ASGNI8=8245
%term ASGNP4=4151
%term ASGNP8=8247
%term ASGNU1=1078
%term ASGNU2=2102
%term ASGNU4=4150
%term ASGNU8=8246
%term ASGNF4=4145
%term ASGNF8=8241

%term INDIRB=73
%term INDIRI1=1093
%term INDIRI2=2117
%term INDIRI4=4165
%term INDIRI8=8261
%term INDIRP4=4167
%term INDIRP8=8263
%term INDIRU1=1094
%term INDIRU2=2118
%term INDIRU4=4166
%term INDIRU8=8262
%term INDIRF4=4161
%term INDIRF8=8257

%term CVFF4=4209
%term CVFF8=8305
%term CVFI4=4213
%term CVFI8=8309

%term CVIF4=4225
%term CVIF8=8321
%term CVII1=1157
%term CVII2=2181
%term CVII4=4229
%term CVII8=8325
%term CVIU1=1158
%term CVIU2=2182
%term CVIU4=4230
%term CVIU8=8326

%term CVPU8=8342

%term CVUI1=1205
%term CVUI2=2229
%term CVUI4=4277
%term CVUI8=8373
%term CVUP4=4279
%term CVUP8=8375
%term CVUU1=1206
%term CVUU2=2230
%term CVUU4=4278
%term CVUU8=8374

%term NEGF4=4289
%term NEGF8=8385
%term NEGI4=4293
%term NEGI8=8389

%term CALLB=217
%term CALLI4=4309
%term CALLI8=8405
%term CALLP4=4311
%term CALLP8=8407
%term CALLU4=4310
%term CALLU8=8406
%term CALLV=216
%term CALLF4=4305
%term CALLF8=8401

%term RETI4=4341
%term RETI8=8437
%term RETP4=4343
%term RETP8=8439
%term RETU4=4342
%term RETU8=8438
%term RETV=248
%term RETF4=4337
%term RETF8=8433

%term ADDRGP4=4359
%term ADDRGP8=8455
%term ADDRFP4=4375
%term ADDRFP8=8471
%term ADDRLP4=4391
%term ADDRLP8=8487

%term ADDI4=4405
%term ADDI8=8501
%term ADDP4=4407
%term ADDP8=8503
%term ADDU4=4406
%term ADDU8=8502
%term ADDF4=4401
%term ADDF8=8497

%term SUBI4=4421
%term SUBI8=8517
%term SUBP4=4423
%term SUBP8=8519
%term SUBU4=4422
%term SUBU8=8518
%term SUBF4=4417
%term SUBF8=8513

%term LSHI4=4437
%term LSHI8=8533
%term LSHU4=4438
%term LSHU8=8534

%term MODI4=4453
%term MODI8=8549
%term MODU4=4454
%term MODU8=8550

%term RSHI4=4469
%term RSHI8=8565
%term RSHU4=4470
%term RSHU8=8566

%term BANDI4=4485
%term BANDI8=8581
%term BANDU4=4486
%term BANDU8=8582

%term BCOMI4=4501
%term BCOMI8=8597
%term BCOMU4=4502
%term BCOMU8=8598

%term BORI4=4517
%term BORI8=8613
%term BORU4=4518
%term BORU8=8614

%term BXORI4=4533
%term BXORI8=8629
%term BXORU4=4534
%term BXORU8=8630

%term DIVI4=4549
%term DIVI8=8645
%term DIVU4=4550
%term DIVU8=8646
%term DIVF4=4545
%term DIVF8=8641

%term MULI4=4565
%term MULI8=8661
%term MULU4=4566
%term MULU8=8662
%term MULF4=4561
%term MULF8=8657

%term EQI4=4581
%term EQI8=8677
%term EQU4=4582
%term EQU8=8678
%term EQF4=4577
%term EQF8=8673

%term GEI4=4597
%term GEI8=8693
%term GEU4=4598
%term GEU8=8694
%term GEF4=4593
%term GEF8=8689

%term GTI4=4613
%term GTI8=8709
%term GTU4=4614
%term GTU8=8710
%term GTF4=4609
%term GTF8=8705

%term LEI4=4629
%term LEI8=8725
%term LEU4=4630
%term LEU8=8726
%term LEF4=4625
%term LEF8=8721

%term LTI4=4645
%term LTI8=8741
%term LTU4=4646
%term LTU8=8742
%term LTF4=4641
%term LTF8=8737

%term NEI4=4661
%term NEI8=8757
%term NEU4=4662
%term NEU8=8758
%term NEF4=4657
%term NEF8=8753

%term JUMPV=584

%term LABELV=600

%term LOADB=233
%term LOADI1=1253
%term LOADI2=2277
%term LOADI4=4325
%term LOADI8=8421
%term LOADP4=4327
%term LOADP8=8423
%term LOADU1=1254
%term LOADU2=2278
%term LOADU4=4326
%term LOADU8=8422
%term LOADF4=4321
%term LOADF8=8417

%term VREGP=711
%%
reg:  INDIRI1(VREGP)     "# read register\n"
reg:  INDIRU1(VREGP)     "# read register\n"
reg:  INDIRI2(VREGP)     "# read register\n"
reg:  INDIRU2(VREGP)     "# read register\n"
reg:  INDIRI4(VREGP)     "# read register\n"
reg:  INDIRU4(VREGP)     "# read register\n"
reg:  INDIRP4(VREGP)     "# read register\n"

stmt: ASGNI1(VREGP,reg)  "# write register\n"
stmt: ASGNU1(VREGP,reg)  "# write register\n"
stmt: ASGNI2(VREGP,reg)  "# write register\n"
stmt: ASGNU2(VREGP,reg)  "# write register\n"
stmt: ASGNI4(VREGP,reg)  "# write register\n"
stmt: ASGNU4(VREGP,reg)  "# write register\n"
stmt: ASGNP4(VREGP,reg)  "# write register\n"

reg: CNSTI1  "# reg\n"  range(a, 0, 0)
reg: CNSTU1  "# reg\n"  range(a, 0, 0)
reg: CNSTU2  "# reg\n"  range(a, 0, 0)
reg: CNSTI2  "# reg\n"  range(a, 0, 0)
reg: CNSTI4  "# reg\n"  range(a, 0, 0)
reg: CNSTU4  "# reg\n"  range(a, 0, 0)
reg: CNSTP4  "# reg\n"  range(a, 0, 0)

con1: CNSTI1  "1"  range(a,1,1)
con1: CNSTI2  "1"  range(a,1,1)
con1: CNSTI4  "1"  range(a,1,1)
con1: CNSTU1  "1"  range(a,1,1)
con1: CNSTU2  "1"  range(a,1,1)
con1: CNSTU4  "1"  range(a,1,1)

con16: CNSTI1 "%a" imm16(a)
con16: CNSTI2 "%a" imm16(a)
con16: CNSTI4 "%a" imm16(a)
con16: CNSTU1 "%a" range(a, 0, 0xffff)
con16: CNSTU2 "%a" range(a, 0, 0xffff)
con16: CNSTU4 "%a" range(a, 0, 0xffff)

con32: CNSTI4 "%a" range(a, -2147483648, 2147483647)
con32: CNSTU4 "%a" range(a, 0, 0xffffffff)

reg: ADDI4(reg, con1)  "inc %0\n"
reg: ADDU4(reg, con1)  "inc %0\n"
reg: SUBI4(reg, con1)  "dec %0\n"
reg: SUBU4(reg, con1)  "dec %0\n"

reg: ADDI4(reg, con32)  "add.l %0, %1\n"    2    
reg: ADDI4(reg, con16)  "add.i %0, %1\n"    1
reg: ADDI4(reg, reg)    "add %c ,%0, %1\n"  
reg: ADDU4(reg, con32)  "add.l %0, %1\n"    2
reg: ADDU4(reg, con16)  "add.i %0, %1\n"    1
reg: ADDU4(reg, reg)    "add %c, %0, %1\n"
reg: SUBI4(reg, con32)  "sub.l %0, %1\n"    2
reg: SUBI4(reg, con16)  "sub.i %0, %1\n"    1
reg: SUBI4(reg, reg)    "sub %c, %0, %1\n"
reg: SUBU4(reg, con32)  "sub.l %0, %1\n"    2
reg: SUBU4(reg, con16)  "sub.i %0, %1\n"    1
reg: SUBU4(reg, reg)    "sub %c, %0, %1\n"
reg: ADDP4(reg, reg)    "ADDP4(reg,reg) %c,%0,%1\n"  1
reg: SUBP4(reg, reg)    "SUBP4(reg,reg) %c,%0,%1\n"  1

reg: BANDI4(reg, con32) "and.l %0, %1\n"   2
reg: BANDI4(reg, con16) "and.i %0, %1\n"   1
reg: BANDI4(reg, reg)   "and %c, %0, %1\n"
reg: BANDU4(reg, con32) "and.l %0, %1\n"   2
reg: BANDU4(reg, con16) "and.i %0, %1\n"   1
reg: BANDU4(reg, reg)   "and %c,%0, %1\n"
reg: BXORI4(reg, con32) "xor.l %0, %1\n"   2
reg: BXORI4(reg, con16) "xor.i %0, %1\n"   1
reg: BXORI4(reg,reg)    "xor %c,%0,%1\n"   
reg: BXORU4(reg, con32) "xor.l %0, %1\n"   2
reg: BXORU4(reg, con16) "xor.i %0, %1\n"   1
reg: BXORU4(reg, reg)   "xor %c, %0, %1\n"
reg: BORU4(reg, con32)  "or.l %0, %1\n"    2
reg: BORU4(reg, con16)  "or.i %0, %1\n"    1
reg: BORU4(reg, reg)    "or %c, %0, %1\n" 
reg: BORI4(reg, con32)  "or.l %0, %1\n"    2
reg: BORI4(reg, con16)  "or.i %0, %1\n"    1
reg: BORI4(reg, reg)    "or %c, %0, %1\n"
reg: LSHI4(reg, reg)    "lsl %0, %1\n" 
reg: LSHU4(reg, reg)    "lsl %0, %1\n"  
reg: RSHI4(reg,reg)     "asr %0, %1\n" 
reg: RSHU4(reg,reg)     "lsr %0, %1\n"  

reg: LOADI1(reg)  "mov %c,%0\n"  move(a)
reg: LOADI2(reg)  "mov %c,%0\n"  move(a)
reg: LOADI4(reg)  "mov %c,%0\n"  move(a)
reg: LOADU1(reg)  "mov %c,%0\n"  move(a)
reg: LOADU2(reg)  "mov %c,%0\n"  move(a)
reg: LOADU4(reg)  "mov %c,%0\n"  move(a)
reg: LOADP4(reg)  "mov %c,%0\n"  move(a)

addr16: con16     "%0"
addr: ADDRGP4     "%a"
addr: addr16      "%0"
addr: con32       "ACON32(%0)"

reg: con32             "ld.l %c, %0\n" 1
reg: con16             "ld.i %c, %0\n"
reg: INDIRI1(addr)     "ld %c, %0\n" 2
reg: INDIRI2(addr)     "ld %c, %0\n" 2
reg: INDIRI4(addr)     "ld %c, %0\n" 2
reg: INDIRU4(addr)     "ld %c, %0\n" 2
reg: addr              "ld %c, %0\n" 3

reg: CVII4(reg) "# extend\n" 2

stmt: reg  ""
stmt: ASGNI1(addr, reg) "st %1, %0\n"
stmt: ASGNI2(addr, reg) "st %1, %0\n"  
stmt: ASGNI4(addr, reg) "st %1, %0\n" 
stmt: ASGNU2(addr, reg) "st %1, %0\n"  
stmt: ASGNU4(addr, reg) "st %1, %0\n" 
stmt: ASGNP4(addr, reg) "st %1, %0\n"

stmt: NEI4(reg, reg) "cmp %0, %1\nbne %a\n"
stmt: NEU4(reg, reg) "cmp %0, %1\nbne %a\n"
stmt: EQI4(reg, reg) "cmp %0, %1\nbeq %a\n"
stmt: EQU4(reg, reg) "cmp %0, %1\nbeq %a\n"
stmt: LTI4(reg, reg) "cmp %0, %1\nblt %a\n"
stmt: LTU4(reg, reg) "cmp %0, %1\nbltu %a\n"
stmt: LEI4(reg, reg) "cmp %0, %1\nble %a\n"
stmt: LEU4(reg, reg) "cmp %0, %1\nbleu %a\n"
stmt: GEI4(reg, reg) "cmp %0, %1\nbge %a\n"
stmt: GEU4(reg, reg) "cmp %0, %1\nbgeu %a\n"
stmt: GTI4(reg, reg) "cmp %0, %1\nbgt %a\n"
stmt: GTU4(reg, reg) "cmp %0, %1\nbgtu %a\n"
stmt: LABELV  "%a:\n"
%%

/* MAYBE OK */
static void progbeg(int argc, char *argv[]) {
        int i;

        {
                union {
                        char c;
                        int i;
                } u;
                u.i = 0;
                u.c = 1;
                swap = ((int)(u.i == 1)) != IR->little_endian;
        }
        parseflags(argc, argv);
        for (i = 0; i < 31; i += 2)
                freg2[i] = mkreg("r%d", i, 3, FREG);
        for (i = 0; i < 32; i++)
                ireg[i]  = mkreg("r%d", i, 1, IREG);
        ireg[31]->x.name = "sp";
        d6 = mkreg("6", 6, 3, IREG);
        freg2w = mkwildcard(freg2);
        iregw = mkwildcard(ireg);
        tmask[IREG] = INTTMP; tmask[FREG] = FLTTMP;
        vmask[IREG] = INTVAR; vmask[FREG] = FLTVAR;
        blkreg = mkreg("8", 8, 7, IREG);
}

/* OK */
static Symbol rmap(int opk) {
        switch (optype(opk)) {
        case I: case U: case P: case B:
                return iregw;
        case F:
                return freg2w;
        default:
                return 0;
        }

	stabprefix = ".LL";
}

/* OK */
static void globalend(void) {
        if (prevg && prevg->type->size > 0)
                print(".size %s,%d\n", prevg->x.name, prevg->type->size);
        prevg = NULL;
}

/* OK */
static void progend(void) {
        globalend();
        (*IR->segment)(CODE);
        print(".ident \"LCC: 4.2\"\n");
}

/* TODO */
static void target(Node p) {
        assert(p);
        switch (specific(p->op)) {
        case CNST+I: case CNST+U: case CNST+P:
                if (range(p, 0, 0) == 0) {
                        setreg(p, ireg[0]);
                        p->x.registered = 1;
                }
                break;
        case CALL+V:
                rtarget(p, 0, ireg[25]);
                break;
        case CALL+F:
                rtarget(p, 0, ireg[25]);
                setreg(p, freg2[0]);
                break;
        case CALL+I: case CALL+P: case CALL+U:
                rtarget(p, 0, ireg[25]);
                setreg(p, ireg[2]);
                break;
        case RET+F:
                rtarget(p, 0, freg2[0]);
                break;
        case RET+I: case RET+U: case RET+P:
                rtarget(p, 0, ireg[2]);
                break;
        case ARG+F: case ARG+I: case ARG+P: case ARG+U: {
                static int ty0;
                int ty = optype(p->op);
                Symbol q;

                q = argreg(p->x.argno, p->syms[2]->u.c.v.i, ty, opsize(p->op), ty0);
                if (p->x.argno == 0)
                        ty0 = ty;
                if (q &&
                !(ty == F && q->x.regnode->set == IREG))
                        rtarget(p, 0, q);
                break;
                }
        case ASGN+B: rtarget(p->kids[1], 0, blkreg); break;
        case ARG+B:  rtarget(p->kids[0], 0, blkreg); break;
        }
}

/* TODO */
static void clobber(Node p) {
        assert(p);
        switch (specific(p->op)) {
        case CALL+F:
                spill(INTTMP | INTRET, IREG, p);
                spill(FLTTMP,          FREG, p);
                break;
        case CALL+I: case CALL+P: case CALL+U:
                spill(INTTMP,          IREG, p);
                spill(FLTTMP | FLTRET, FREG, p);
                break;
        case CALL+V:
                spill(INTTMP | INTRET, IREG, p);
                spill(FLTTMP | FLTRET, FREG, p);
                break;
        }
}

/* TODO */
static void emit2(Node p) {
        int dst, n, src, sz, ty;
        static int ty0;
        Symbol q;

        switch (specific(p->op)) {
        case ARG+F: case ARG+I: case ARG+P: case ARG+U:
                ty = optype(p->op);
                sz = opsize(p->op);
                if (p->x.argno == 0)
                        ty0 = ty;
                q = argreg(p->x.argno, p->syms[2]->u.c.v.i, ty, sz, ty0);
                src = getregnum(p->x.kids[0]);
                if (q == NULL && ty == F && sz == 4)
                        print("s.s $f%d,%d($sp)\n", src, p->syms[2]->u.c.v.i);
                else if (q == NULL && ty == F)
                        print("s.d $f%d,%d($sp)\n", src, p->syms[2]->u.c.v.i);
                else if (q == NULL)
                        print("sw $%d,%d($sp)\n", src, p->syms[2]->u.c.v.i);
                else if (ty == F && sz == 4 && q->x.regnode->set == IREG)
                        print("mfc1 $%d,$f%d\n", q->x.regnode->number, src);
                else if (ty == F && q->x.regnode->set == IREG)
                        print("mfc1.d $%d,$f%d\n", q->x.regnode->number, src);
                break;
        case ASGN+B:
                dalign = salign = p->syms[1]->u.c.v.i;
                blkcopy(getregnum(p->x.kids[0]), 0,
                        getregnum(p->x.kids[1]), 0,
                        p->syms[0]->u.c.v.i, tmpregs);
                break;
        case ARG+B:
                dalign = 4;
                salign = p->syms[1]->u.c.v.i;
                blkcopy(29, p->syms[2]->u.c.v.i,
                        getregnum(p->x.kids[0]), 0,
                        p->syms[0]->u.c.v.i, tmpregs);
                n   = p->syms[2]->u.c.v.i + p->syms[0]->u.c.v.i;
                dst = p->syms[2]->u.c.v.i;
                for ( ; dst <= 12 && dst < n; dst += 4)
                        print("lw $%d,%d($sp)\n", (dst/4)+4, dst);
                break;
        }
}

/* TODO */
static Symbol argreg(int argno, int offset, int ty, int sz, int ty0) {
        assert((offset&3) == 0);
        if (offset > 12)
                return NULL;
        else if (argno == 0 && ty == F)
                return freg2[12];
        else if (argno == 1 && ty == F && ty0 == F)
                return freg2[14];
        else if (argno == 1 && ty == F && sz == 8)
                return d6;  /* Pair! */
        else
                return ireg[(offset/4) + 4];
}

/* OK */
static int imm16(Node p) {
        return range(p, -32766, 32767);
}

/* TODO */
static void doarg(Node p) {
        assert(p && p->syms[0]);
        mkactual(4, p->syms[0]->u.c.v.i);
}

/* TODO */
static void local(Symbol p) {
        if (isfloat(p->type))
                p->sclass = AUTO;
        if (askregvar(p, rmap(ttob(p->type))) == 0) {
                assert(p->sclass == AUTO);
                offset = roundup(offset + p->type->size,
                            p->type->align < 4 ? 4 : p->type->align);
                p->x.offset = -offset;
                
                p->x.name = stringd(-offset);
        }
}

/* TODO */
static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls) {
        int i, saved, sizefsave, sizeisave, varargs;
        Symbol r, argregs[4];

	globalend();
	print(".align 2\n");
	print(".type %s,@function\n", f->x.name);
	print("%s:\n", f->x.name);

        usedmask[0] = usedmask[1] = 0;
        freemask[0] = freemask[1] = ~(unsigned)0;
        offset = maxoffset = 0;
        for (i = 0; callee[i]; i++) {
                Symbol p = callee[i];
                Symbol q = caller[i];
                assert(q);
                offset = roundup(offset, q->type->align);
                p->x.offset = q->x.offset = offset;
                p->x.name = q->x.name = stringd(offset);
                offset += roundup(q->type->size, 4);
		p->sclass = q->sclass = AUTO;
        }
        assert(caller[i] == 0);
        offset = maxoffset = 0;
        gencode(caller, callee);
        framesize = roundup(maxoffset, 4);
        if (framesize > 0) {
	  print("allocate %d on stack for frame\n", framesize);
        }
        emitcode();
        if (framesize > 0)
                print("deallocate %d from stack?\n", framesize);
        print("rts\n");
        { int l = genlabel(1);
          print(".Lf%d:\n", l);
          print(".size %s,.Lf%d-%s\n", f->x.name, l, f->x.name);
        }
}

/* OK */
static void defconst(int suffix, int size, Value v) {
        if (suffix == I && size == 1)
                print(".byte %d\n", (int)v.u);
        else if (suffix == I && size == 2)
                print(".word %d\n", (int)v.i);
        else if (suffix == I && size == 4)
                print(".long %d\n", (int)v.i);
        else if (suffix == U && size == 1)
                print(".byte %d\n", (int)((char)v.u));
        else if (suffix == U && size == 2)
                print(".word %d\n", (int)v.u);
        else if (suffix == U && size == 4)
                print(".long %d\n", (int)v.u);
        else if (suffix == P && size == 4)
                print(".long %d\n", (int)v.p);
        else if (suffix == F && size == 4) {
                float f = v.d;
                print(".long %d\n", (int)(*(unsigned *)&f));
        } else if (suffix == F && size == 8) {
                double d = v.d;
                unsigned *p = (unsigned *)&d;
                print(".long %d\n.long %d\n", (int)p[swap], (int)p[!swap]);
        }
        else assert(0);
}

/* OK */
static void defaddress(Symbol p) {
       print(".long %s\n", p->x.name);
}

/* OK, may need to check null termination */
static void defstring(int n, char *str) {
       assert(str[n] == '\0');
       print(".string \"%s\"\n", str);
}

/* OK */
static void export(Symbol p) {
        print(".globl %s\n", p->x.name);
}

/* OK */
static void import(Symbol p) {}

/* OK */
static void defsymbol(Symbol p) {
        if (p->scope >= LOCAL && p->sclass == STATIC)
                p->x.name = stringf("%s.%d", p->name, genlabel(1));
        else if (p->generated)
                p->x.name = stringf(".LC%s", p->name);
        else if (p->scope == GLOBAL || p->sclass == EXTERN)
                p->x.name = stringf("%s", p->name);
        else
                p->x.name = p->name;
}

/* OK */
static void segment(int n) {
        if (n == cseg)
                return;
        cseg = n;
        if (cseg == CODE)
                print(".text\n");
        else if (cseg == BSS)
                print(".section bss\n");
        else if (cseg == DATA || cseg == LIT)
                print(".data\n");
}

/* OK */
static void address(Symbol q, Symbol p, long n) {
        if (p->scope == GLOBAL
        || p->sclass == STATIC || p->sclass == EXTERN)
                q->x.name = stringf("%s%s%D", p->x.name,
                        n >= 0 ? "+" : "", n);
        else {
                assert(n <= INT_MAX && n >= INT_MIN);
                q->x.offset = p->x.offset + n;
                q->x.name = stringd(q->x.offset);
        }
}

/* TODO */
static void global(Symbol p) {
        globalend();
        print(".align %d\n", p->type->align > 4 ? 4 : p->type->align);
        if (!p->generated) {
                print(".type %s,@%s\n", p->x.name,
                        isfunc(p->type) ? "function" : "object");
                if (p->type->size > 0)
                        print(".size %s,%d\n", p->x.name, p->type->size);
                else
                        prevg = p;
        }
        if (p->u.seg == BSS) {
                if (p->sclass == STATIC)
                        print(".lcomm %s,%d\n", p->x.name, p->type->size);
                else
                        print(".comm %s,%d\n", p->x.name, p->type->size);
        } else {
                print("%s:\n", p->x.name);
        }
}

/* OK */
static void space(int n) {
        if (cseg != BSS)
                print(".space %d\n", n);
}

/* TODO */
static void blkloop(int dreg, int doff, int sreg, int soff, int size, int tmps[]) {}
static void blkfetch(int size, int off, int reg, int tmp) {
        assert(size == 1 || size == 2 || size == 4);
}
static void blkstore(int size, int off, int reg, int tmp) {
}

static void stabinit(char *, int, char *[]);
static void stabline(Coordinate *);
static void stabsym(Symbol);

static char *currentfile;

static int bitcount(unsigned mask) {
        unsigned i, n = 0;

        for (i = 1; i; i <<= 1)
                if (mask&i)
                        n++;
        return n;
}

/* stabinit - initialize stab output */
static void stabinit(char *file, int argc, char *argv[]) {
        if (file) {
                print(".file 2,\"%s\"\n", file);
                currentfile = file;
        }
}

/* stabline - emit stab entry for source coordinate *cp */
static void stabline(Coordinate *cp) {
        if (cp->file && cp->file != currentfile) {
                print(".file 2,\"%s\"\n", cp->file);
                currentfile = cp->file;
        }
        print(".loc 2,%d\n", cp->y);
}

/* stabsym - output a stab entry for symbol p */
static void stabsym(Symbol p) {
        if (p == cfunc && IR->stabline)
                (*IR->stabline)(&p->src);
}
Interface bexkat1IR = {
        1, 1, 0,  /* char */
        2, 2, 0,  /* short */
        4, 4, 0,  /* int */
        4, 4, 0,  /* long */
        4, 4, 0,  /* long long */
        4, 4, 1,  /* float */
        4, 4, 1,  /* double */
        4, 4, 1,  /* long double */
        4, 4, 0,  /* T * */
        0, 1, 0,  /* struct */
        0,      /* little_endian */
        0,  /* mulops_calls */
        0,  /* wants_callb */
        1,  /* wants_argb */
        1,  /* left_to_right */
        0,  /* wants_dag */
        0,  /* unsigned_char */
        address,
        blockbeg,
        blockend,
        defaddress,
        defconst,
        defstring,
        defsymbol,
        emit,
        export,
        function,
        gen,
        global,
        import,
        local,
        progbeg,
        progend,
        segment,
        space,
        0, 0, 0, stabinit, stabline, stabsym, 0,
        {
                2,      /* max_unaligned_load */
                rmap,
                blkfetch, blkstore, blkloop,
                _label,
                _rule,
                _nts,
                _kids,
                _string,
                _templates,
                _isinstruction,
                _ntname,
                emit2,
                doarg,
                target,
                clobber,

        }
};
static char rcsid[] = "$Id$";
