# Final Submission Evaluation Scheme

The following key is used for calculating the resulting grade:

- **1:** ≥ 90%
- **2:** [80%, 90%)
- **3:** [70%, 80%)
- **4:** [60%, 70%)
- **5:** < 60%

Points will be subtracted for shortcomings discovered during evaluation.
This includes things like:

- Encountered issues not mentioned or justified in the *Known Issues* section
- Executables *segfaulting*
- Faulty code that is not even tested
- Overcomplicated build system setups
- Intermixing interfaces and implementation details
- Misuse of assertions
- Unnecessary waste of time or space (memory leaks)
- Inconsistently formatted or unreadable source code
- …

## Hard Requirements

- README is present:
    - Contains list of prerequisites
    - Contains build instructions
    - Contains *Known Issues* section
- `mcc` executable operates as demanded by the specification.
- A respective executable can be built and run for the *mandelbrot* test input.

## General (10 Points)

This is all about compiling *valid* input programs.

- Provided test inputs (examples) build and run successfully.
- Additional, secret test inputs build and run successfully.

## Front-end (8 Points)

This is all about rejecting *invalid* input programs.

- Invalid input yields a meaningful error message including source location (filename, start line, and start column).
- Syntactically invalid input is rejected by the parser.
- Semantic checks demanded by the specification are implemented and run on the obtained AST.

## Core (2 Points)

The IR needs to be decoupled in order to exploit its benefits.
Furthermore, the control flow graph is an essential tool used by optimising compilers.

- TAC data structure is present and independent from front- and back-end.
- A dedicated CFG data structure is present.
- A CFG of a given IR function can be obtained and visualised.

## Evaluation System

I'll be using a virtualised, updated Ubuntu 20.10 (64 bit) to examine your submissions.
From this you can infer the software versions I'll be using.
The submitted code has to compile and run on this system.
