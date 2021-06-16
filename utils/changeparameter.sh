#!/bin/bash

#change a single parameter in a bdsim options file

display_usage() {
    echo ""
    echo "Usage: "
    echo "changeparameter [gmad file] [parameter name] [new value]"
    echo "will normally edit file unless -n used at end"
    echo "changeparameter [gmad file] [parameter name] [new value] -n [newfilename]"
}

#check correct number of parameters
if [ $# -lt 3 ]; then
echo "too few commands"
display_usage
fi
if [ $# -gt 3 ] && [ $# -lt 5 ]; then
echo "wrong number of parameters"
display_usage
exit 0
fi

#test if input file exists
if [[ ! -e $1 ]]; then
echo "ERROR: file $1 doesn't exist"
display_usage
exit 0
fi

#test to see if parameter string is in file - exact match
if ! grep -q $2 $1; then
echo "ERROR: file does not contain parameter $2"
display_usage
exit 0
fi

#get the last character - whether it's a "," or ";"
the_parameter_line=$(grep $2 $1 | tail -c 5)
if [[ $the_parameter_line == *,* ]]; then
last_character=','
else
last_character=';'
fi

#do the replacement
#either inline or in new file
if [[ $4 == "-n" ]]; then
cat $1 | sed "/.*$2.*/ s/=.*/= $3$last_character/" > $5
else
sed -ie "/.*$2.*/ s/=.*/= $3$last_character/" $1
fi


