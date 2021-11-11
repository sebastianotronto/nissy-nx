#include "moves.h"

/* Local functions ***********************************************************/

static Cube        apply_move_cubearray(Move m, Cube cube, PieceFilter f);
static bool        read_mtables_file();
static bool        write_mtables_file();

/* Tables and other data *****************************************************/

/* Every move is translated to a an <U, x, y> alg before filling the
   transition tables, see init_moves() */

static int              edge_cycle[NMOVES][12] =
{
	[U] = { UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR },
	[x] = { DF, FL, UF, FR, DB, BL, UB, BR, DR, DL, UL, UR },
	[y] = { UR, UF, UL, UB, DR, DF, DL, DB, BR, FR, FL, BL }
};

static int              corner_cycle[NMOVES][8] =
{
	[U] = { UBR, UFR, UFL, UBL, DFR, DFL, DBL, DBR },
	[x] = { DFR, DFL, UFL, UFR, DBR, DBL, UBL, UBR },
	[y] = { UBR, UFR, UFL, UBL, DBR, DFR, DFL, DBL }
};

static int              center_cycle[NMOVES][6] =
{
	[x] = { F_center, B_center, R_center, L_center, D_center, U_center },
	[y] = { U_center, D_center, B_center, F_center, R_center, L_center }
};

static int              eofb_flipped[NMOVES][12] = {
	[x] = { [UF] = 1, [UB] = 1, [DF] = 1, [DB] = 1 },
	[y] = { [FR] = 1, [FL] = 1, [BL] = 1, [BR] = 1 }
};

static int              eorl_flipped[NMOVES][12] = {
	[x] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
	[y] = { [FR] = 1, [FL] = 1, [BL] = 1, [BR] = 1 }
};

static int              eoud_flipped[NMOVES][12] = {
	[U] = { [UF] = 1, [UL] = 1, [UB] = 1, [UR] = 1 },
	[x] = { [UF] = 1, [UB] = 1, [DF] = 1, [DB] = 1 },
	[y] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  }
};

static int              coud_flipped[NMOVES][8] = {
	[x] = {
		[UFR] = 2, [UBR] = 1, [UFL] = 1, [UBL] = 2,
		[DBR] = 2, [DFR] = 1, [DBL] = 1, [DFL] = 2
	}
};

static int              corl_flipped[NMOVES][8] = {
	[U] = { [UFR] = 1, [UBR] = 2, [UBL] = 1, [UFL] = 2 },
	[y] = {
		[UFR] = 1, [UBR] = 2, [UBL] = 1, [UFL] = 2,
		[DFR] = 2, [DBR] = 1, [DBL] = 2, [DFL] = 1
	}
};

static int              cofb_flipped[NMOVES][8] = {
	[U] = { [UFR] = 2, [UBR] = 1, [UBL] = 2, [UFL] = 1 },
	[x] = {
		[UFR] = 1, [UBR] = 2, [UBL] = 1, [UFL] = 2,
		[DFR] = 2, [DBR] = 1, [DBL] = 2, [DFL] = 1
	},
	[y] = {
		[UFR] = 2, [UBR] = 1, [UBL] = 2, [UFL] = 1,
		[DFR] = 1, [DBR] = 2, [DBL] = 1, [DFL] = 2
	}
};

