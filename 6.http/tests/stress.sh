#!/bin/bash
client="./6.HTTP/tests/test_client.sh"

for ((n = 1; n <= 30; n++)) do
    $client &
done