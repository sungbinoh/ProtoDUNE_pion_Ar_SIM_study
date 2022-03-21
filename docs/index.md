## Research log of pion-Ar scattering

You can use the [package](https://github.com/sungbinoh/ProtoDUNE_pion_Ar_SIM_study/) to analyize GEANT4/FLUKA simulated root files.

For research log, please check above menu bar.

### Interested processes

Signal region (SR) 1 : ${\pi}^{+} + {}^{40} Ar \rightarrow {\pi}^{+} + p + {}^{39}Cl$  
SR2 : ${\pi}^{+} + {}^{40} Ar \rightarrow {\pi}^{+} + p + p + {}^{38}S$  
SR3 : ${\pi}^{+} + {}^{40} Ar \rightarrow {\pi}^{0} + p + {}^{39}Ar$  
SR4 : ${\pi}^{+} + {}^{40} Ar \rightarrow {\pi}^{-} + p + p + {}^{38}Cl$  

### Applied cuts on truth level particles
* For signal particles
  * Proton : P > 0.3 GeV
  * ${\pi}^{+}$ : P > 0.1 GeV
  * ${\pi}^{0}$ : full acceptance
* Background particles
  * ${\pi}^{-}$, Kaon, and muon : P > 0.1 GeV

### Event selection

Exact number of pions and protons.  
Allowing any number of gammas.  
No background particles.

### Smearing

* Momentum
  * ${\pi}^{+}$ : 2%
  * ${\pi}^{0}$ : 5%
  * proton : 2%
* Angle
  * 5% for $\theta$ and $\phi$


