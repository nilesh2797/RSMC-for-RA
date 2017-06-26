#!/bin/bash
for filename in litmus-tests/*.rsmc; do
	echo "$filename";
	TIMEFORMAT='%lU';
    time ./a.out < "$filename";
done