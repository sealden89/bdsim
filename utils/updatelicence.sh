#! /bin/bash
# check for licence file
LICENCEFILE="LICENCE.txt"
[ ! -f "$LICENCEFILE" ] && echo "$LICENCEFILE is missing. Abort." && exit 1

# if we have an old licence file, remove the length of that licence only (ie not source code)
if [ -f "OLDLICENCE.txt" ]; then
    REMOVENLINES=$(wc -l < "OLDLICENCE.txt")
else
    REMOVENLINES=$(wc -l < $LICENCEFILE)
fi
# inrement by 2 to include /* */
REMOVENLINES=$((REMOVENLINES+2))

# for detecting the licence
FIRSTLINE="Beam Delivery Simulation (BDSIM) Copyright (C)"


function remove_copyright {
    echo "Removing first ${REMOVENLINES} lines from $1"
    sed -i '' 1,"${REMOVENLINES}"d  $1
}

function add_copyright {
    if [[ $1 == Makefile ]]; then
	[ "$(head -c46 $1)" == "$FIRSTLINE" ] && continue
	NEWFILE="${1}.new"
	[ "${1#*.}" == "new" ] && continue
	[ -f "$NEWFILE" ] && echo "Sorry, $NEWFILE already exists" && continue
	while read line; do
            echo "# $line" >> "$NEWFILE"
	done < "$LICENCEFILE"
	cat "$1" >> "$NEWFILE"
    else
	[ "$(head -c46 $1)" == "$FIRSTLINE" ] && continue
	NEWFILE="${1}.new"
	[ -f "$NEWFILE" ] && echo "Sorry, $NEWFILE already exists" && continue
	echo "/* " > "$NEWFILE"
	cat "$LICENCEFILE" >> "$NEWFILE"
	echo "*/" >> "$NEWFILE"
	cat "$1" >> "$NEWFILE"
    fi
}

# development test
#if grep -q 'Copyright' src/BDSAcceleratorComponent.cc; then
#    remove_copyright src/BDSAcceleratorComponent.cc
#fi
#add_copyright src/BDSAcceleratorComponent.cc
#mv src/BDSAcceleratorComponent.cc.new src/BDSAcceleratorComponent.cc

echo "Inserting "$LICENCEFILE" into .cc and .hh files."
#for file in $(find . -name "*.h" -or -name "*.cc"); do
for file in $(find . -not -path "*src-external*" -name "*.h" -or -name "*.cc" -or -name "*.hh" -or -name "*.y" -or -name "*.l"); do
    echo "Working on file $file"
    if grep -q 'Copyright' "$file"; then
        remove_copyright "$file"
    fi
    add_copyright "$file"
done

echo "Moving new files into place."
for file in $(find . -name "*.new"); do
    mv "$file" "${file%.new}";
done
