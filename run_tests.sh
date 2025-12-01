#!/bin/bash

mkdir -p output_files

for input in input_files/*.txt; do
    basename=$(basename "$input" .txt)

    echo "Testing $basename..."

    ./bin/interrupts_EP "$input"
    mv execution.txt "output_files/${basename}_EP.txt"

    ./bin/interrupts_RR "$input"
    mv execution.txt "output_files/${basename}_RR.txt"

    ./bin/interrupts_EP_RR "$input"
    mv execution.txt "output_files/${basename}_EP_RR.txt"
done

echo "all tests completed"