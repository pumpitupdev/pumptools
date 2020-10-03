#!/usr/bin/env bash

for i in "$1"/bin/test-*; do
    test="$(basename $i)"

    echo "Running test $test..."

    "$i" || exit 1
done

echo "All tests successful"

exit 0