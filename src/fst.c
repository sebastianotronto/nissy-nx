#define FST_C

#include "fst.h"

static FstCube          ep_to_fst_epos(int *ep);
static void             init_fst_corner_invtables();
static void             init_fst_eo_invtables();
static void             init_fst_eo_update(uint64_t, uint64_t, int, Cube *);
static void             init_fst_where_is_edge();
static bool             read_fst_tables_file();
static bool             write_fst_tables_file();

static int edge_slice[12] = {[FR] = 0, [FL] = 0, [BL] = 0, [BR] = 0,
                             [UL] = 1, [UR] = 1, [DR] = 1, [DL] = 1,
                             [UF] = 2, [UB] = 2, [DF] = 2, [DB] = 2};

static uint16_t         inv_coud[FACTORIAL8][POW3TO7];
static uint16_t         inv_cp[FACTORIAL8];
static uint16_t         uf_cp_to_fr_cp[FACTORIAL8];
static uint16_t         uf_cp_to_rd_cp[FACTORIAL8];
static uint16_t         eo_invtable[3][POW2TO11][BINOM12ON4*FACTORIAL4];
static uint16_t         fst_where_is_edge_arr[3][12][BINOM12ON4*FACTORIAL4];

FstCube
cube_to_fst(Cube *cube)
{
	Cube c;
	FstCube ret;

	copy_cube(cube, &c);
	ret.uf_eofb    = coord_eofb.i[0]->index(&c);
	ret.uf_eposepe = coord_eposepe.i[0]->index(&c);
	ret.uf_coud    = coord_coud.i[0]->index(&c);
	ret.uf_cp      = coord_cp.i[0]->index(&c);
	copy_cube(cube, &c);
	apply_trans(fr, &c);
	ret.fr_eofb    = coord_eofb.i[0]->index(&c);
	ret.fr_eposepe = coord_eposepe.i[0]->index(&c);
	ret.fr_coud    = coord_coud.i[0]->index(&c);
	copy_cube(cube, &c);
	apply_trans(rd, &c);
	ret.rd_eofb    = coord_eofb.i[0]->index(&c);
	ret.rd_eposepe = coord_eposepe.i[0]->index(&c);
	ret.rd_coud    = coord_coud.i[0]->index(&c);

	return ret;
}

static FstCube
ep_to_fst_epos(int *ep)
{
	static int eind[12] = {
		[FR] = 0, [FL] = 1, [BL] = 2, [BR] = 3,
		[UR] = 0, [DR] = 1, [DL] = 2, [UL] = 3,
		[DB] = 0, [DF] = 1, [UF] = 2, [UB] = 3
	};
	static int eptrans_fr[12] = {
		[FR] = UF, [DF] = UL, [FL] = UB, [UF] = UR,
		[BR] = DF, [DB] = DL, [BL] = DB, [UB] = DR,
		[UR] = FR, [DR] = FL, [DL] = BL, [UL] = BR
	};
	static int eptrans_rd[12] = {
		[DR] = UF, [FR] = UL, [UR] = UB, [BR] = UR,
		[DL] = DF, [FL] = DL, [UL] = DB, [BL] = DR,
		[DB] = FR, [DF] = FL, [UF] = BL, [UB] = BR
	};

	FstCube ret;
	int i, ce, cs, cm;
	int epe[4], eps[4], epm[4], epose[12], eposs[12], eposm[12];

	memset(epose, 0, 12*sizeof(int));
	memset(eposs, 0, 12*sizeof(int));
	memset(eposm, 0, 12*sizeof(int));

	for (i = 0, ce = 0; i < 12; i++) {
		switch (edge_slice[ep[i]]) {
		case 0:
			epose[i] = 1;
			epe[ce++] = eind[ep[i]];
			break;
		case 1:
			eposs[eptrans_fr[i]] = eind[ep[i]] + 1;
			break;
		default:
			eposm[eptrans_rd[i]] = eind[ep[i]] + 1;
			break;
		}
	}

	for (i = 0, cs = 0, cm = 0; i < 12; i++) {
		if (eposs[i]) {
			eps[cs++] = eposs[i] - 1;
			eposs[i] = 1;
		}
		if (eposm[i]) {
			epm[cm++] = eposm[i] - 1;
			eposm[i] = 1;
		}
	}

	ret.uf_eposepe = subset_to_index(epose, 12, 4) * FACTORIAL4 +
	                 perm_to_index(epe, 4);
	ret.fr_eposepe = subset_to_index(eposs, 12, 4) * FACTORIAL4 +
	                 perm_to_index(eps, 4);
	ret.rd_eposepe = subset_to_index(eposm, 12, 4) * FACTORIAL4 +
	                 perm_to_index(epm, 4);

	return ret;
}

