#!/bin/sh

set -e

for arg in $*
do
	for prog in 1proc pthread fork
	do
		for i in $(seq 0 1 100); do
			./bin/$prog "$arg" $i >> "./results/${arg}proc-${prog}.txt"
		done
	done
done

exit 0

