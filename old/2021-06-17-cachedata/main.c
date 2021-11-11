#include <stdio.h>
#include "cube.h"
#include "steps.h"

int main() {
	Alg *algo;
	AlgList *sols;
	Cube cube;
	SolveOptions opts;
	char line[1000];
	int i, ns = 3; /*, nrand = 100000, sum1, sum2;*/

	Step *stps[20]    = {&eofb_HTM, &corners_URF, &optimal_HTM};
	char sss[30][30] = {"EO on F/B", "Corners URF", "Optimal solution"};

	opts = (SolveOptions) {
		.min_moves     = 0,
		.max_moves     = 20,
		.optimal_only  = true,
		.max_solutions = 1,
		.can_niss      = false,
		.feedback      = true,
		.pre_trans     = uf
	};

	init();

/*
	print_ptable(&pd_corners_HTM);

	srand(time(NULL));
	sum1 = 0;
	sum2 = 0;
	for (i = 0; i < nrand; i++) {
		sum1 += optimal_HTM.check(random_cube());
		sum2 += corners_HTM.check(random_cube());
	}
	printf("Average optimal pruning: %lf\n", ((double)sum1) / ((double) nrand));
	printf("Average corners pruning: %lf\n", ((double)sum2) / ((double) nrand));
*/

	print_cachedata(&cd_optimal_HTM_6, false);

	printf("Welcome to nissy 2.0! Insert a scramble:\n");

	if (fgets(line, 1000, stdin) != NULL) {
		algo = new_alg(line);
		cube = apply_alg(algo, (Cube){0});

		for (i = 0; i < ns; i++) {
			if (stps[i]->check == NULL)
				fprintf(stderr, "Check function for step %d is null\n", i);
			sols = solve(cube, *stps[i], &opts);
			printf("%s: %d solutions found:\n", sss[i], sols->len);
			print_alglist(sols, true);
			free_alglist(sols);
		}
		free(algo);
	}

	return 0;
}
