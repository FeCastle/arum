
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
	expected_systemtime = float(a[2]);
	expected_maxrss = int(a[3]);
	expected_minflt = int(a[4]);
	expected_majflt = int(a[5]);
	expected_inblock = int(a[6]);
	expected_outblock = int(a[7]);

	# Preparing command in the form of : "./Arum TestCase". E.g: "./Arum Permutation"
	command = './Arum -r ' + test_name;

	fout, fin, ferror = popen2.popen3(command)
	fin.close();

	output_lines = fout.readlines();
	fout.close();
	
	for output_line in output_lines:
		if 'User time' in output_line:
			u = shlex.split(output_line);
			usertime  = float(u[2]);

		if 'System time' in output_line:
                        u = shlex.split(output_line);
                        systemtime  = float(u[2]);
		
		if 'maxrss' in output_line:
                        u = shlex.split(output_line);
                        maxrss  = int(u[6]);

		if 'minflt' in output_line:
                        u = shlex.split(output_line);
                        minflt  = int(u[6]);

		if 'majflt' in output_line:
                        u = shlex.split(output_line);
                        majflt  = int(u[6]);

		if 'inblock' in output_line:
                        u = shlex.split(output_line);
                        inblock  = int(u[5]);

		if 'oublock' in output_line:
                        u = shlex.split(output_line);
                        outblock  = int(u[5]);	

	# Validating output and expected output

	TestPassed = 1;

	if ((usertime - expected_usertime) ** 2) > 0.01:
		Testpassed = 0;
	if ((systemtime - expected_systemtime) ** 2) > 0.01:
		TestPassed = 0;
	if abs(maxrss - expected_maxrss) > 10:
		TestPassed = 0;
	if abs(minflt - expected_minflt) > 5:
		TestPassed = 0;
	if abs(majflt - expected_majflt) > 2:
        	TestPassed = 0;
	if abs(inblock - expected_inblock) > 2:
        	TestPassed = 0;
	if abs(outblock - expected_outblock) > 2:
        	TestPassed = 0;

	if(TestPassed):
                print test_name, " Passed."
	else:
		print test_name, " Failed."
		print "\tTest result: user time: ", usertime
		print "\t    Expected user time: ", expected_usertime
		print "\t           System time: ", systemtime
		print "\t  Expected system time: ", expected_systemtime
		print "\t                Maxrss: ", maxrss
		print "\t       Expected Maxrss: ", expected_maxrss
		print "\t                Minflt: ", minflt
                print "\t       Expected minflt: ", expected_minflt
		print "\t                Majflt: ", majflt
                print "\t       Expected Majflt: ", expected_majflt
		print "\t               Inblock: ", inblock
                print "\t      Expected Inblock: ", expected_inblock
		print "\t              outblock: ", outblock
                print "\t     Expected outblock: ", expected_outblock
		




		
