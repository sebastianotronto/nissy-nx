#ifndef FST_H
#define FST_H

#include "coord.h"

FstCube     cube_to_fst(Cube *cube);
FstCube     fst_inverse(FstCube fst);
bool        fst_is_solved(FstCube fst);
FstCube     fst_move(Move m, FstCube fst);
void        fst_to_cube(FstCube fst, Cube *cube);
void        init_fst();

#endif

