#!/bin/bash
for m in sphere 
do
	for s in 28k 212k 1m 24m
	do
		for c in tet hex
		do
			for f in msh
			do
				echo "ANNEX unit_${m}_${c}_${s}.$f"
				file=unit_${m}_${c}_${s}.$f
				git annex addurl --file=${file} https://zenodo.org/records/22978340/files/${file}?download=1/${file}?download=1
			done
		done
	done
done

for m in sphere 
do
	for s in 28k 212k 1m 24m
	do
		for c in tet
		do
			for f in meshb
			do
				echo "ANNEX unit_${m}_${c}_${s}.$f"
				file=unit_${m}_${s}.$f
				git annex addurl --file=${file} https://zenodo.org/records/17954653/files/${file}?download=1/${file}?download=1/${file}?download=1
			done
		done
	done
done
