#ifndef PRUNING_H
#define PRUNING_H

#include "coord.h"

void        free_pd(PruneData *pd);
PruneData * genptable(PDGenData *data, int nthreads);
void        print_ptable(PruneData *pd);
uint64_t    ptablesize(PruneData *pd);
int         ptableval(PruneData *pd, uint64_t ind);

extern PDGenData *active_pdg[256];

#endif