FstCube
fst_inverse(FstCube fst)
{
	FstCube ret;
	int ep_inv[12];

	ep_inv[FR] = fst_where_is_edge_arr[0][FR][fst.uf_eposepe];
	ep_inv[FL] = fst_where_is_edge_arr[0][FL][fst.uf_eposepe];
	ep_inv[BL] = fst_where_is_edge_arr[0][BL][fst.uf_eposepe];
	ep_inv[BR] = fst_where_is_edge_arr[0][BR][fst.uf_eposepe];

	ep_inv[UR] = fst_where_is_edge_arr[1][UR][fst.fr_eposepe];
	ep_inv[UL] = fst_where_is_edge_arr[1][UL][fst.fr_eposepe];
	ep_inv[DR] = fst_where_is_edge_arr[1][DR][fst.fr_eposepe];
	ep_inv[DL] = fst_where_is_edge_arr[1][DL][fst.fr_eposepe];

	ep_inv[UF] = fst_where_is_edge_arr[2][UF][fst.rd_eposepe];
	ep_inv[UB] = fst_where_is_edge_arr[2][UB][fst.rd_eposepe];
	ep_inv[DF] = fst_where_is_edge_arr[2][DF][fst.rd_eposepe];
	ep_inv[DB] = fst_where_is_edge_arr[2][DB][fst.rd_eposepe];

	ret = ep_to_fst_epos(ep_inv);

	ret.uf_eofb = ((uint16_t)eo_invtable[0][fst.uf_eofb][fst.uf_eposepe]) |
	              ((uint16_t)eo_invtable[1][fst.uf_eofb][fst.fr_eposepe]) |
	              ((uint16_t)eo_invtable[2][fst.uf_eofb][fst.rd_eposepe]);
	ret.fr_eofb = ((uint16_t)eo_invtable[0][fst.fr_eofb][fst.uf_eposepe]) |
	              ((uint16_t)eo_invtable[1][fst.fr_eofb][fst.fr_eposepe]) |
	              ((uint16_t)eo_invtable[2][fst.fr_eofb][fst.rd_eposepe]);
	ret.rd_eofb = ((uint16_t)eo_invtable[0][fst.rd_eofb][fst.uf_eposepe]) |
	              ((uint16_t)eo_invtable[1][fst.rd_eofb][fst.fr_eposepe]) |
	              ((uint16_t)eo_invtable[2][fst.rd_eofb][fst.rd_eposepe]);

	ret.uf_cp = inv_cp[fst.uf_cp];

	ret.uf_coud = inv_coud[fst.uf_cp][fst.uf_coud];
	ret.fr_coud = inv_coud[uf_cp_to_fr_cp[fst.uf_cp]][fst.fr_coud];
	ret.rd_coud = inv_coud[uf_cp_to_rd_cp[fst.uf_cp]][fst.rd_coud];

	return ret;
}

bool
fst_is_solved(FstCube fst)
{
	/* We only check one orientation */

	return fst.uf_eofb == 0 && fst.uf_eposepe == 0 &&
	       fst.uf_coud == 0 && fst.uf_cp == 0;
}

FstCube
fst_move(Move m, FstCube fst)
{
	FstCube ret;
	Move m_fr, m_rd;

	m_fr = transform_move(fr, m);
	m_rd = transform_move(rd, m);

	ret.uf_eofb    = coord_eofb.mtable[m][fst.uf_eofb];
	ret.uf_eposepe = coord_eposepe.mtable[m][fst.uf_eposepe];
	ret.uf_coud    = coord_coud.mtable[m][fst.uf_coud];
	ret.uf_cp      = coord_cp.mtable[m][fst.uf_cp];

	ret.fr_eofb    = coord_eofb.mtable[m_fr][fst.fr_eofb];
	ret.fr_eposepe = coord_eposepe.mtable[m_fr][fst.fr_eposepe];
	ret.fr_coud    = coord_coud.mtable[m_fr][fst.fr_coud];

	ret.rd_eofb    = coord_eofb.mtable[m_rd][fst.rd_eofb];
	ret.rd_eposepe = coord_eposepe.mtable[m_rd][fst.rd_eposepe];
	ret.rd_coud    = coord_coud.mtable[m_rd][fst.rd_coud];

	return ret;
}