static char equiv_alg_string[100][NMOVES] = {
	[NULLMOVE] = "",

	[U]   = "        U           ",
	[U2]  = "        UU          ",
	[U3]  = "        UUU         ",
	[D]   = "  xx    U    xx      ",
	[D2]  = "  xx    UU   xx      ",
	[D3]  = "  xx    UUU  xx      ",
	[R]   = "  yx    U    xxxyyy  ",
	[R2]  = "  yx    UU   xxxyyy  ",
	[R3]  = "  yx    UUU  xxxyyy  ",
	[L]   = "  yyyx  U    xxxy    ",
	[L2]  = "  yyyx  UU   xxxy    ",
	[L3]  = "  yyyx  UUU  xxxy    ",
	[F]   = "  x     U    xxx     ",
	[F2]  = "  x     UU   xxx     ",
	[F3]  = "  x     UUU  xxx     ",
	[B]   = "  xxx   U    x       ",
	[B2]  = "  xxx   UU   x       ",
	[B3]  = "  xxx   UUU  x       ",

	[Uw]  = "  xx    U    xx      y        ",
	[Uw2] = "  xx    UU   xx      yy       ",
	[Uw3] = "  xx    UUU  xx      yyy      ",
	[Dw]  = "        U            yyy      ",
	[Dw2] = "        UU           yy       ",
	[Dw3] = "        UUU          y        ",
	[Rw]  = "  yyyx  U    xxxy    x        ",
	[Rw2] = "  yyyx  UU   xxxy    xx       ",
	[Rw3] = "  yyyx  UUU  xxxy    xxx      ",
	[Lw]  = "  yx    U    xxxyyy  xxx      ",
	[Lw2] = "  yx    UU   xxxyyy  xx       ",
	[Lw3] = "  yx    UUU  xxxyyy  x        ",
	[Fw]  = "  xxx   U    x       yxxxyyy  ",
	[Fw2] = "  xxx   UU   x       yxxyyy   ",
	[Fw3] = "  xxx   UUU  x       yxyyy    ",
	[Bw]  = "  x     U    xxx     yxyyy    ",
	[Bw2] = "  x     UU   xxx     yxxyyy   ",
	[Bw3] = "  x     UUU  xxx     yxxxyyy  ",

	[M]   = "  yx  U    xx  UUU  yxyyy  ",
	[M2]  = "  yx  UU   xx  UU   xxxy   ",
	[M3]  = "  yx  UUU  xx  U    yxxxy  ",
	[S]   = "  x   UUU  xx  U    yyyx   ",
	[S2]  = "  x   UU   xx  UU   yyx    ",
	[S3]  = "  x   U    xx  UUU  yx     ",
	[E]   = "      U    xx  UUU  xxyyy  ",
	[E2]  = "      UU   xx  UU   xxyy   ",
	[E3]  = "      UUU  xx  U    xxy    ",

	[x]   = "       x         ",
	[x2]  = "       xx        ",
	[x3]  = "       xxx       ",
	[y]   = "       y         ",
	[y2]  = "       yy        ",
	[y3]  = "       yyy       ",
	[z]   = "  yyy  x    y    ",
	[z2]  = "  yy   xx        ",
	[z3]  = "  y    x    yyy  "
};

/* Transition tables, to be loaded up at the beginning */
static int              epose_mtable[NMOVES][FACTORIAL12/FACTORIAL8];
static int              eposs_mtable[NMOVES][FACTORIAL12/FACTORIAL8];
static int              eposm_mtable[NMOVES][FACTORIAL12/FACTORIAL8];
static int              eofb_mtable[NMOVES][POW2TO11];
static int              eorl_mtable[NMOVES][POW2TO11];
static int              eoud_mtable[NMOVES][POW2TO11];
static int              cp_mtable[NMOVES][FACTORIAL8];
static int              coud_mtable[NMOVES][POW3TO7];
static int              cofb_mtable[NMOVES][POW3TO7];
static int              corl_mtable[NMOVES][POW3TO7];
static int              cpos_mtable[NMOVES][FACTORIAL6];


/* Local functions implementation ********************************************/

static Cube
apply_move_cubearray(Move m, Cube cube, PieceFilter f)
{
	/*init_moves();*/

	CubeArray m_arr = {
		edge_cycle[m],
		eofb_flipped[m],
		eorl_flipped[m],
		eoud_flipped[m],
		corner_cycle[m],
		coud_flipped[m],
		corl_flipped[m],
		cofb_flipped[m],
		center_cycle[m]
	};

	return move_via_arrays(&m_arr, cube, f);
}

/* Public functions **********************************************************/

Cube
apply_alg_generic(Alg *alg, Cube c, PieceFilter f, bool a)
{
	Cube ret = {0};
	int i;

	for (i = 0; i < alg->len; i++)
		if (alg->inv[i])
			ret = a ? apply_move(alg->move[i], ret) :
			          apply_move_cubearray(alg->move[i], ret, f);

	ret = compose_filtered(c, inverse_cube(ret), f);

	for (i = 0; i < alg->len; i++)
		if (!alg->inv[i])
			ret = a ? apply_move(alg->move[i], ret) :
			          apply_move_cubearray(alg->move[i], ret, f);

	return ret;
}

Cube
apply_alg(Alg *alg, Cube cube)
{
	return apply_alg_generic(alg, cube, pf_all, true);
}

Cube
apply_move(Move m, Cube cube)
{
	/*init_moves();*/

	return (Cube) {
		.epose = epose_mtable[m][cube.epose],
		.eposs = eposs_mtable[m][cube.eposs],
		.eposm = eposm_mtable[m][cube.eposm],
		.eofb  = eofb_mtable[m][cube.eofb],
		.eorl  = eorl_mtable[m][cube.eorl],
		.eoud  = eoud_mtable[m][cube.eoud],
		.coud  = coud_mtable[m][cube.coud],
		.cofb  = cofb_mtable[m][cube.cofb],
		.corl  = corl_mtable[m][cube.corl],
		.cp    = cp_mtable[m][cube.cp],
		.cpos  = cpos_mtable[m][cube.cpos]
	};
}

