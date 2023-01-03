#!/usr/bin/env bash

echo "Running autopep..."
find -type f -name '*.py' ! -path '*/migrations/*' -exec autopep8 -r -v -v -v -a -a --experimental --in-place '{}' \;

echo "Running pycodestyle..."
find -type f -name '*.py' ! -path '*/migrations/*' -exec pycodestyle --first '{}' \;
