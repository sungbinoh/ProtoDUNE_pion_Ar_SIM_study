# Definition of residual mass

We reconstruct 4-vector of residual nuclei using beam, outgoing ${\pi}^{+}$ and proton.  
```
${X}^{'}$ = beam - Ar_target(at rest) - ${\pi}^{+}$ - proton
```

## Residual mass distribution before smearing

![](residual_mass_no_smear.png)

We observe Z = 17 background (Cl with different atomic mass) at just above signal events.  
So, we can expect that overlap between signal and background after smearing will be dominated by Z = 17 background.

## Residual mass distribution after smearing

![](residual_mass_with_smear.png)

We observe exactly what we expected

### How many additional neutrons?

There should be additional neutrons for events in Z = 17 background.  
We want to check,  

*   How many neutrons at truth level after scattering?
*   Which cut value on residual mass will give good signal purity?

<img src="../../plots/GEANT4/M_residual_Nn_SR1_All_36.200_36.400.png"  width="45%"  /> <img src="../../plots/FLUKA/M_residual_Nn_SR1_All_36.200_36.400.png" width="45%"/>
<img src="../../plots/GEANT4/M_residual_Nn_SR1_All_36.230_36.310.png"  width="45%"  /> <img src="../../plots/FLUKA/M_residual_Nn_SR1_All_36.230_36.310.png" width="45%"/>
<img src="../../plots/GEANT4/M_residual_Nn_SR1_All_36.230_36.280.png"  width="45%"  /> <img src="../../plots/FLUKA/M_residual_Nn_SR1_All_36.230_36.280.png" width="45%"/>
<img src="../../plots/GEANT4/M_residual_Nn_SR1_All_36.230_36.255.png"  width="45%"  /> <img src="../../plots/FLUKA/M_residual_Nn_SR1_All_36.230_36.255.png" width="45%"/>

We observed that GEANT4 shows more signal events.
Both generators show that background events are dominated by events coming from Z = 17 background.

### Neutron momentum

To understand possibility of Z = 17 background rejection using neutron tagging, we need to check momentum of neutrons.

*   Plots before applying mX cut.

<img src="../../plots/GEANT4/P_neutron_1st_SR1_All.png "  width="45%"  /> <img src="../../plots/FLUKA/P_neutron_1st_SR1_All.png" width="45%"/>
<img src="../../plots/GEANT4/P_neutron_2nd_SR1_All.png "  width="45%"  /> <img src="../../plots/FLUKA/P_neutron_2nd_SR1_All.png" width="45%"/>
<img src="../../plots/GEANT4/P_neutron_3rd_SR1_All.png "  width="45%"  /> <img src="../../plots/FLUKA/P_neutron_3rd_SR1_All.png" width="45%"/>

Most of the neutrons have momentum smaller than 150 MeV.

*   Plost after applying mX < 36.1 GeV cut.

<img src="../../plots/GEANT4/P_neutron_1st_SR1_All_mXcut.png "  width="45%"  /> <img src="../../plots/FLUKA/P_neutron_1st_SR1_All_mXcut.png" width="45%"/>
<img src="../../plots/GEANT4/P_neutron_2nd_SR1_All_mXcut.png "  width="45%"  /> <img src="../../plots/FLUKA/P_neutron_2nd_SR1_All_mXcut.png" width="45%"/>
<img src="../../plots/GEANT4/P_neutron_3rd_SR1_All_mXcut.png "  width="45%"  /> <img src="../../plots/FLUKA/P_neutron_3rd_SR1_All_mXcut.png" width="45%"/>

Long tails are removed.