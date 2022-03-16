# ProtoDUNE pion-Ar simulation study

## Purpose

To study interaction between charged pion and Ar nuclear in ProtoDUNE (PD).

## Example of Running

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