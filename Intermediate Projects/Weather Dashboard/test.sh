#!/bin/bash

echo "🧹 Cleaning old files"
rm -f main.o test.o test_program .tests/test-results.json

echo "🔧 Compiling main.cpp"
g++ -c main.cpp -o main.o

echo "🔧 Compiling tests/test.cpp"
g++ -c .tests/test.cpp -o test.o

echo "🔧 Linking object files with wrapped main()"
g++ main.o test.o -Wl,--wrap=main -o test_program

echo "🧪 Running test_program"
./test_program
