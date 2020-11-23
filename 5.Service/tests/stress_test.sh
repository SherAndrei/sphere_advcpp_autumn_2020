#!/bin/bash
calc_client="./5.Service/tests/calc_test.sh"

for ((n = 1; n < 10; n++)) do
    gnome-terminal -e $calc_client
done
    