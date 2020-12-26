#!/bin/bash
client="./debug/5.Service/tests/client"

for ((n = 1; n <= 100; n++))
    do echo "=========== take $n ============="
       sleep 0.01
       ${client}
done