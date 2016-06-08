//
//

#ifndef G4E1Probability001_hh
#define G4E1Probability001_hh

#include "globals.hh"
#include "G4VEmissionProbability.hh"
#include "G4Fragment.hh"
#include "G4VLevelDensityParameter.hh"

class G4E1Probability001 : public G4VEmissionProbability
{

public:

  G4E1Probability001() {};

  ~G4E1Probability001();

  G4double EmissionProbability(const G4Fragment& frag, const G4double excite);
  G4double EmissionProbDensity(const G4Fragment& frag, const G4double ePhoton);

private:

  // G4E1Probability001() {};

  G4E1Probability001(const G4E1Probability001& right);

  const G4E1Probability001& operator=(const G4E1Probability001& right);
  G4bool operator==(const G4E1Probability001& right) const;
  G4bool operator!=(const G4E1Probability001& right) const;

  // Integrator (simple Gaussian quadrature)

  G4double EmissionIntegration(const G4Fragment& frag, const G4double excite,
                               const G4double lowLim, const G4double upLim,
                               const G4int numIters);

  // G4VLevelDensityParameter* _levelDensity; // Don't need this

};

#endif