static bool
read_mtables_file()
{
	init_env();

	FILE *f;
	char fname[strlen(tabledir)+20];
	int m, b = sizeof(int);
	bool r = true;

	/* Table sizes, used for reading and writing files */
	uint64_t me[11] = {
		[0]  = FACTORIAL12/FACTORIAL8,
		[1]  = FACTORIAL12/FACTORIAL8,
		[2]  = FACTORIAL12/FACTORIAL8,
		[3]  = POW2TO11,
		[4]  = POW2TO11,
		[5]  = POW2TO11,
		[6]  = FACTORIAL8,
		[7]  = POW3TO7,
		[8]  = POW3TO7,
		[9]  = POW3TO7,
		[10] = FACTORIAL6
	};

	strcpy(fname, tabledir);
	strcat(fname, "/mtables");

	if ((f = fopen(fname, "rb")) == NULL)
		return false;

	for (m = 0; m < NMOVES; m++) {
		r = r && fread(epose_mtable[m], b, me[0],  f) == me[0];
		r = r && fread(eposs_mtable[m], b, me[1],  f) == me[1];
		r = r && fread(eposm_mtable[m], b, me[2],  f) == me[2];
		r = r && fread(eofb_mtable[m],  b, me[3],  f) == me[3];
		r = r && fread(eorl_mtable[m],  b, me[4],  f) == me[4];
		r = r && fread(eoud_mtable[m],  b, me[5],  f) == me[5];
		r = r && fread(cp_mtable[m],    b, me[6],  f) == me[6];
		r = r && fread(coud_mtable[m],  b, me[7],  f) == me[7];
		r = r && fread(corl_mtable[m],  b, me[8],  f) == me[8];
		r = r && fread(cofb_mtable[m],  b, me[9],  f) == me[9];
		r = r && fread(cpos_mtable[m],  b, me[10], f) == me[10];
	}

	fclose(f);
	return r;
}

static bool
write_mtables_file()
{
	init_env();

	FILE *f;
	char fname[strlen(tabledir)+20];
	int m, b = sizeof(int);
	bool r = true;

	/* Table sizes, used for reading and writing files */
	uint64_t me[11] = {
		[0]  = FACTORIAL12/FACTORIAL8,
		[1]  = FACTORIAL12/FACTORIAL8,
		[2]  = FACTORIAL12/FACTORIAL8,
		[3]  = POW2TO11,
		[4]  = POW2TO11,
		[5]  = POW2TO11,
		[6]  = FACTORIAL8,
		[7]  = POW3TO7,
		[8]  = POW3TO7,
		[9]  = POW3TO7,
		[10] = FACTORIAL6
	};

	strcpy(fname, tabledir);
	strcat(fname, "/mtables");

	if ((f = fopen(fname, "wb")) == NULL)
		return false;

	for (m = 0; m < NMOVES; m++) {
		r = r && fwrite(epose_mtable[m], b, me[0],  f) == me[0];
		r = r && fwrite(eposs_mtable[m], b, me[1],  f) == me[1];
		r = r && fwrite(eposm_mtable[m], b, me[2],  f) == me[2];
		r = r && fwrite(eofb_mtable[m],  b, me[3],  f) == me[3];
		r = r && fwrite(eorl_mtable[m],  b, me[4],  f) == me[4];
		r = r && fwrite(eoud_mtable[m],  b, me[5],  f) == me[5];
		r = r && fwrite(cp_mtable[m],    b, me[6],  f) == me[6];
		r = r && fwrite(coud_mtable[m],  b, me[7],  f) == me[7];
		r = r && fwrite(corl_mtable[m],  b, me[8],  f) == me[8];
		r = r && fwrite(cofb_mtable[m],  b, me[9],  f) == me[9];
		r = r && fwrite(cpos_mtable[m],  b, me[10], f) == me[10];
	}

	fclose(f);
	return r;
}