void
fst_to_cube(FstCube fst, Cube *cube)
{
	Cube e, s, m;
	int i;

	coord_eposepe.i[0]->to_cube(fst.uf_eposepe, &e);
	coord_eposepe.i[0]->to_cube(fst.fr_eposepe, &s);
	apply_trans(inverse_trans(fr), &s);
	coord_eposepe.i[0]->to_cube(fst.rd_eposepe, &m);
	apply_trans(inverse_trans(rd), &m);

	for (i = 0; i < 12; i++) {
		if (edge_slice[e.ep[i]] == 0)
			cube->ep[i] = e.ep[i];
		if (edge_slice[s.ep[i]] == 1)
			cube->ep[i] = s.ep[i];
		if (edge_slice[m.ep[i]] == 2)
			cube->ep[i] = m.ep[i];
	}

	coord_eofb.i[0]->to_cube((uint64_t)fst.uf_eofb, cube);
	coord_coud.i[0]->to_cube((uint64_t)fst.uf_coud, cube);
	coord_cp.i[0]->to_cube((uint64_t)fst.uf_cp, cube);

	for (i = 0; i < 6; i++)
		cube->xp[i] = i;
}

void
init_fst()
{
	init_trans();
	gen_coord(&coord_eofb);
	gen_coord(&coord_eposepe);
	gen_coord(&coord_coud);
	gen_coord(&coord_cp);

	if (!read_fst_tables_file()) {
		fprintf(stderr,
		    "Could not load fst_tables, generating them\n");
		init_fst_corner_invtables();
		init_fst_eo_invtables();
		init_fst_where_is_edge();
		if (!write_fst_tables_file())
			fprintf(stderr, "fst_tables could not be written\b");
	}
}

static void
init_fst_corner_invtables()
{
	Cube c, d;
	uint64_t cp, coud;

	for (cp = 0; cp < FACTORIAL8; cp++) {
		make_solved_corners(&c);
		coord_cp.i[0]->to_cube(cp, &c);

		copy_cube_corners(&c, &d);
		invert_cube_corners(&d);
		inv_cp[cp] = coord_cp.i[0]->index(&d);

		for (coud = 0; coud < POW3TO7; coud++) {
			copy_cube_corners(&c, &d);
			coord_coud.i[0]->to_cube(coud, &d);
			invert_cube_corners(&d);
			inv_coud[cp][coud] = coord_coud.i[0]->index(&d);
		}

		copy_cube_corners(&c, &d);
		apply_trans(fr, &d);
		uf_cp_to_fr_cp[cp] = coord_cp.i[0]->index(&d);

		copy_cube_corners(&c, &d);
		apply_trans(rd, &d);
		uf_cp_to_rd_cp[cp] = coord_cp.i[0]->index(&d);
	}
}

static void
init_fst_eo_invtables()
{
	uint64_t ep, eo;
	Cube c, d;

	for (ep = 0; ep < BINOM12ON4 * FACTORIAL4; ep++) {
		make_solved(&c);
		coord_eposepe.i[0]->to_cube(ep, &c);
		for (eo = 0; eo < POW2TO11; eo++) {
			copy_cube_edges(&c, &d);
			coord_eofb.i[0]->to_cube(eo, &d);
			init_fst_eo_update(eo, ep, 0, &d);

			apply_trans(inverse_trans(fr), &d);
			coord_eofb.i[0]->to_cube(eo, &d);
			init_fst_eo_update(eo, ep, 1, &d);

			copy_cube_edges(&c, &d);
			apply_trans(inverse_trans(rd), &d);
			coord_eofb.i[0]->to_cube(eo, &d);
			init_fst_eo_update(eo, ep, 2, &d);
		}
	}
}

static void
init_fst_eo_update(uint64_t eo, uint64_t ep, int s, Cube *d)
{
	int i;

	for (i = 0; i < 12; i++) {
		if (edge_slice[d->ep[i]] == s && d->eo[i] && d->ep[i] != 11)
			eo_invtable[s][eo][ep] |=
			    ((uint16_t)1) << ((uint16_t)d->ep[i]);
	}
}

