import re, sys, os

seed = 0
tasks = 1200

def print_data(results):
	
	data_w = 	[	
				[0,-1,-1,-1,-1],
				[5,-1,-1,-1,-1],
				[10,-1,-1,-1,-1],
				[15,-1,-1,-1,-1],
				[20,-1,-1,-1,-1],
				[25,-1,-1,-1,-1]
			 	]

	data_t =	[	
				[0,-1,-1,-1,-1],
				[5,-1,-1,-1,-1],
				[10,-1,-1,-1,-1],
				[15,-1,-1,-1,-1],
				[20,-1,-1,-1,-1],
				[25,-1,-1,-1,-1]
			 	]

	row = 0
	col = 1

	for point in results:
		data_w[row][col] = point[0]
		data_t[row][col] = point[1]

		row = (row +1) % 6
		col = col+1 if (row == 0) else col

	wait_time = open('results/wait_time.dat', 'w+')
	turn_time = open('results/turn_around.dat', 'w+')

	wait_time.write('# rrsim wait time data\n')
	wait_time.write('# ----------------------------------------------\n')

	turn_time.write('# rrsim turn around time data\n')
	turn_time.write('# ----------------------------------------------\n')

	for val in data_w:
		wait_time.write('%d, %.2f, %.2f, %.2f, %.2f\n' % (val[0], val[1], val[2], val[3], val[4]))

	for val in data_t:
		turn_time.write('%d, %.2f, %.2f, %.2f, %.2f\n' % (val[0], val[1], val[2], val[3], val[4]))

def parse_line(line):
	match = re.findall("\d+\.\d+", line)
	return match[0], match[1]

def main():

	results = []
	for quantum in [50,100,250,500]:
		for overhead in [0,5,10,15,20,25]:
			file_name = 'results/q{}_d{}.txt'.format(quantum, overhead)
			file = open(file_name, 'r')

			wait_avg = 0
			turn_avg = 0
			for line in file:
				if 'EXIT' in line:
					wait, turn = parse_line(line)
					wait_avg += float(wait)
					turn_avg += float(turn)

			#close(file_name)

			wait_avg = wait_avg / tasks
			turn_avg = turn_avg / tasks

			results.append([wait_avg, turn_avg])

	print_data(results)

if __name__ == '__main__':
	main()