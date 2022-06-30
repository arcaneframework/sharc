#!/bin/bash

#SBATCH --wckey my_project_key
#SBATCH -J my_super_job_name
#SBATCH -N 1
#SBATCH --time=01:00:00
#SBATCH --exclusive
#SBATCH --switches=1@48:00:00

RUN_TAG=my_tag_`date +%d_%m_%y-%R`-${SLURM_JOB_ID}
OUTDIR=output-${RUN_TAG}
OUTFILE=${RUN_TAG}.log

rm -f $OUTFILE
touch $OUTFILE

export SCRATCHDIR=/ifpengpfs/scratch/work/r11/${USER}


source /ifpengpfs/scratch/work/r11/commonlib/toolchain.sh
source /ifpengpfs/scratch/work/r11/commonlib/arcuser.sh
module load Python/2.7.15-bare

export SCRIPT_PATH=path/to/this_script
cd $SRCIPT_PATH
python run_tests.py



