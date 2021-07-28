project('mcc', 'c',
    default_options: [ 'buildtype=release',
                       'c_std=c17',
                       'warning_level=3' ],
    meson_version: '>=0.44.0'
)

# --------------------------------------------------------------------- Library

mcc_inc = include_directories('include')

mcc_def = [ '-D_POSIX_C_SOURCE=200809L' ]

mcc_src = [ 'src/ast.c',
            'src/ast_print.c',
            'src/ast_visit.c',
            'src/parser.c',
            'src/lexer.c' ]

mcc_lib = library('mcc', mcc_src,
                  c_args: mcc_def,
                  include_directories: [mcc_inc, include_directories('src')])

# ---------------------------------------------------------------- Applications

mcc_apps = [ 'mcc', 'mc_ast_to_dot', 'mc_lex' ]

foreach app : mcc_apps
    executable(app, 'app/' + app + '.c',
               c_args: mcc_def,
               include_directories: mcc_inc,
               link_with: mcc_lib)
endforeach

# ----------------------------------------------------------------------- Tests

mcc_tests = [ 'parser_test' ]

cutest_inc = include_directories('vendor/cutest')

foreach test : mcc_tests
    t = executable(test, 'test/unit/' + test + '.c', 'vendor/cutest/CuTest.c',
                   c_args: mcc_def,
                   include_directories: [mcc_inc, cutest_inc],
                   link_with: mcc_lib)
    test(test, t)
endforeach
