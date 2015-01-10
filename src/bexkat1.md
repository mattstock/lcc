%{
#define INTTMP 0x3fff0000
#define INTVAR 0x00007fff
#define FLTTMP 0xffff0000
#define FLTVAR 0x0000ffff
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
static int bitcount(unsigned);
static int regop(Node, int);
static int sametree(Node, Node);

#define hasargs(p) (p->syms[0] && p->syms[0]->u.c.v.i > 0 ? 0 : LBURG_MAX)

static Symbol ireg[32], freg2[32];
static Symbol iregw, freg2w;
static Symbol blkreg;
static Symbol prevg;

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

con1: CNSTI1  "1"  range(a,1,1)
con1: CNSTI2  "1"  range(a,1,1)
con1: CNSTI4  "1"  range(a,1,1)
con1: CNSTU1  "1"  range(a,1,1)
con1: CNSTU2  "1"  range(a,1,1)
con1: CNSTU4  "1"  range(a,1,1)
con1: CNSTP4  "1"  range(a,1,1)

con16: CNSTI1 "%a"
con16: CNSTI2 "%a" imm16(a)
con16: CNSTI4 "%a" imm16(a)
con16: conu16 "%0"

conu16: CNSTU1 "%a"
conu16: CNSTU2 "%a" range(a,0,65534)
conu16: CNSTU4 "%a" range(a,0,65534)

con: CNSTI1 "%a"
con: CNSTI2 "%a"
con: CNSTI4 "%a"
con: CNSTU1 "%a"
con: CNSTU2 "%a"
con: CNSTU4 "%a"
con: CNSTP4 "%a"

reg: ADDI4(reg, con16)  "add %c, %0, %1\n" 5
reg: ADDI4(reg, reg)    "add %c, %0, %1\n" 4
reg: ADDU4(reg, con16)  "add %c, %0, %1\n" 5
reg: ADDU4(reg, reg)    "add %c, %0, %1\n" 4
reg: SUBI4(reg, con16)  "sub %c, %0, %1\n" 5
reg: SUBI4(reg, reg)    "sub %c, %0, %1\n" 4
reg: SUBU4(reg, con16)  "sub %c, %0, %1\n" 5
reg: SUBU4(reg, reg)    "sub %c, %0, %1\n" 4
reg: ADDP4(reg, reg)    "add %c, %0, %1\n" 4
reg: SUBP4(reg, reg)    "sub %c, %0, %1\n" 4

reg: MULI4(reg, reg)    "mul %c, %0, %1\n" 4
reg: MULI4(reg, con16)  "mul %c, %0, %1\n" 4
reg: MULU4(reg, reg)    "mulu %c, %0, %1\n" 4
reg: MULU4(reg, con16)  "mulu %c, %0, %1\n" 4
reg: DIVI4(reg, reg)    "div %c, %0, %1\n" 4
reg: DIVI4(reg, con16)  "div %c, %0, %1\n" 4
reg: DIVU4(reg, reg)    "divu %c, %0, %1\n" 4
reg: DIVU4(reg, con16)  "divu %c, %0, %1\n" 4
reg: MODI4(reg, reg)    "mod %c, %0, %1\n" 4
reg: MODI4(reg, con16)  "mod %c, %0, %1\n" 4
reg: MODU4(reg, reg)    "modu %c, %0, %1\n" 4
reg: MODU4(reg, con16)  "modu %c, %0, %1\n" 4

reg: BCOMU4(reg) "com %c, %0\n" 3
reg: BCOMI4(reg) "com %c, %0\n" 3
reg: NEGI4(reg)  "neg %c, %0\n" 3

reg: BANDI4(reg, con16) "and %c, %0, %1\n" 5
reg: BANDI4(reg, reg)   "and %c, %0, %1\n" 4
reg: BANDU4(reg, con16) "and %c, %0, %1\n" 5
reg: BANDU4(reg, reg)   "and %c, %0, %1\n" 4
reg: BXORI4(reg, con16) "xor %c, %0, %1\n" 5
reg: BXORI4(reg, reg)   "xor %c, %0, %1\n" 4 
reg: BXORU4(reg, con16) "xor %c, %0, %1\n" 5
reg: BXORU4(reg, reg)   "xor %c, %0, %1\n" 4
reg: BORI4(reg, con16)  "or %c %0, %1\n"   5
reg: BORI4(reg, reg)    "or %c, %0, %1\n"  4
reg: BORU4(reg, con16)  "or %c, %0, %1\n"  5
reg: BORU4(reg, reg)    "or %c, %0, %1\n"  4
reg: LSHI4(reg, con16)  "lsl %c, %0, %1\n" 5
reg: LSHI4(reg, reg)    "lsl %c, %0, %1\n" 4
reg: LSHU4(reg, con16)  "lsl %c, %0, %1\n" 5
reg: LSHU4(reg, reg)    "lsl %c, %0, %1\n" 4 
reg: RSHI4(reg, con16)  "asr %c, %0, %1\n" 5
reg: RSHI4(reg, reg)    "asr %c, %0, %1\n" 4
reg: RSHU4(reg, con16)  "lsr %c, %0, %1\n" 5
reg: RSHU4(reg, reg)    "lsr %c, %0, %1\n" 4 

reg: LOADI1(reg)  "mov %c,%0\n"  move(a)
reg: LOADI2(reg)  "mov %c,%0\n"  move(a)
reg: LOADI4(reg)  "mov %c,%0\n"  move(a)
reg: LOADU1(reg)  "mov %c,%0\n"  move(a)
reg: LOADU2(reg)  "mov %c,%0\n"  move(a)
reg: LOADU4(reg)  "mov %c,%0\n"  move(a)
reg: LOADP4(reg)  "mov %c,%0\n"  move(a)

acon: ADDRGP4          "%a"
addr: acon             "%0"
addr: reg              "0(%0)"
addr: ADDI4(reg, acon) "%1(%0)"
addr: ADDU4(reg, acon) "%1(%0)"
addr: ADDP4(reg, acon) "%1(%0)"
addr: ADDRFP4          "%a(%%30)"
addr: ADDRLP4          "%a(%%30)"

reg: con               "ldi %c, %0\n"  5
reg: con16             "lds %c, %0\n"  4
reg: conu16            "ldu %c, %0\n"  4
reg: INDIRI1(addr)     "ld.b %c, %0\n" 4
reg: INDIRU1(addr)     "ld.b %c, %0\n" 4
reg: INDIRI2(addr)     "ld %c, %0\n"   4
reg: INDIRU2(addr)     "ld %c, %0\n"   4
reg: INDIRI4(addr)     "ld.l %c, %0\n" 4
reg: INDIRU4(addr)     "ld.l %c, %0\n" 4
reg: INDIRP4(addr)     "ld.l %c, %0\n" 4 
reg: addr              "ld.l %c, %0\n" 4
reg: ADDRGP4	       "ldi %c, %a\n"  3

reg: CVII4(reg) "# extend\n" 2
reg: CVUI4(reg) "# nop\n" 2

stmt: reg  ""
stmt: ASGNI1(addr, reg) "st.b %1, %0\n" 1
stmt: ASGNU1(addr, reg) "st.b %1, %0\n" 1
stmt: ASGNI2(addr, reg) "st %1, %0\n"   1
stmt: ASGNU2(addr, reg) "st %1, %0\n"   1
stmt: ASGNI4(addr, reg) "st.l %1, %0\n" 1
stmt: ASGNU4(addr, reg) "st.l %1, %0\n" 1
stmt: ASGNP4(addr, reg) "st.l %1, %0\n" 1

stmt: ASGNI4(VREGP, ADDI4(reg, con16)) "add %0, %1\n" regop(a,3)
stmt: ASGNU4(VREGP, ADDU4(reg, con16)) "add %0, %1\n" regop(a,3)
stmt: ASGNI4(VREGP, SUBI4(reg, con16)) "sub %0, %1\n" regop(a,3)
stmt: ASGNU4(VREGP, SUBU4(reg, con16)) "sub %0, %1\n" regop(a,3)
stmt: ASGNI4(VREGP, MULI4(reg, con16)) "mul %0, %1\n" regop(a,3)
stmt: ASGNU4(VREGP, MULU4(reg, con16)) "mulu %0, %1\n" regop(a,3)

stmt: ASGNI4(VREGP, ADDI4(reg, con1)) "inc %0\n" regop(a,0)
stmt: ASGNI4(VREGP, ADDU4(reg, con1)) "inc %0\n" regop(a,0)
stmt: ASGNU4(VREGP, ADDU4(reg, con1)) "inc %0\n" regop(a,0)
stmt: ASGNP4(VREGP, ADDP4(reg, con1)) "inc %0\n" regop(a,0)
stmt: ASGNI4(VREGP, SUBI4(reg, con1)) "dec %0\n" regop(a,0)
stmt: ASGNI4(VREGP, SUBU4(reg, con1)) "dec %0\n" regop(a,0)
stmt: ASGNU4(VREGP, SUBU4(reg, con1)) "dec %0\n" regop(a,0)
stmt: ASGNP4(VREGP, SUBP4(reg, con1)) "dec %0\n" regop(a,0)

stmt: ARGU4(reg) "push %0\n"
stmt: ARGI4(reg) "push %0\n"
stmt: ARGP4(reg) "push %0\n"
stmt: ARGF4(reg) "push %0\n"

reg: CALLI4(addr) "jsr %0\nadd %%31, %a\n" hasargs(a)
reg: CALLI4(addr) "jsr %0\n"                 1
reg: CALLU4(addr) "jsr %0\nadd %%31, %a\n" hasargs(a)
reg: CALLU4(addr) "jsr %0\n"                 1
reg: CALLP4(addr) "jsr %0\nadd %%31, %a\n" hasargs(a)
reg: CALLP4(addr) "jsr %0\n"                 1

stmt: CALLV(addr) "jsr %0\nadd %%31, %a\n" hasargs(a)
stmt: CALLV(addr) "jsr %0\n"                 1

stmt: RETI4(reg)  "# ret\n"
stmt: RETU4(reg)  "# ret\n"
stmt: RETF4(reg)  "# ret\n"
stmt: RETP4(reg)  "# ret\n"

stmt: JUMPV(addr) "jmp %0\n"  2

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
        for (i = 0; i < 32; i++)
                freg2[i] = mkreg("%%%d", i, 1, FREG);
        for (i = 0; i < 32; i++)
                ireg[i]  = mkreg("%%%d", i, 1, IREG);
        freg2w = mkwildcard(freg2);
        iregw = mkwildcard(ireg);
        tmask[IREG] = INTTMP; tmask[FREG] = FLTTMP;
        vmask[IREG] = INTVAR; vmask[FREG] = FLTVAR;
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

static void globalend(void) {
        if (prevg && prevg->type->size > 0)
                print(".size %s,%d\n", prevg->x.name, prevg->type->size);
        prevg = NULL;
}

static void progend(void) {
        globalend();
        (*IR->segment)(CODE);
        print(".ident \"LCC: 4.2\"\n");
}

/* TODO */
static void target(Node p) {
  assert(p);
  switch (specific(p->op)) {
    case CALL+I:
    case CALL+U:
    case CALL+P:
    case CALL+V:
      setreg(p, ireg[15]);
      break;
    case RET+I:
    case RET+U:
    case RET+P:
      rtarget(p, 0, ireg[15]);
      break;
  }
}

/* TODO */
static void clobber(Node p) {
        assert(p);
        switch (specific(p->op)) {
        case CALL+F:
                spill(INTTMP, IREG, p);
                spill(FLTTMP, FREG, p);
                break;
        case CALL+I: case CALL+P: case CALL+U:
                spill(INTTMP, IREG, p);
                spill(FLTTMP, FREG, p);
                break;
        case CALL+V:
                spill(INTTMP, IREG, p);
                spill(FLTTMP, FREG, p);
                break;
        }
}

/* TODO */
static void emit2(Node p) {
#define preg(f) ((f)[getregnum(p->x.kids[0])]->x.name)
  if (generic(p->op) == CVI || generic(p->op) == CVU || 
      generic(p->op) == LOAD) {
    char *dst = ireg[getregnum(p)]->x.name;
    char *src = preg(ireg);
    if (dst != src)
      print("mov %s,%s\n", dst, src);
  }
}

/* OK */
static int imm16(Node p) {
        return range(p, -32766, 32767);
}

static int regop(Node p, int w) {
       assert(p);
       assert(p->kids[0]);
       assert(p->kids[1]);
       if (generic(p->kids[1]->kids[0]->op) == INDIR &&
	   sametree(p->kids[0], p->kids[1]->kids[0]->kids[0]))
	 return w;
       else
	 return LBURG_MAX;
}

static int sametree(Node p, Node q) {
  return p == NULL && q == NULL
    || p && q && p->op == q->op && p->syms[0] == q->syms[0]
    && sametree(p->kids[0], q->kids[0])
    && sametree(p->kids[1], q->kids[1]);
}

/* TODO */
static void local(Symbol p) {
        if (isfloat(p->type))
                p->sclass = AUTO;
        if (askregvar(p, rmap(ttob(p->type))) == 0) {
                assert(p->sclass == AUTO);
                offset = roundup(offset + p->type->size,
                            p->type->align < 2 ? 2 : p->type->align);
                p->x.offset = -offset;
                
                p->x.name = stringd(-offset);
        }
}

/* TODO */
static void function(Symbol f, Symbol caller[], Symbol callee[], int ncalls) {
        int i;

	globalend();
	print(".align 2\n");
	print(".type %s,@function\n", f->x.name);
	print("%s:\n", f->x.name);

        usedmask[IREG] = usedmask[FREG] = 0;
        freemask[IREG] = freemask[FREG] = ~(unsigned)0;
        offset = maxoffset = 0;

	/* Set up basic arg info for gencode.
           We patch up the offsets below, once we know
	   how many registers need to be put on the stack. */
        for (i = 0; callee[i]; i++) {
                Symbol p = callee[i];
                Symbol q = caller[i];
                assert(q);
                p->x.name = q->x.name = stringf("tmp");
		p->sclass = q->sclass = AUTO;
        }
        assert(caller[i] == 0);

        gencode(caller, callee);

	// Push used registers on stack
	offset = 8; // pc + fp
	for (i=0; i < 15; i++)
	  if (usedmask[IREG] & (1 << i)) {
            print("push %%%d\n", i);
	    offset += 4;
          }
	print("push %%30\n");
	print("mov %%30, %%31\n");

	// Pass 2: Assign the argument offsets in the stack
        for (i = 0; callee[i]; i++) {
                Symbol p = callee[i];
                Symbol q = caller[i];
                assert(q);
                offset = roundup(offset, q->type->align);
                p->x.offset = q->x.offset = offset;
                p->x.name = q->x.name = stringf("%d", offset);
                offset += roundup(q->type->size, 2);
        }
        assert(caller[i] == 0);

        framesize = roundup(maxoffset, 2);
        if (framesize > 0) {
	  print("sub %%31, %d\n", framesize);
        }

        emitcode();

	print("mov %%31, %%30\n");
	print("pop %%30\n");
	for (i=14; i >= 0; i--)
	  if (usedmask[IREG] & (1 << i))
            print("pop %%%d\n", i);
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
        print(".align %d\n", p->type->align > 2 ? 2 : p->type->align);
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
                        print(".comm %s,%d\n", p->x.name, p->type->size);
                else
                        print(".lcomm %s,%d\n", p->x.name, p->type->size);
        } else {
                print("%s:\n", p->x.name);
        }
}

/* OK */
static void space(int n) {
        if (cseg != BSS)
                print(".space %d\n", n);
}

static void doarg(Node p) {
        assert(p && p->syms[0]);
        mkactual(4, p->syms[0]->u.c.v.i);
}

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
        0,  /* left_to_right */
        0,  /* wants_dag */
        1,  /* unsigned_char */
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
