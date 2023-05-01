# Refactoring

## Init functions

* All .h files should have a single init function.
* This function should initialize everything that this module needs, including
  calling the init functions of the modules it depends on.
* To avoid multiple initialization of the same module, each should have a
  static bool initialized variable.
* Everything that a module needs should be initialized by init(), avoid
  initializing stuff when solving. Exception: pruning tables, move tables.
* Most functions should generate some tables and save them to disk.
* Init functions should have a consistent structure (e.g. the way they check
  if the tables are already generated should be the same).

## Cube types

* Get rid of cubetype.h, split type definitionss into the other modules.
* Every type definition should be in the most fundamental module that needs it.

## Code style

* Use goto for error handling (if needed)
* Headers: blank line between <> and "" includes
* Remove variable names from prototypes (debatable).
* Stop declaring all variables at the beginning of a function (debatable).
* Rename functions and variable to have a consistent naming scheme.
* Sort functions: prototypes in logical blocks, then alphabetic.
  Implementations in the same order as the prototypes.
* Order of variable declarations (in files, functions and structs):
  Size first (large to small), then alphabetic.
* Indent: make second level indent consistent to 4 spaces.
  Try to avoid long statements.
* Avoid include statements in .h files, use ifdef guards in .c files
  (to reconsider as part of the redesign).
* Functions that copy data: swap src and dest, follow memcpy standard.

## Generic

* Avoid malloc and free/new function pointers for cube objects,
  use array of char on stack.
