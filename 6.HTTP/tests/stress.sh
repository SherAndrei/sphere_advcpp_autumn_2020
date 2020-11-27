#!/bin/bash
client="./6.HTTP/tests/test_client.sh"

for ((n = 1; n < 10; n++)) do
    gnome-terminal --command $client
done