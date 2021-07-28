# Compiler Construction

|    Date    | Topic / Recommended Schedule / Deadlines  |
| ---------- | ----------------------------------------- |
| 2021-03-05 | *Introduction*                            |
| 2021-03-12 | *Explaining lexer & parser code*          |
| 2021-03-19 | Lexer complete                            |
| 2021-03-26 | Team composition                          |
| 2021-04-16 | Parser complete                           |
| 2021-04-23 |                                           |
| 2021-04-30 | Semantic checks complete                  |
| 2021-05-07 |                                           |
| 2021-05-14 | AST → TAC conversion complete             |
| 2021-05-21 |                                           |
| 2021-05-28 | TAC → ASM (no function calls) complete    |
| 2021-06-04 |                                           |
| 2021-06-11 | TAC → ASM (with function calls) complete  |
| 2021-06-18 | CFG generation complete                   |
| 2021-06-25 |                                           |
| 2021-07-16 | Final Submission deadline (no extensions) |

- [mC Compiler Specification](specification.md)
- [Getting Started Code-base](mcc)

## Structure

The ultimate goal of this course is to build a working compiler according to the given specification.

You are not allowed to use code from other people participating in this course or code that has been submitted previously by somebody else.
A *getting started* code-base is provided, but you can also start from scratch.

Please note that minor modifications may be made to the specification until 2 weeks before the final deadline.
Therefore, double check for modifications before submitting — Git provides you the diff anyway.

You may work together in teams of 1–3 people.
Teams may span across proseminar groups.

The proseminar takes place online [over at Twitch](https://www.twitch.tv/AlexWarhawk), attendance is not mandatory.

### Repository

Host your code on the University GitLab and add me as developer to your repository.

Furthermore, it is required to set up GitLab CI to automatically build and test your code.
The getting started code-base comes with a basic [`.gitlab-ci.yml`](mcc/.gitlab-ci.yml).

### Programming Language

Any of the following programming languages can be used:

- modern C (used for the getting started code-base)
- modern C++
- Go
- Rust
- Haskell

Go easy on external dependencies and obscure language extensions — yes, I'm looking at you, Haskell.
Code readability is paramount.
Using overly complex or cryptic concepts may negatively impact the evaluation process — again, looking at you, Haskell and your voodoo magic lenses.

## Submission

### Team Composition

Send me a mail (one per team) with the names of your team members and a link to your repository — even if you work alone.
Don't forget to add me as developer to your repository.

### Feedback Request

Just message me whenever you want to receive feedback on your code.
Tell me which commit to look at.

Ensure that the files do not contain any personal information like names or student IDs as I'll be going over the code on stream.

### Final Submission

Add the tag `final` to the revision you want me to look at for the final evaluation.
Note that tags need to be pushed explicitly.

I'll only evaluate your solution if the corresponding CI job builds and tests your code successfully.
Do not forget to enable integration tests when using the provided CI configuration.

## Grading

The final grade is computed solely from the final submission.

Be sure to adhere to the specification, deviating from it (without giving proper reason) will negatively impact your grade.
See [Final Submission Evaluation Scheme](evaluation_scheme.md) for more details.

## Contacting Me

Just message me on Discord if you need anything.
Alternatively, you can send me a mail using the following link.

📧 [send email](mailto:alexander.hirsch@uibk.ac.at?subject=703602%20-%20)

You can also open an issue on GitLab.
Please message me if I do not respond within 24 hours; I might not have received a notification.
