## -- 0 : GEANT4, 1 : FLUKA
root -l -b -q "plotter/Draw_basic_plots.C(0)"
root -l -b -q "plotter/Draw_basic_plots.C(1)"

scp -r plots suoh@147.47.242.44:/home/suoh/Dropbox/pion_study/ProtoDUNE_pion_Ar_SIM_study
