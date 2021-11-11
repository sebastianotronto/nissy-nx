#include "pieces.h"

char edge_string[12][5] = {
  "UF", "UL", "UB", "UR", "DF", "DL", "DB", "DR", "FR", "FL", "BL", "BR"
};

char corner_string[8][5] = {
  "UFR", "UFL", "UBL", "UBR", "DFR", "DFL", "DBL", "DBR"
};

char center_string[6][5] = { "U", "D", "R", "L", "F", "B" };

char move_string[NULLMOVE+1][5] = {
  "U", "U2", "U\'", "D", "D2", "D\'", "R", "R2", "R\'",
  "L", "L2", "L\'", "F", "F2", "F\'", "B", "B2", "B\'",
  "Uw", "Uw2", "Uw\'", "Dw", "Dw2", "Dw\'", "Rw", "Rw2", "Rw\'",
  "Lw", "Lw2", "Lw\'", "Fw", "Fw2", "Fw\'", "Bw", "Bw2", "Bw\'",
  "M", "M2", "M\'", "S", "S2", "S\'", "E", "E2", "E\'",
  "x", "x2", "x\'", "y", "y2", "y\'", "z", "z2", "z\'",
  "-"
};

int edge_cycle[NULLMOVE+1][12] = {
  [U]  = {UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR},
  [U2] = {UB, UR, UF, UL, DF, DL, DB, DR, FR, FL, BL, BR},
  [U3] = {UL, UB, UR, UF, DF, DL, DB, DR, FR, FL, BL, BR},
  [D]  = {UF, UL, UB, UR, DL, DB, DR, DF, FR, FL, BL, BR},
  [D2] = {UF, UL, UB, UR, DB, DR, DF, DL, FR, FL, BL, BR},
  [D3] = {UF, UL, UB, UR, DR, DF, DL, DB, FR, FL, BL, BR},
  [R]  = {UF, UL, UB, FR, DF, DL, DB, BR, DR, FL, BL, UR},
  [R2] = {UF, UL, UB, DR, DF, DL, DB, UR, BR, FL, BL, FR},
  [R3] = {UF, UL, UB, BR, DF, DL, DB, FR, UR, FL, BL, DR},
  [L]  = {UF, BL, UB, UR, DF, FL, DB, DR, FR, UL, DL, BR},
  [L2] = {UF, DL, UB, UR, DF, UL, DB, DR, FR, BL, FL, BR},
  [L3] = {UF, FL, UB, UR, DF, BL, DB, DR, FR, DL, UL, BR},
  [F]  = {FL, UL, UB, UR, FR, DL, DB, DR, UF, DF, BL, BR},
  [F2] = {DF, UL, UB, UR, UF, DL, DB, DR, FL, FR, BL, BR},
  [F3] = {FR, UL, UB, UR, FL, DL, DB, DR, DF, UF, BL, BR},
  [B]  = {UF, UL, BR, UR, DF, DL, BL, DR, FR, FL, UB, DB},
  [B2] = {UF, UL, DB, UR, DF, DL, UB, DR, FR, FL, BR, BL},
  [B3] = {UF, UL, BL, UR, DF, DL, BR, DR, FR, FL, DB, UB}, 
  
  [Uw]  = {UR, UF, UL, UB, DF, DL, DB, DR, BR, FR, FL, BL},
  [Uw2] = {UB, UR, UF, UL, DF, DL, DB, DR, BL, BR, FR, FL},
  [Uw3] = {UL, UB, UR, UF, DF, DL, DB, DR, FL, BL, BR, FR},
  [Dw]  = {UF, UL, UB, UR, DL, DB, DR, DF, FL, BL, BR, FR},
  [Dw2] = {UF, UL, UB, UR, DB, DR, DF, DL, BL, BR, FR, FL},
  [Dw3] = {UF, UL, UB, UR, DR, DF, DL, DB, BR, FR, FL, BL},
  [Rw]  = {DF, UL, UF, FR, DB, DL, UB, BR, DR, FL, BL, UR},
  [Rw2] = {DB, UL, DF, DR, UB, DL, UF, UR, BR, FL, BL, FR},
  [Rw3] = {UB, UL, DB, BR, UF, DL, DF, FR, UR, FL, BL, DR},
  [Lw]  = {UB, BL, DB, UR, UF, FL, DF, DR, FR, UL, DL, BR},
  [Lw2] = {DB, DL, DF, UR, UB, UL, UF, DR, FR, BL, FL, BR},
  [Lw3] = {DF, FL, UF, UR, DB, BL, UB, DR, FR, DL, UL, BR},
  [Fw]  = {FL, DL, UB, UL, FR, DR, DB, UR, UF, DF, BL, BR},
  [Fw2] = {DF, DR, UB, DL, UF, UR, DB, UL, FL, FR, BL, BR},
  [Fw3] = {FR, UR, UB, DR, FL, UL, DB, DL, DF, UF, BL, BR},
  [Bw]  = {UF, UR, BR, DR, DF, UL, BL, DL, FR, FL, UB, DB},
  [Bw2] = {UF, DR, DB, DL, DF, UR, UB, UL, FR, FL, BR, BL},
  [Bw3] = {UF, DL, BL, UL, DF, DR, BR, UR, FR, FL, DB, UB}, 

  [M]  = {UB, UL, DB, UR, UF, DL, DF, DR, FR, FL, BL, BR},
  [M2] = {DB, UL, DF, UR, UB, DL, UF, DR, FR, FL, BL, BR},
  [M3] = {DF, UL, UF, UR, DB, DL, UB, DR, FR, FL, BL, BR},
  [S]  = {UF, DL, UB, UL, DF, DR, DB, UR, FR, FL, BL, BR},
  [S2] = {UF, DR, UB, DL, DF, UR, DB, UL, FR, FL, BL, BR},
  [S3] = {UF, UR, UB, DR, DF, UL, DB, DL, FR, FL, BL, BR},
  [E]  = {UF, UL, UB, UR, DF, DL, DB, DR, FL, BL, BR, FR},
  [E2] = {UF, UL, UB, UR, DF, DL, DB, DR, BL, BR, FR, FL},
  [E3] = {UF, UL, UB, UR, DF, DL, DB, DR, BR, FR, FL, BL},

  [X]  = {DF, FL, UF, FR, DB, BL, UB, BR, DR, DL, UL, UR},
  [X2] = {DB, DL, DF, DR, UB, UL, UF, UR, BR, BL, FL, FR},
  [X3] = {UB, BL, DB, BR, UF, FL, DF, FR, UR, UL, DL, DR},
  [Y]  = {UR, UF, UL, UB, DR, DF, DL, DB, BR, FR, FL, BL},
  [Y2] = {UB, UR, UF, UL, DB, DR, DF, DL, BL, BR, FR, FL},
  [Y3] = {UL, UB, UR, UF, DL, DB, DR, DF, FL, BL, BR, FR},
  [Z]  = {FL, DL, BL, UL, FR, DR, BR, UR, UF, DF, DB, UB},
  [Z2] = {DF, DR, DB, DL, UF, UR, UB, UL, FL, FR, BR, BL},
  [Z3] = {FR, UR, BR, DR, FL, UL, BL, DL, DF, UF, UB, DB},

  [NULLMOVE] = {UF, UL, UB, UR, DF, DL, DB, DR, FR, FL, BL, BR},
};

