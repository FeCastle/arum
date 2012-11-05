import popen2
import shlex

fout, fin = popen2.popen2('./Arum Permutation');

fin.close();

lines = fout.readlines();
fout.close();

u = shlex.split(lines[3]);

usertime = float(u[3]);

print usertime;
