#include "DD4hep/Version.h"
#include "DDG4/Geant4SensDetAction.inl"
#include "DDG4/Geant4EventAction.h"
#include "DDG4/Geant4Mapping.h"
#include "G4VProcess.hh"
#include "G4Neutron.hh"

#if DD4HEP_VERSION_GE(1, 21)
#define GEANT4_CONST_STEP const
#else
#define GEANT4_CONST_STEP
#endif

/// Namespace for the AIDA detector description toolkit
namespace dd4hep {

  /// Namespace for the Geant4 based simulation part of the AIDA detector description toolkit
  namespace sim   {

    /**
     *  Geant4SensitiveAction<NeutronSDdata> sensitive detector for dummy neutron detector
     *  \author  A. Tolosa-Delgado
     *  \version 1.0
     *  \ingroup DD4HEP_SIMULATION
     */
    struct NeutronSDdata {


      //typedef Geant4HitCollection HitCollection;
      Geant4Sensitive*  sensitive{nullptr};

      // Setup a static property of this Sensitive Detector
      // dynamic properties (which may change on event basis) can
      // also be defined here
      G4double fNeutronEnergyThreshold{0};

      // Setup pointers/IDs to access several collections during the event
      Geant4HitCollection * fHitCollection{nullptr};
      G4int fMainParticleCollectionID{-1};

      Geant4HitCollection * fNeutralParticleCollection{nullptr};
      G4int fNeutralParticleCollectionID{-1};

      Geant4HitCollection * fChargedParticleCollection{nullptr};
      G4int fChargedParticleCollectionID{-1};

      /// Post-event action callback, useful for combining hits, etc
      /// See TrackerCombine case in DDG4/plugins/Geant4SDActions.cpp:571
      void endEvent(const G4Event* /* event */){}

      // In case it is needed to reset the state of the object
      void clear(){}


      /// Method for generating hit(s) using the information of G4Step object.
      G4bool process(G4Step GEANT4_CONST_STEP * step, G4TouchableHistory* )
{
      G4Track * track =  step->GetTrack();
      typedef Geant4Tracker::Hit Hit;
      Geant4StepHandler h(step);
      Position prePos    = h.prePos();
      Position postPos   = h.postPos();
      Position direction = postPos - prePos;
      Position position  = mean_direction(prePos,postPos);
      double   hit_len   = direction.R();

      Hit* hit = new Hit(h.trkID(), h.trkPdgID(), h.deposit(), h.track->GetGlobalTime());
      HitContribution contrib = Hit::extractContribution(step);
      hit->cellID        = this->sensitive->cellID(step);
      hit->energyDeposit = contrib.deposit;
      hit->position      = position;
      hit->momentum      = 0.5*( h. preMom() + h.postMom() ) ;
      hit->length        = hit_len;

      if (track->GetDefinition() == G4Neutron::Definition()) {
        track->SetTrackStatus(fStopAndKill);
      }
      sensitive->collectionByID(fMainParticleCollectionID)->add(hit);

      // check if NeutralParticle
      bool IsNeutralParticle = fabs(track->GetDefinition()->GetPDGCharge()) < 0.01;
      if( IsNeutralParticle )
          fNeutralParticleCollection->add(hit);
      else
        sensitive->collection(fChargedParticleCollectionID)->add(hit);



    /// Mark the track to be kept for MC truth propagation during hit processing
      sensitive->mark(h.track);

      sensitive->printM2("Hit with deposit:%f  Pos:%f %f %f ID=%016X",
            step->GetTotalEnergyDeposit(),position.X(),position.Y(),position.Z(),
            (void*)hit->cellID);
      Geant4TouchableHandler handler(step);
      sensitive->printM2("    Geant4 path:%s",handler.path().c_str());
      return true;
    }

    };

// // // // // // // // // // // // // // //
// //     specialization of Geant4SensitiveAction<> class


    /// Initialization overload for specialization
    template <> void Geant4SensitiveAction<NeutronSDdata>::initialize() {
      eventAction().callAtEnd(&m_userData,&NeutronSDdata::endEvent);

      m_userData.fNeutronEnergyThreshold = m_sensitive.energyCutoff();
      m_userData.sensitive = this;

      //IDDescriptor dsc = m_sensitive.idSpec() ;

    }

    /// Define collections created by this sensitivie action object
    template <> void Geant4SensitiveAction<NeutronSDdata>::defineCollections() {
      m_collectionID = defineCollection<Geant4Tracker::Hit>(m_sensitive.readout().name());
      m_userData.fMainParticleCollectionID=m_collectionID;
      m_userData.fNeutralParticleCollectionID = defineCollection<Geant4Tracker::Hit>("NeutralParticleCollection");
      m_userData.fChargedParticleCollectionID = defineCollection<Geant4Tracker::Hit>("ChargedParticleCollection");
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> void Geant4SensitiveAction<NeutronSDdata>::clear(G4HCofThisEvent*) {
      m_userData.clear();
    }

    /// Method for generating hit(s) using the information of G4Step object.
    template <> G4bool
    Geant4SensitiveAction<NeutronSDdata>::process(G4Step GEANT4_CONST_STEP * step, G4TouchableHistory* history) {
      return m_userData.process(step, history);
    }

    typedef Geant4SensitiveAction<NeutronSDdata>  NeutronSDAction;

  }// end sim namespace
}// end dd4hep namespace

// Register the SDAction into DDG4 plugin system
#include "DDG4/Factories.h"
DECLARE_GEANT4SENSITIVE( NeutronSDAction )
