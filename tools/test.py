# This Python script automates testing
# author: Sisinty Sasmita Patra

import popen2
import shlex

# opens the file with reading mode and returns a file handle
fr = open("testcases.txt", 'r') 

# reads all test cases line by line into the array testcases 
testcases = fr.readlines()
fr.close()

for testcase in testcases:
	# split the testcase by space
	a = shlex.split(testcase)
	input = a[0]
	expectedOutput = int(a[1])

	# runs twice program
	fout, fin = popen2.popen2("./twice")

	# fin is standard input handle
	fin.write(a[0])
	fin.close()

	# fout is standard output handle
	output = int(fout.readline()) 
	fout.close()

	if (output == expectedOutput):
		print("Testcase Passed")
	else:
		print("Testcase Failed")


