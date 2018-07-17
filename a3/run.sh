#!/bin/bash

TASKS=$1
SEED=$2

mkdir -p results
mkdir -p graphs

echo
echo "Running simulation with $TASKS Tasks on Seed: $SEED"
echo 
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

echo "analyzing results"

python3 analyze.py $TASKS

# https://askubuntu.com/questions/701986/how-to-execute-commands-in-gnuplot-using-shell-script
# https://alvinalexander.com/technology/gnuplot-charts-graphs-examples

R1="'results/wait_time.csv'" 
R2="'results/turn_around.csv'"
gnuplot -persist <<-EOFMarker
	
	set terminal pdf
	set output 'graphs/graph_waiting.pdf'

	set title "Round Robin Scheduler- #Tasks: $TASKS, Seed: $SEED" font "Courier Bold,12"
	set xlabel "Dispatch Overhead (ticks)" font "Courier Bold,"
	set ylabel "Average Waiting Time (ticks)" font "Courier Bold,"
	
	plot $R1 u 1:2 with lp title 'q=50', $R1 u 1:3 with lp title 'q=100', $R1 u 1:4 with lp title 'q=250', $R1 u 1:5 with lp title 'q=500'  

	set output 'graphs/graph_turnaround.pdf'
	set ylabel "Average Turn Around Time (ticks)" font "Courier Bold,"
	plot $R1 u 1:2 with lp title 'q=50', $R1 u 1:3 with lp title 'q=100', $R1 u 1:4 with lp title 'q=250', $R1 u 1:5 with lp title 'q=500'

EOFMarker

rm -r results