find . -name \*.[chs] -print > srcfiles
wc -l `cat srcfiles`
rm srcfiles
