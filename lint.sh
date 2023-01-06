#!/usr/bin/env bash

# echo "Running autopep..."
# find -type f -name '*.py' ! -path '*/migrations/*' -exec autopep8 -r -v -v -v -a -a --experimental --in-place '{}' \;

echo "Running yapf"
find -type f -name '*.py' -exec yapf -i -r --style pep8 -vv '{}' \;



echo "Running pycodestyle..."
find -type f -name '*.py' ! -exec pycodestyle --first '{}' \;

echo "Running pylint..."
find -type f -name '*.py' ! -exec pylint '{}' \;
