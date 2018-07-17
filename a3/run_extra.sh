#!/bin/bash

TASKS=$1
SEED=$2

mkdir -p results
mkdir -p graphs

echo
echo "Running simulation with $TASKS Tasks on Seed: $SEED"

./simgen $TASKS $SEED | ./fcfssim --quantum 0 --dispatch 0 > results/q50_d0.txt
./simgen $TASKS $SEED | ./fcfssim --quantum 0 --dispatch 5 > results/q50_d5.txt
./simgen $TASKS $SEED | ./fcfssim --quantum 0 --dispatch 10 > results/q50_d10.txt
./simgen $TASKS $SEED | ./fcfssim --quantum 0 --dispatch 15 > results/q50_d15.txt
./simgen $TASKS $SEED | ./fcfssim --quantum 0 --dispatch 20 > results/q50_d20.txt
./simgen $TASKS $SEED | ./fcfssim --quantum 0 --dispatch 25 > results/q50_d25.txt

echo "analyzing results"

python3 analyze.py $TASKS

# https://askubuntu.com/questions/701986/how-to-execute-commands-in-gnuplot-using-shell-script
# https://alvinalexander.com/technology/gnuplot-charts-graphs-examples

R1="'results/wait_time.csv'" 
R2="'results/turn_around.csv'"
gnuplot -persist <<-EOFMarker
	
	set terminal pdf
	set output 'graphs/graph_waiting_fcfs.pdf'

	set title "Round Robin Scheduler- #Tasks: $TASKS, Seed: $SEED" font "Courier Bold,12"
	set xlabel "Dispatch Overhead (ticks)" font "Courier Bold,"
	set ylabel "Average Waiting Time (ticks)" font "Courier Bold,"
	
	plot $R1 u 1:2 with lp title 'q=N/A'  

	set output 'graphs/graph_turnaround_fcfs.pdf'
	set ylabel "Average Turn Around Time (ticks)" font "Courier Bold,"
	plot $R1 u 1:2 with lp title 'q=N/A

EOFMarker

rm -r results/