#!/bin/bash
client="./debug/6.HTTP/tests/keep_alive_zombie"

for ((n = 1; n <= 100; n++))
    do echo "=========== take $n ============="
       sleep 0.01
       ${client}
done