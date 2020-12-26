#!/bin/bash
calc_client="./5.Service/tests/calc_test.sh"

for ((n = 1; n <= 20; n++)) do
    $calc_client &
done
    