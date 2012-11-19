
# Test driver for arum
# Author : Sisinty Sasmita Patra

import popen2
import shlex

fr = open('testcases_arum.txt', 'r')

testcases = fr.readlines()
fr.close()

# For each test case
for testcase in testcases:

	# Parsing the test_name and expected outputs
	a = shlex.split(testcase);
	test_name = a[0];
	expected_usertime = float(a[1]);

	# Preparing command in the form of : "./Arum TestCase". E.g: "./Arum Permutation"
	command = './Arum ' + test_name;

	fout, fin = popen2.popen2(command)
	fin.close();

	output_lines = fout.readlines();
	fout.close();
	
	for output_line in output_lines:
		if 'User time' in output_line:
			u = shlex.split(output_line);
			usertime  = float(u[3]);
			
	# Validating output and expected output
	if ((usertime - expected_usertime) ** 2) < 0.001:
                print test_name, " Passed."
	else:
		print test_name, " Failed."
		print "\tTest result: user time: ", usertime
		print "\t    Expected user time: ", expected_usertime


		




		
