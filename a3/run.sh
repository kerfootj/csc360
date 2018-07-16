#!/bin/bash

SEED="1800"
TASKS="1200"

mkdir -p results

echo "quantum 50"

./simgen $TASKS $SEED | ./rrsim --quantum 50 --dispatch 0 > results/q50_d0.txt
./simgen $TASKS $SEED | ./rrsim --quantum 50 --dispatch 5 > results/q50_d5.txt
./simgen $TASKS $SEED | ./rrsim --quantum 50 --dispatch 10 > results/q50_d10.txt
./simgen $TASKS $SEED | ./rrsim --quantum 50 --dispatch 15 > results/q50_d15.txt
./simgen $TASKS $SEED | ./rrsim --quantum 50 --dispatch 20 > results/q50_d20.txt
./simgen $TASKS $SEED | ./rrsim --quantum 50 --dispatch 25 > results/q50_d25.txt

echo "quantum 100" 

./simgen $TASKS $SEED | ./rrsim --quantum 100 --dispatch 0 > results/q100_d0.txt
./simgen $TASKS $SEED | ./rrsim --quantum 100 --dispatch 5 > results/q100_d5.txt
./simgen $TASKS $SEED | ./rrsim --quantum 100 --dispatch 10 > results/q100_d10.txt
./simgen $TASKS $SEED | ./rrsim --quantum 100 --dispatch 15 > results/q100_d15.txt
./simgen $TASKS $SEED | ./rrsim --quantum 100 --dispatch 20 > results/q100_d20.txt
./simgen $TASKS $SEED | ./rrsim --quantum 100 --dispatch 25 > results/q100_d25.txt

echo "quantum 250"

./simgen $TASKS $SEED | ./rrsim --quantum 250 --dispatch 0 > results/q250_d0.txt
./simgen $TASKS $SEED | ./rrsim --quantum 250 --dispatch 5 > results/q250_d5.txt
./simgen $TASKS $SEED | ./rrsim --quantum 250 --dispatch 10 > results/q250_d10.txt
./simgen $TASKS $SEED | ./rrsim --quantum 250 --dispatch 15 > results/q250_d15.txt
./simgen $TASKS $SEED | ./rrsim --quantum 250 --dispatch 20 > results/q250_d20.txt
./simgen $TASKS $SEED | ./rrsim --quantum 250 --dispatch 25 > results/q250_d25.txt

echo quantum "500"

./simgen $TASKS $SEED | ./rrsim --quantum 500 --dispatch 0 > results/q500_d0.txt
./simgen $TASKS $SEED | ./rrsim --quantum 500 --dispatch 5 > results/q500_d5.txt
./simgen $TASKS $SEED | ./rrsim --quantum 500 --dispatch 10 > results/q500_d10.txt
./simgen $TASKS $SEED | ./rrsim --quantum 500 --dispatch 15 > results/q500_d15.txt
./simgen $TASKS $SEED | ./rrsim --quantum 500 --dispatch 20 > results/q500_d20.txt
./simgen $TASKS $SEED | ./rrsim --quantum 500 --dispatch 25 > results/q500_d25.txt
