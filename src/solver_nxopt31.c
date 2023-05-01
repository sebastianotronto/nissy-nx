#include "solver_nxopt31.h"

/* TODO: make generic for nxopt solvers */

static void             apply_move_fst(void *, void *, Move);
static void *           prepare_cube_fst(void *, Cube *);
static bool             move_check_stop_nxopt31(void *, DfsArg *, Threader *);
static bool             is_solved_fst(void *, void *);
static void *           alloc_cubedata_fst(void *);
static void             copy_cubedata_fst(void *, void *, void *);
static void             free_cubedata_fst(void *, void *);
static void             invert_cubedata_fst(void *, void *);
static uint64_t         index_nxopt31(uint16_t, uint16_t, uint16_t, uint16_t, Trans);
static int              update(PruneData*, uint16_t *, uint16_t *,
                                        uint16_t *, uint16_t, Move, Trans);

static uint16_t cpudsep_table[FACTORIAL8];

Solver solver_nxopt31 = {
	.moveset           = &moveset_HTM,
	.move_check_stop   = move_check_stop_nxopt31,
	.validate_solution = NULL,
	.niss_makes_sense  = NULL,
	.param             = NULL, 
	.alloc_cubedata    = alloc_cubedata_fst,
	.copy_cubedata     = copy_cubedata_fst,
	.free_cubedata     = free_cubedata_fst,
	.invert_cube       = invert_cubedata_fst,
	.is_solved         = is_solved_fst,
	.apply_move        = apply_move_fst,
	.prepare_cube      = prepare_cube_fst,
};

/* TODO: this is a bit weird because of realloc,
	but we want to refactor this in any case */
static void
apply_move_fst(void *param, void *cubedata, Move m)
{
	FstCube *fst = (FstCube *)cubedata;

	FstCube *moved = malloc(sizeof(FstCube));
	*moved = fst_move(m, *fst);

	free(fst);
	cubedata = moved;
}

static void
init_cpudsep_table()
{
	static bool generated = false;
	uint64_t i;
	Cube c;

	if (generated)
		return;

	gen_coord(&coord_coud_cpudsep);
	for (i = 0; i < FACTORIAL8; i++) {
		index_to_perm(i, 8, c.cp);
		cpudsep_table[i] = index_coord(&coord_cpudsep, &c, NULL);
	}

	generated = true;
}

/* TODO: ugly */
void
prepare_solver_nxopt31()
{
	init_fst();
	gen_coord(&coord_eposepe);
	gen_coord(&coord_eofbepos_sym16);
	gen_coord(&coord_coud);
	gen_coord(&coord_cp);

	/* TODO: prepare also table cp pruning and 
	   internal table for cpudsep from cp */
	/* TODO: also check if ptables are already generated? */

	init_cpudsep_table();

	PruneData *pd = malloc(sizeof(PruneData));
	pd->moveset = &moveset_HTM;
	init_moveset(&moveset_HTM);
	pd->coord   = &coord_nxopt31;
	gen_coord(&coord_nxopt31);
	pd->compact = true;
	solver_nxopt31.param = genptable(pd, 4); /* TODO: threads */
}

static void *
prepare_cube_fst(void *param, Cube *cube)
{
	FstCube *fst = malloc(sizeof(FstCube));

	*fst = cube_to_fst(cube);

	return fst;
}

static bool
is_solved_fst(void *param, void *cubedata)
{
	return fst_is_solved(*(FstCube *)cubedata);
}

static void *
alloc_cubedata_fst(void *param)
{
	return malloc(sizeof(FstCube));
}

static void
copy_cubedata_fst(void *param, void *src, void *dst)
{
	memcpy(dst, src, sizeof(FstCube));
}

static void
free_cubedata_fst(void *param, void *cubedata)
{
	free(cubedata);
}

/* TODO: again, weird reallocation */
static void
invert_cubedata_fst(void *param, void *cubedata)
{
	FstCube *inv = malloc(sizeof(FstCube));

	*inv = fst_inverse(*(FstCube *)cubedata);
	free(cubedata);

	cubedata = inv;
}

