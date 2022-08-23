#install StaticChecker

requirements: clang version 12.0

mkdir build

cd build

CC=pathto/clang+llvm-12.0.0-x86_64-apple-darwin/bin/clang CXX=pathto/clang+llvm-12.0.0-x86_64-apple-darwin/bin/clang++ cmake -DLLVM_PREFIX=pathto/clang+llvm-12.0.0-x86_64-apple-darwin/ -D CMAKE_BUILD_TYPE=Debug ..

make

#run testing

cd tests/ReturnStackAddrChecker/test1

pathto/clang+llvm-12.0.0-x86_64-apple-darwin/bin/clang -emit-ast test.c

find $(pwd) -name "*.ast" > astList.txt

pathto/StaticChecker/build/tools/TemplateChecker/TemplateChecker astList.txt ../../config.txt

