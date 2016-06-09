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
 // Hadronic Process: Ion Inelastic Process
 // J.P. Wellisch, CERN, Apr. 14 2000
 // Last modified: 03-Apr-1997

#ifndef G4PositronNuclearProcess_h
#define G4PositronNuclearProcess_h 1
 
// Class Description
// Process for positron nuclear inelastic scattering; 
// to be used in your physics list in case you need this physics.
// Class Description - End

#include "G4HadronInelasticProcess.hh"
#include "G4Positron.hh"
#include "G4ElectroNuclearCrossSection.hh"
 

 class G4PositronNuclearProcess : public G4HadronInelasticProcess
 {
 public:
    
    G4PositronNuclearProcess(
     const G4String& processName = "PositronNuclear" ) :

    G4HadronInelasticProcess( processName, G4Positron::Positron() )
    { 
      G4CrossSectionDataStore * theStore = GetCrossSectionDataStore();
      theStore->AddDataSet(&theData);
    } 
    
    ~G4PositronNuclearProcess()
    { }
    
 private:
 
   G4ElectroNuclearCrossSection theData;
 };

#endif
