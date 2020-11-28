#!/bin/bash
client="./debug/6.HTTP/tests/simple"

for ((n = 1; n <= 100; n++))
    do echo "=========== take $n ============="
       sleep 0.1
       ${client}
done