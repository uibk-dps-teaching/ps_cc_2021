# Development Notes

## Adding new Files

New files are *not* automatically added to the build system.
You have to add them to the corresponding list inside `meson.build`.

## Unit Test Listings

Due to the simplicity of the unit testing framework one has to list all test cases composing a test suite.
This is done by using [X-Macros](https://en.wikipedia.org/wiki/X_Macro).
See bottom of `parser_test.c`.
Extend this list when adding new tests.

In order to add new test suites, copy `parser_test.c` and replace the contained tests.
Add the new test suite to the corresponding list in `meson.build`.

## Test Wrappers

Meson provides a `--wrapper` flag to run tests through arbitrary programs, for instance:

    meson test --wrapper 'valgrind --error-exitcode=1 --leak-check=full'

Full output can be obtained by also passing `--verbose`.

If you encounter segfaults happening at random, catch them by repeating unit tests multiple times with GDB attached:

    meson test --repeat 1000000 --gdb

## Coverage

Taken from the [Meson Documentation](https://mesonbuild.com/Unit-tests.html#coverage):

> If you enable coverage measurements by giving Meson the command line flag `-Db_coverage=true`, you can generate coverage reports.
> Meson will autodetect what coverage generator tools you have installed and will generate the corresponding targets.
> These targets are `coverage-xml` and `coverage-text` which are both provided by Gcovr and `coverage-html`, which requires Lcov and GenHTML or Gcovr with html support.

## Printing and Debugging

An AST printer for the [Dot Format](https://en.wikipedia.org/wiki/DOT_(graph_description_language)) is provided.
Together with [Graphviz](https://graphviz.gitlab.io/), ASTs can be visualised.

    ./mc_ast_to_dot ../test/integration/fib/fib.mc | xdot -

or

    ./mc_ast_to_dot ../test/integration/fib/fib.mc | dot -Tpng > fib_ast.png
    xdg-open fib_ast.png

## `mcc` Stub

A stub for the mC compiler is provided to ease infrastructure development.
It can already be used with the integration test runner.

    MCC=../scripts/mcc_stub ../scripts/run_integration_tests
