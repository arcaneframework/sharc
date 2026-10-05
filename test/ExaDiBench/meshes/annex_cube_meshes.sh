#!/bin/bash
for m in cube 
do
	for s in 28k 212k 1m 24m
	do
		for c in tet hex
		do
			for f in msh meshb
			do
				echo "ANNEX unit_${m}_${c}_${s}.$f"
				file=unit_${m}_${c}_${s}.$f
				git annex addurl --file=${file} https://zenodo.org/records/23000296/files/${file}?download=1
			done
		done
	done
done

