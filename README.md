# ProtoDUNE pion-Ar simulation study

## Purpose

To study interaction between charged pion and Ar nuclear in ProtoDUNE (PD).

## Example of Running

Clone this repository
```
git clone git@github.com:sungbinoh/ProtoDUNE_pion_Ar_SIM_study.git
```

Compile
```
source setup.sh
make
```

To test
```
source run_debug.sh
```

To run whole root files for a sample
```
python python/PDStudy.py -a PionAnalyzer -i Piplus_1GeV_Ar -sim GEANT4 -q fastq -n 10 &
```

## Example of email report
```
#### Job Info ####
HOST = TAMSA2
JobID = 399747
Analyzer = PionAnalyzer
Simulator = GEANT4
# of Jobs = 10
InputSample = ['Piplus_1GeV_Ar']
Output sent to : /data6/Users/suoh/PDStudyOutput//Run1_v1/PionAnalyzer/GEANT4/
##################
Job started at 2022-03-16 11:03:06
Job finished at 2022-03-16 11:05:47
```

## Research log

[Link](https://sungbinoh.github.io/ProtoDUNE_pion_Ar_SIM_study/)