static void
init_fst_where_is_edge()
{
	Cube c, d;
	uint64_t e;

	make_solved(&c);
	for (e = 0; e < BINOM12ON4 * FACTORIAL4; e++) {
		coord_eposepe.i[0]->to_cube(e, &c);

		copy_cube_edges(&c, &d);
		fst_where_is_edge_arr[0][FR][e] = where_is_edge(FR, &d);
		fst_where_is_edge_arr[0][FL][e] = where_is_edge(FL, &d);
		fst_where_is_edge_arr[0][BL][e] = where_is_edge(BL, &d);
		fst_where_is_edge_arr[0][BR][e] = where_is_edge(BR, &d);

		copy_cube_edges(&c, &d);
		apply_trans(inverse_trans(fr), &d);
		fst_where_is_edge_arr[1][UL][e] = where_is_edge(UL, &d);
		fst_where_is_edge_arr[1][UR][e] = where_is_edge(UR, &d);
		fst_where_is_edge_arr[1][DL][e] = where_is_edge(DL, &d);
		fst_where_is_edge_arr[1][DR][e] = where_is_edge(DR, &d);

		copy_cube_edges(&c, &d);
		apply_trans(inverse_trans(rd), &d);
		fst_where_is_edge_arr[2][UF][e] = where_is_edge(UF, &d);
		fst_where_is_edge_arr[2][UB][e] = where_is_edge(UB, &d);
		fst_where_is_edge_arr[2][DF][e] = where_is_edge(DF, &d);
		fst_where_is_edge_arr[2][DB][e] = where_is_edge(DB, &d);
	}
}

static bool
read_fst_tables_file()
{
	init_env();

	FILE *f;
	char fname[strlen(tabledir)+256];
	uint64_t i, j, r, total;

	strcpy(fname, tabledir);
	strcat(fname, "/fst_tables");

	if ((f = fopen(fname, "rb")) == NULL)
		return false;

	r = 0;
	total = FACTORIAL8*(POW3TO7+3) + 3*BINOM12ON4*FACTORIAL4*(12+POW2TO11);

	for (i = 0; i < FACTORIAL8; i++)
		r += fread(inv_coud[i], sizeof(uint16_t), POW3TO7, f);
	r += fread(inv_cp, sizeof(uint16_t), FACTORIAL8, f);
	r += fread(uf_cp_to_fr_cp, sizeof(uint16_t), FACTORIAL8, f);
	r += fread(uf_cp_to_rd_cp, sizeof(uint16_t), FACTORIAL8, f);
	for (i = 0; i < 3; i++)
		for (j = 0; j < POW2TO11; j++)
			r += fread(eo_invtable[i][j],
			    sizeof(uint16_t), BINOM12ON4*FACTORIAL4, f);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 12; j++)
			r += fread(fst_where_is_edge_arr[i][j],
			    sizeof(uint16_t), BINOM12ON4*FACTORIAL4, f);

	fclose(f);

	return r == total;
}

static bool
write_fst_tables_file()
{
	init_env();

	FILE *f;
	char fname[strlen(tabledir)+256];
	uint64_t i, j, w, total;

	strcpy(fname, tabledir);
	strcat(fname, "/fst_tables");

	if ((f = fopen(fname, "wb")) == NULL)
		return false;

	w = 0;
	total = FACTORIAL8*(POW3TO7+3) + 3*BINOM12ON4*FACTORIAL4*(12+POW2TO11);

	for (i = 0; i < FACTORIAL8; i++)
		w += fwrite(inv_coud[i], sizeof(uint16_t), POW3TO7, f);
	w += fwrite(inv_cp, sizeof(uint16_t), FACTORIAL8, f);
	w += fwrite(uf_cp_to_fr_cp, sizeof(uint16_t), FACTORIAL8, f);
	w += fwrite(uf_cp_to_rd_cp, sizeof(uint16_t), FACTORIAL8, f);
	for (i = 0; i < 3; i++)
		for (j = 0; j < POW2TO11; j++)
			w += fwrite(eo_invtable[i][j],
			    sizeof(uint16_t), BINOM12ON4*FACTORIAL4, f);
	for (i = 0; i < 3; i++)
		for (j = 0; j < 12; j++)
			w += fwrite(fst_where_is_edge_arr[i][j],
			    sizeof(uint16_t), BINOM12ON4*FACTORIAL4, f);

	fclose(f);

	return w == total;
}
