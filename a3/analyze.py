import re, sys

seed = 0
tasks = 1000

def parse_line(line):
	match = re.findall("\d+\.\d+", line)
	return match[0], match[1]

def main():

	results = []

	for quantum in [50,100,250,500]:
		wait_avg = 0;
		turn_avg = 0;
		for overhead in [0,5,10,15,20,25]:
			try:
				file_name = 'results_q' + str(quantum) + '_d' + str(overhead) + '.txt'
				file = open(file_name, 'r')
				for line in file:
					if "EXIT" in line:
						wait, turn = parse_line(line)
						wait_avg += wait
						turn_avg += turn
				print('average: ' + str(wait_avg) + ' ' + str(turn_avg)) 
				
			except:
				print("file " + file_name + " not Found", file=sys.stderr)


if __name__ == '__main__':
	main()