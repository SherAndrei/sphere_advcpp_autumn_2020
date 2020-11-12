client="./5.Service/tests/client"

for ((n = 1; n < 50; n++))
    do echo "=========== take $n ============="
       ${client}
done