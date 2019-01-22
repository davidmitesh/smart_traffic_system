import sys

a=5;
f= open("guru99.txt","w+")
f.write(str(a));
f.close();
sys.stdout.write(str(a));
