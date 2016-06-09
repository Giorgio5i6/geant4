//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
#ifndef G4NUCLEI_MODEL_HH
#define G4NUCLEI_MODEL_HH

//#include "G4InuclNuclei.hh"

#ifndef G4INUCL_ELEMENTARY_PARTICLE_HH
#include "G4InuclElementaryParticle.hh"
#endif
#include "G4CascadParticle.hh"
#include "G4InuclSpecialFunctions.hh"
#include "G4CascadSpecialFunctions.hh"
#include "G4ElementaryParticleCollider.hh"

#include "G4CascadeKplusPChannel.hh"
#include "G4CascadeKplusNChannel.hh"
#include "G4CascadeKminusPChannel.hh"
#include "G4CascadeKminusNChannel.hh"
#include "G4CascadeKzeroPChannel.hh"
#include "G4CascadeKzeroNChannel.hh"
#include "G4CascadeKzeroBarPChannel.hh"
#include "G4CascadeKzeroBarNChannel.hh"
#include "G4CascadeLambdaPChannel.hh"
#include "G4CascadeLambdaNChannel.hh"
#include "G4CascadeSigmaPlusPChannel.hh"
#include "G4CascadeSigmaPlusNChannel.hh"
#include "G4CascadeSigmaZeroPChannel.hh"
#include "G4CascadeSigmaZeroNChannel.hh"
#include "G4CascadeSigmaMinusPChannel.hh"
#include "G4CascadeSigmaMinusNChannel.hh"
#include "G4CascadeXiZeroPChannel.hh"
#include "G4CascadeXiZeroNChannel.hh"
#include "G4CascadeXiMinusPChannel.hh"
#include "G4CascadeXiMinusNChannel.hh"

#include <vector>

class G4InuclNuclei;

using namespace G4InuclSpecialFunctions;
using namespace G4CascadSpecialFunctions;

typedef std::pair<G4InuclElementaryParticle, G4double> partner;
typedef std::vector<partner> partners;

class G4NucleiModel {

public:

  G4NucleiModel();

  G4NucleiModel(G4InuclNuclei* nuclei) {

    generateModel(nuclei->getA(), nuclei->getZ());
  };

  void generateModel(G4double a, 
		     G4double z);

  void reset() {

    neutronNumberCurrent = neutronNumber;
    protonNumberCurrent = protonNumber;
  };

  void printModel() const; 

  G4double getDensity(G4int ip, 
		      G4int izone) const {

    return nucleon_densities[ip - 1][izone];
  };

  G4double getFermiMomentum(G4int ip, 
			    G4int izone) const {

    return fermi_momenta[ip - 1][izone];
  };

  G4double getFermiKinetic(G4int ip, 
			   G4int izone) const {

    G4double ekin = 0.0;

    if(ip < 3 && izone < number_of_zones) {
      G4double pf = fermi_momenta[ip - 1][izone]; 
      G4double mass = ip == 1 ? 0.93827 : 0.93957;

      ekin = std::sqrt(pf * pf + mass * mass) - mass;
    };  

    return ekin;
  };

  G4double getPotential(G4int ip, 
			G4int izone) const {

    G4int ip0 = ip < 3 ? ip - 1 : 2;
    if (ip > 10 && ip < 18) ip0 = 3;
    if (ip > 20) ip0 = 4;

    return izone < number_of_zones ? zone_potentials[ip0][izone] : 0.0;
  };

  std::vector<G4CascadParticle> 
  generateParticleFate(G4CascadParticle& cparticle,
		       G4ElementaryParticleCollider* theElementaryParticleCollider); 

  G4double getNumberOfNeutrons() const { 

    return neutronNumberCurrent; 
  };

  G4double getNumberOfProtons() const { 

    return protonNumberCurrent; 
  };

  G4bool empty() const { 

    return neutronNumberCurrent < 1.0 && protonNumberCurrent < 1.0; 
  };

  G4bool stillInside(const G4CascadParticle& cparticle) {

    return cparticle.getCurrentZone() < number_of_zones;
  };

  G4CascadParticle initializeCascad(G4InuclElementaryParticle* particle);

  std::pair<std::vector<G4CascadParticle>, std::vector<G4InuclElementaryParticle> > initializeCascad(G4InuclNuclei* bullet, G4InuclNuclei* target);

  std::pair<G4int, G4int> getTypesOfNucleonsInvolved() const {
    return std::pair<G4int, G4int>(current_nucl1, current_nucl2);
  };
  G4bool worthToPropagate(const G4CascadParticle& cparticle) const; 
    
  G4InuclElementaryParticle generateNucleon(G4int type, 
					    G4int zone) const;

private: 
G4int verboseLevel;
  G4bool passFermi(const std::vector<G4InuclElementaryParticle>& particles, 
		   G4int zone);

  void boundaryTransition(G4CascadParticle& cparticle);

  G4InuclElementaryParticle generateQuasiDeutron(G4int type1, 
						 G4int type2,
						 G4int zone) const;

  partners generateInteractionPartners(G4CascadParticle& cparticle) const;

  G4double volNumInt(G4double r1, 
		     G4double r2, 
		     G4double cu, 
		     G4double d1) const; 

  G4double volNumInt1(G4double r1, 
		      G4double r2, 
		      G4double cu2) const; 

  G4double getRatio(G4int ip) const;

  std::vector<std::vector<G4double> > nucleon_densities;

  std::vector<std::vector<G4double> > zone_potentials;

  std::vector<std::vector<G4double> > fermi_momenta;

  std::vector<G4double> zone_radii;

  std::vector<G4double> binding_energies;

  G4double nuclei_radius;

  G4int number_of_zones;

  G4double A;

  G4double Z;

  G4double neutronNumber;

  G4double protonNumber;

  G4double neutronNumberCurrent;

  G4double protonNumberCurrent;

  G4int current_nucl1;

  G4int current_nucl2;

  G4CascadeKplusPChannel kpp;
  G4CascadeKplusNChannel kpn;
  G4CascadeKminusPChannel kmp;
  G4CascadeKminusNChannel kmn;
  G4CascadeKzeroPChannel k0p;
  G4CascadeKzeroNChannel k0n;
  G4CascadeKzeroBarPChannel k0bp;
  G4CascadeKzeroBarNChannel k0bn;
  G4CascadeLambdaPChannel lp;
  G4CascadeLambdaNChannel ln;
  G4CascadeSigmaPlusPChannel spp;
  G4CascadeSigmaPlusNChannel spn;
  G4CascadeSigmaZeroPChannel s0p;
  G4CascadeSigmaZeroNChannel s0n;
  G4CascadeSigmaMinusPChannel smp;
  G4CascadeSigmaMinusNChannel smn;
  G4CascadeXiZeroPChannel x0p;
  G4CascadeXiZeroNChannel x0n;
  G4CascadeXiMinusPChannel xmp;
  G4CascadeXiMinusNChannel xmn;

};        

#endif // G4NUCLEI_MODEL_HH 