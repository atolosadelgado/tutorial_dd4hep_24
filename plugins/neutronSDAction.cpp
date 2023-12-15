#include "DD4hep/Version.h"
#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4Mapping.h"
#include "G4VProcess.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4OpticalPhoton.hh"

#if DD4HEP_VERSION_GE(1, 21)
#define GEANT4_CONST_STEP const
#else
#define GEANT4_CONST_STEP
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {


    /// Helper class to define properties of optical trackers. UNTESTED
    struct mySDdata {};

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<mySDdata>::defineCollections()    {
      m_collectionID = declareReadoutFilteredCollection<Geant4Tracker::Hit>();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> bool
    Geant4SensitiveAction<mySDdata>::process(const G4Step* step,G4TouchableHistory* /* hist */) {
      // Note: 1) We store in the hit the hit-direction, which is not the same as the track direction.
      //       2) The energy deposit is the track momentum
      typedef Geant4Tracker::Hit Hit;
      Geant4StepHandler h(step);
      auto      contrib = Hit::extractContribution(step);
      Direction hit_momentum = 0.5 * (h.preMom() + h.postMom());
      double    hit_deposit  = contrib.deposit;
      Hit* hit = new Hit(contrib, hit_momentum, hit_deposit);
      // // check if this track corresponds to Gamma/e+/e-
      // bool IsGamma      = G4Electron::Definition() == track->GetDefinition();
      // bool IsPositron   = G4Positron::Definition() == track->GetDefinition();
      // bool IsElectron   = G4Electron::Definition() == track->GetDefinition();
      // bool IsEMparticle = IsGamma || IsPositron || IsElectron;
      //
      // if ( IsEMparticle ) {
      //   // stop EM shower, deposit energy as EM and kill particle
      //   step->GetTrack()->SetTrackStatus(fStopAndKill);
      // }
      hit->cellID = cellID(step);
      if ( 0 == hit->cellID )  {
        hit->cellID      = volumeID( step ) ;
        except("+++ Invalid CELL ID for hit!");
      }
      collection(m_collectionID)->add(hit);
      mark(h.track);
      print("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
            hit->energyDeposit,hit->position.X(),hit->position.Y(),hit->position.Z(),(void*)hit->cellID);
      Geant4TouchableHandler handler(step);
      print("    Geant4 path:%s",handler.path().c_str());
      return true;
    }


    typedef Geant4SensitiveAction<mySDdata> mySDaction;


  }// end sim namespace
}// end dd4hep namespace

// Register the SDAction into DDG4 plugin system
#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE( mySDaction )
