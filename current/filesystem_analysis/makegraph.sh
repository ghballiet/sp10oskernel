
echo "This will take a while, because I am doing find for every file from /"
echo "If you don't run this as root, you may have a lot of errors."

HOSTNAME=`hostname`

gcc -o makebars makebars.c

find / -type f -exec ls -al \{\} \;  | awk '{ print $5 }' | grep -v "^0$" | sort -n | makebars > data.$HOSTNAME

sed -e "s/<<MACHINE_NAME>>/$HOSTNAME/g" makeplot.src > makeplot.gnuplot

gnuplot makeplot.gnuplot


