#ifndef COMMANDS_H
#define COMMANDS_H

#include <time.h>

#include "solve.h"
#include "solver_nxopt31.h"
#include "threader_single.h"
#include "threader_eager.h"

void                    free_args(CommandArgs *args);
CommandArgs *           new_args();

/* Arg parsing functions *****************************************************/

CommandArgs *           gen_parse_args(int c, char **v);
CommandArgs *           help_parse_args(int c, char **v);
CommandArgs *           parse_only_scramble(int c, char **v);
CommandArgs *           parse_no_arg(int c, char **v);
CommandArgs *           solve_parse_args(int c, char **v);

/* Exec functions ************************************************************/

void                    gen_exec(CommandArgs *args);
void                    solve_exec(CommandArgs *args);
void                    help_exec(CommandArgs *args);
void                    quit_exec(CommandArgs *args);

/* Commands ******************************************************************/

#ifndef COMMANDS_C

extern Command gen_cmd;
extern Command help_cmd;
extern Command quit_cmd;
extern Command solve_cmd;

extern Command *commands[];

#else

Command
solve_cmd = {
	.name        = "solve",
	.usage       = "solve [OPTIONS] SCRAMBLE",
	.description = "Solve the cube",
	.parse_args  = solve_parse_args,
	.exec        = solve_exec
};

Command
gen_cmd = {
	.name        = "gen",
	.usage       = "gen [-t N]",
	.description = "Generate all tables [using N threads]",
	.parse_args  = gen_parse_args,
	.exec        = gen_exec
};

Command
help_cmd = {
	.name        = "help",
	.usage       = "help [COMMAND]",
	.description = "Display nissy manual page or help on specific command",
	.parse_args  = help_parse_args,
	.exec        = help_exec,
};

Command
quit_cmd = {
	.name        = "quit",
	.usage       = "quit",
	.description = "Quit nissy",
	.parse_args  = parse_no_arg,
	.exec        = quit_exec,
};

Command *commands[] = {
	&gen_cmd,
	&help_cmd,
	&quit_cmd,
	&solve_cmd,
	NULL
};

#endif

#endif
