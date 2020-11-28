#!/bin/bash
client="./debug/6.HTTP/tests/httpclient"

for ((n = 1; n <= 100; n++))
    do echo "=========== take $n ============="
       sleep 0.2
       ${client}
done