##Step 1:

	* In bash (run.sh):

		- Run each simulation with the same seed and number of tasks

		- Redirect the output of each simulation to a different .txt file

##Step 2:

	* In python3 (analyze.py):

		- Go through each text file and find the EXIT lines from the simulations

		- Parse the wait time and turn around time with re

		- Calculate the average wait and turn around time for each simulation

		- Re-format the data and save it in wait and turn around .csv's for plotting

##Step 3:

	* In bash with gnuplot (run.sh):

	- plot wait and turn around time based on data created in python

	- save the plots as .pdf's
