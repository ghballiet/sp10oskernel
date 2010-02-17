find . -name \*.[ch] -print > srcfiles
wc -l `cat srcfiles`
rm srcfiles