bool
commute(Move m1, Move m2)
{
	static bool initialized = false;
	static bool commute_aux[NMOVES][NMOVES];

	if (!initialized) {
		Cube c1, c2;
		int i, j;

		for (i = 0; i < NMOVES; i++) {
			for (j = 0; j < NMOVES; j++) {
				c1 = apply_move(i, apply_move(j, (Cube){0}));
				c2 = apply_move(j, apply_move(i, (Cube){0}));
				commute_aux[i][j] = equal(c1, c2) && i && j;
			}
		}

		initialized = true;
	}

	return commute_aux[m1][m2];
}

bool
possible_next(Move m1, Move m2, Move m3)
{
	static bool initialized = false;
	static bool paux[NMOVES][NMOVES][NMOVES];

	if (!initialized) {
		int i, j, k;
		bool p, q, c;

		for (i = 0; i < NMOVES; i++) {
			for (j = 0; j < NMOVES; j++) {
				for (k = 0; k < NMOVES; k++) {
					p = j && base_move(j) == base_move(k);
					q = i && base_move(i) == base_move(k);
					c = commute(i, j);
					paux[i][j][k] = !(p || (c && q));
				}
			}
		}

		initialized = true;
	}

	return paux[m1][m2][m3];
}

void
init_moves() {
	static bool initialized = false;
	if (initialized)
		return;
	initialized = true;

	Cube c;
	CubeArray arrs;
	int i;
	unsigned int ui;
	Move m;
	Alg *equiv_alg[NMOVES];

	for (i = 0; i < NMOVES; i++)
		equiv_alg[i] = new_alg(equiv_alg_string[i]);

	/* Generate all move cycles and flips; I do this regardless */
	for (i = 0; i < NMOVES; i++) {
		if (i == U || i == x || i == y)
			continue;

		c = apply_alg_generic(equiv_alg[i], (Cube){0}, pf_all, false);

		arrs = (CubeArray) {
			edge_cycle[i],
			eofb_flipped[i],
			eorl_flipped[i],
			eoud_flipped[i],
			corner_cycle[i],
			coud_flipped[i],
			corl_flipped[i],
			cofb_flipped[i],
			center_cycle[i]
		};
		cube_to_arrays(c, &arrs, pf_all);
	}

	if (read_mtables_file())
		return;

	fprintf(stderr, "Cannot load %s, generating it\n", "mtables"); 

	/* Initialize transition tables */
	for (m = 0; m < NMOVES; m++) {
		for (ui = 0; ui < FACTORIAL12/FACTORIAL8; ui++) {
			c = (Cube){ .epose = ui };
			c = apply_move_cubearray(m, c, pf_e);
			epose_mtable[m][ui] = c.epose;

			c = (Cube){ .eposs = ui };
			c = apply_move_cubearray(m, c, pf_s);
			eposs_mtable[m][ui] = c.eposs;

			c = (Cube){ .eposm = ui };
			c = apply_move_cubearray(m, c, pf_m);
			eposm_mtable[m][ui] = c.eposm;
		}
		for (ui = 0; ui < POW2TO11; ui++ ) {
			c = (Cube){ .eofb = ui };
			c = apply_move_cubearray(m, c, pf_eo);
			eofb_mtable[m][ui] = c.eofb;

			c = (Cube){ .eorl = ui };
			c = apply_move_cubearray(m, c, pf_eo);
			eorl_mtable[m][ui] = c.eorl;

			c = (Cube){ .eoud = ui };
			c = apply_move_cubearray(m, c, pf_eo);
			eoud_mtable[m][ui] = c.eoud;
		}
		for (ui = 0; ui < POW3TO7; ui++) {
			c = (Cube){ .coud = ui };
			c = apply_move_cubearray(m, c, pf_co);
			coud_mtable[m][ui] = c.coud;

			c = (Cube){ .corl = ui };
			c = apply_move_cubearray(m, c, pf_co);
			corl_mtable[m][ui] = c.corl;

			c = (Cube){ .cofb = ui };
			c = apply_move_cubearray(m, c, pf_co);
			cofb_mtable[m][ui] = c.cofb;
		}
		for (ui = 0; ui < FACTORIAL8; ui++) {
			c = (Cube){ .cp = ui };
			c = apply_move_cubearray(m, c, pf_cp);
			cp_mtable[m][ui] = c.cp;
		}
		for (ui = 0; ui < FACTORIAL6; ui++) {
			c = (Cube){ .cpos = ui };
			c = apply_move_cubearray(m, c, pf_cpos);
			cpos_mtable[m][ui] = c.cpos;
		}
	}

	if (!write_mtables_file())
		fprintf(stderr, "Error writing mtables\n");

	for (i = 0; i < NMOVES; i++)
		free_alg(equiv_alg[i]);
}

