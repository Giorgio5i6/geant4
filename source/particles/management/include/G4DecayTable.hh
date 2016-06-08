// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4DecayTable.hh,v 2.1 1998/07/13 17:17:50 urbi Exp $
// GEANT4 tag $Name: geant4-00 $
//
//
// ------------------------------------------------------------
//      GEANT 4 class header file
//
//      For information related to this code contact:
//      CERN, CN Division, ASD group
//      History: first implementation, based on object model of
//      7 July 1996 H.Kurashige
//      14 June 1997 H.Kurashige
// ------------------------------------------------------------

#ifndef G4DecayTable_h
#define G4DecayTable_h 1

#include "G4ios.hh"
#include <rw/tpsrtvec.h>
#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4VDecayChannel.hh"

class G4DecayTable 
{
 public:
   typedef RWTPtrSortedVector<G4VDecayChannel> G4VDecayChannelVector;

  //constructors
 public:
    G4DecayTable();
    ~G4DecayTable();

 private:
    G4DecayTable(const G4DecayTable &){};
    G4DecayTable & operator=(const G4DecayTable &){return *this;};

 public:
    // equality operators
    G4int operator==(const G4DecayTable &right) const {return (this == &right);};
    G4int operator!=(const G4DecayTable &right) const {return (this != &right);};

 public:
    void  Insert( G4VDecayChannel* aChannel);
    G4int entries() const;

 public:
    G4VDecayChannel* SelectADecayChannel();
    G4VDecayChannel* GetDecayChannel(G4int index) const;
    G4VDecayChannel* operator[](G4int index);
    void DumpInfo() const;

 private:
    G4ParticleDefinition       *parent;
    G4VDecayChannelVector       *channels;
};

inline     
 G4int G4DecayTable::entries() const
{
  return channels->entries();
}

inline     
 G4VDecayChannel* G4DecayTable::operator[](G4int index)
{
  return (*channels)(index);
}

inline     
 G4VDecayChannel* G4DecayTable::GetDecayChannel(G4int index) const
{
  G4VDecayChannel* selectedChannel = NULL;
  if ( (index>=0) && (index<channels->entries()) ){
    selectedChannel = (*channels)(index);
  }
  return selectedChannel;
}
 
 
#endif