static uint64_t
index_nxopt31(uint16_t eofb, uint16_t eposepe, uint16_t coud, uint16_t cp, Trans t)
{
	uint64_t eofbepos = (eposepe / FACTORIAL4) * POW2TO11 + eofb;
	uint64_t eofbepos_sym16 = coord_eofbepos_sym16.symclass[eofbepos];
	Trans ttr = coord_eofbepos_sym16.transtorep[eofbepos];
	if (t != uf)
		cp = trans_coord(&coord_cp, t, cp);
	uint64_t sep = cpudsep_table[cp];
	uint64_t coud_cpudsep = coud * BINOM8ON4 + sep;
	uint64_t cc = trans_coord(&coord_coud_cpudsep, ttr, coud_cpudsep);
	uint64_t ind = eofbepos_sym16 * POW3TO7 * BINOM8ON4 + cc;

	return ind;
}

static int
update(PruneData *pd, uint16_t *eofb, uint16_t *eposepe,
    uint16_t *coud, uint16_t cp, Move m, Trans t)
{
	uint64_t ind;

/* TODO fix
	*eofb    = coord_eofb.mtable[m][*eofb];
	*eposepe = coord_eposepe.mtable[m][*eposepe];
	*coud    = coord_coud.mtable[m][*coud];
*/

	ind = index_nxopt31(*eofb, *eposepe, *coud, cp, t);

	return ptableval(pd, ind);
}

static bool
move_check_stop_nxopt31(void *param, DfsArg *arg, Threader *threader)
{
/* TODO: implement nxopt trick, i.e. inverse probing + switching */
/*       this means making the cube data structure more complex,
         because we need to memorize the values from last probing
         not hard, but it means we can't delegate everything to
	 a cube structure (unless we change FstCube to also include
	 tmhe probing values, but it does not make much sense) */

	uint64_t ind;
	PruneData *pd = (PruneData *)param;
	FstCube *fst = (FstCube *)arg->cubedata;

	int l = arg->current_alg->len;
	int target = arg->d - l;

	Move m = l > 0 ? arg->current_alg->move[l-1] : NULLMOVE;
	/*fst->uf_cp = coord_cp.mtable[m][fst->uf_cp];*/
	fst_move(m, *fst);

	int nuf = update(pd, &fst->uf_eofb, &fst->uf_eposepe, &fst->uf_coud,
	    fst->uf_cp, m, uf);
	if (nuf > target)
		return true;
	int nfr = update(pd, &fst->fr_eofb, &fst->fr_eposepe, &fst->fr_coud,
	    fst->uf_cp, m, fr);
	if (nfr > target)
		return true;
	int nrd = update(pd, &fst->rd_eofb, &fst->rd_eposepe, &fst->rd_coud,
	    fst->uf_cp, m, rd);
	if (nrd > target)
		return true;

	if (nuf == nfr && nuf == nrd && nuf == target)
		return true;

	FstCube inv = fst_inverse(*fst);

	ind = index_nxopt31(inv.uf_eofb, inv.uf_eposepe, inv.uf_coud, inv.uf_cp, uf);
	int iuf = ptableval(pd, ind);
	if (iuf > target)
		return true;
	ind = index_nxopt31(inv.fr_eofb, inv.fr_eposepe, inv.fr_coud, inv.uf_cp, fr);
	int ifr = ptableval(pd, ind);
	if (ifr > target)
		return true;
	ind = index_nxopt31(inv.rd_eofb, inv.rd_eposepe, inv.rd_coud, inv.uf_cp, rd);
	int ird = ptableval(pd, ind);
	if (ird > target)
		return true;

	if (iuf == ifr && iuf == ird && iuf == target)
		return true;

	fst->uf_cp = coord_cp.mtable[m][fst->uf_cp];

/* TODO: check also pd_corners */
	return false;
}
