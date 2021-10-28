## current version

g++ -o afs afs.cpp -lstdc++fs

our source code should be compiled by gcc that support C++ 17.

the minimium version of GCC is 7.0.0.

## v2

g++ -o conf conf.cpp -I. -L. -lapt-pkg -lapt-private
