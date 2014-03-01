# This is in python because CMake has problems processing text files when there are variable names in them

import sys
with open(sys.argv[3], 'w') as outfile:
	outfile.write('var '+sys.argv[1]+'=(function(){\n')
	with open(sys.argv[2]) as infile:
		for line in infile:
			outfile.write(line)
	outfile.write('return Module;})();')