int corner_cycle[NULLMOVE+1][8] = {
  [U]  = {UBR, UFR, UFL, UBL, DFR, DFL, DBL, DBR},
  [U2] = {UBL, UBR, UFR, UFL, DFR, DFL, DBL, DBR}, 
  [U3] = {UFL, UBL, UBR, UFR, DFR, DFL, DBL, DBR}, 
  [D]  = {UFR, UFL, UBL, UBR, DFL, DBL, DBR, DFR},
  [D2] = {UFR, UFL, UBL, UBR, DBL, DBR, DFR, DFL},
  [D3] = {UFR, UFL, UBL, UBR, DBR, DFR, DFL, DBL},
  [R]  = {DFR, UFL, UBL, UFR, DBR, DFL, DBL, UBR},
  [R2] = {DBR, UFL, UBL, DFR, UBR, DFL, DBL, UFR},
  [R3] = {UBR, UFL, UBL, DBR, UFR, DFL, DBL, DFR},
  [L]  = {UFR, UBL, DBL, UBR, DFR, UFL, DFL, DBR},
  [L2] = {UFR, DBL, DFL, UBR, DFR, UBL, UFL, DBR},
  [L3] = {UFR, DFL, UFL, UBR, DFR, DBL, UBL, DBR},
  [F]  = {UFL, DFL, UBL, UBR, UFR, DFR, DBL, DBR},
  [F2] = {DFL, DFR, UBL, UBR, UFL, UFR, DBL, DBR},
  [F3] = {DFR, UFR, UBL, UBR, DFL, UFL, DBL, DBR},
  [B]  = {UFR, UFL, UBR, DBR, DFR, DFL, UBL, DBL},
  [B2] = {UFR, UFL, DBR, DBL, DFR, DFL, UBR, UBL},
  [B3] = {UFR, UFL, DBL, UBL, DFR, DFL, DBR, UBR},

  [Uw]  = {UBR, UFR, UFL, UBL, DFR, DFL, DBL, DBR},
  [Uw2] = {UBL, UBR, UFR, UFL, DFR, DFL, DBL, DBR}, 
  [Uw3] = {UFL, UBL, UBR, UFR, DFR, DFL, DBL, DBR}, 
  [Dw]  = {UFR, UFL, UBL, UBR, DFL, DBL, DBR, DFR},
  [Dw2] = {UFR, UFL, UBL, UBR, DBL, DBR, DFR, DFL},
  [Dw3] = {UFR, UFL, UBL, UBR, DBR, DFR, DFL, DBL},
  [Rw]  = {DFR, UFL, UBL, UFR, DBR, DFL, DBL, UBR},
  [Rw2] = {DBR, UFL, UBL, DFR, UBR, DFL, DBL, UFR},
  [Rw3] = {UBR, UFL, UBL, DBR, UFR, DFL, DBL, DFR},
  [Lw]  = {UFR, UBL, DBL, UBR, DFR, UFL, DFL, DBR},
  [Lw2] = {UFR, DBL, DFL, UBR, DFR, UBL, UFL, DBR},
  [Lw3] = {UFR, DFL, UFL, UBR, DFR, DBL, UBL, DBR},
  [Fw]  = {UFL, DFL, UBL, UBR, UFR, DFR, DBL, DBR},
  [Fw2] = {DFL, DFR, UBL, UBR, UFL, UFR, DBL, DBR},
  [Fw3] = {DFR, UFR, UBL, UBR, DFL, UFL, DBL, DBR},
  [Bw]  = {UFR, UFL, UBR, DBR, DFR, DFL, UBL, DBL},
  [Bw2] = {UFR, UFL, DBR, DBL, DFR, DFL, UBR, UBL},
  [Bw3] = {UFR, UFL, DBL, UBL, DFR, DFL, DBR, UBR},

  [M]  = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [M2] = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [M3] = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [S]  = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [S2] = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [S3] = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [E]  = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [E2] = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
  [E3] = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},

  [X]  = {DFR, DFL, UFL, UFR, DBR, DBL, UBL, UBR},
  [X2] = {DBR, DBL, DFL, DFR, UBR, UBL, UFL, UFR},
  [X3] = {UBR, UBL, DBL, DBR, UFR, UFL, DFL, DFR},
  [Y]  = {UBR, UFR, UFL, UBL, DBR, DFR, DFL, DBL},
  [Y2] = {UBL, UBR, UFR, UFL, DBL, DBR, DFR, DFL},
  [Y3] = {UFL, UBL, UBR, UFR, DFL, DBL, DBR, DFR},
  [Z]  = {UFL, DFL, DBL, UBL, UFR, DFR, DBR, UBR},
  [Z2] = {DFL, DFR, DBR, DBL, UFL, UFR, UBR, UBL},
  [Z3] = {DFR, UFR, UBR, DBR, DFL, UFL, UBL, DBL},

  [NULLMOVE] = {UFR, UFL, UBL, UBR, DFR, DFL, DBL, DBR},
};

