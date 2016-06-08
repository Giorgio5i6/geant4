// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4Nucleon.cc,v 1.1 1998/08/22 08:58:10 hpw Exp $
// GEANT4 tag $Name: geant4-00 $
//
#include "G4Nucleon.hh"

// ------------------------------------------------------------
//      GEANT 4 class implementation file
//
//      For information related to this code contact:
//      CERN, CN Division, ASD group
//      ---------------- G4Nucleon ----------------
//             by Gunter Folger, May 1998.
//       class for a nucleon (inside a 3D Nucleus)
// ------------------------------------------------------------

G4Nucleon::G4Nucleon()
: theBindingE(0.) , theParticleType(NULL), theSplitableHadron(NULL)
{}

G4Nucleon::~G4Nucleon()
{
}

void G4Nucleon::Boost(const G4LorentzVector & aMomentum)
{

//   see e.g. CERNLIB short writeup U101 for the algorithm

	G4double mass=aMomentum.mag();
	G4double factor=
	    ( theMomentum.vect()*aMomentum.vect()/(aMomentum.e()+mass) - theMomentum.e() ) / mass;

	theMomentum.setE(1/mass*theMomentum.dot(aMomentum));
	theMomentum.setVect(factor*aMomentum.vect() + theMomentum.vect());
}