int center_cycle[NULLMOVE+1][6] = {
  [U]  = {U_center, D_center, R_center, L_center, F_center, B_center},
  [U2] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [U3] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [D]  = {U_center, D_center, R_center, L_center, F_center, B_center},
  [D2] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [D3] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [R]  = {U_center, D_center, R_center, L_center, F_center, B_center},
  [R2] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [R3] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [L]  = {U_center, D_center, R_center, L_center, F_center, B_center},
  [L2] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [L3] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [F]  = {U_center, D_center, R_center, L_center, F_center, B_center},
  [F2] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [F3] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [B]  = {U_center, D_center, R_center, L_center, F_center, B_center},
  [B2] = {U_center, D_center, R_center, L_center, F_center, B_center},
  [B3] = {U_center, D_center, R_center, L_center, F_center, B_center},

  [Uw]  = {U_center, D_center, B_center, F_center, R_center, L_center},
  [Uw2] = {U_center, D_center, L_center, R_center, B_center, F_center},
  [Uw3] = {U_center, D_center, F_center, B_center, L_center, R_center},
  [Dw]  = {U_center, D_center, F_center, B_center, L_center, R_center},
  [Dw2] = {U_center, D_center, L_center, R_center, B_center, F_center},
  [Dw3] = {U_center, D_center, B_center, F_center, R_center, L_center},
  [Rw]  = {F_center, B_center, R_center, L_center, D_center, U_center},
  [Rw2] = {D_center, U_center, R_center, L_center, B_center, F_center},
  [Rw3] = {B_center, F_center, R_center, L_center, U_center, D_center},
  [Lw]  = {B_center, F_center, R_center, L_center, U_center, D_center},
  [Lw2] = {D_center, U_center, R_center, L_center, B_center, F_center},
  [Lw3] = {F_center, B_center, R_center, L_center, D_center, U_center},
  [Fw]  = {L_center, R_center, U_center, D_center, F_center, B_center},
  [Fw2] = {D_center, U_center, L_center, R_center, F_center, B_center},
  [Fw3] = {R_center, L_center, D_center, U_center, F_center, B_center},
  [Bw]  = {R_center, L_center, D_center, U_center, F_center, B_center},
  [Bw2] = {D_center, U_center, L_center, R_center, F_center, B_center},
  [Bw3] = {L_center, R_center, U_center, D_center, F_center, B_center},

  [X]  = {F_center, B_center, R_center, L_center, D_center, U_center},
  [X2] = {D_center, U_center, R_center, L_center, B_center, F_center},
  [X3] = {B_center, F_center, R_center, L_center, U_center, D_center},
  [Y]  = {U_center, D_center, B_center, F_center, R_center, L_center},
  [Y2] = {U_center, D_center, L_center, R_center, B_center, F_center},
  [Y3] = {U_center, D_center, F_center, B_center, L_center, R_center},
  [Z]  = {L_center, R_center, U_center, D_center, F_center, B_center},
  [Z2] = {D_center, U_center, L_center, R_center, F_center, B_center},
  [Z3] = {R_center, L_center, D_center, U_center, F_center, B_center},

  [M]  = {B_center, F_center, R_center, L_center, U_center, D_center},
  [M2] = {D_center, U_center, R_center, L_center, B_center, F_center},
  [M3] = {F_center, B_center, R_center, L_center, D_center, U_center},
  [E]  = {U_center, D_center, F_center, B_center, L_center, R_center},
  [E2] = {U_center, D_center, L_center, R_center, B_center, F_center},
  [E3] = {U_center, D_center, B_center, F_center, R_center, L_center},
  [S]  = {L_center, R_center, U_center, D_center, F_center, B_center},
  [S2] = {D_center, U_center, L_center, R_center, F_center, B_center},
  [S3] = {R_center, L_center, D_center, U_center, F_center, B_center},

  [NULLMOVE] = {U_center, D_center, R_center, L_center, F_center, B_center},
};

