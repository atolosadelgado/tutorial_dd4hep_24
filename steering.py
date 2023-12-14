#steering.py
# ddsim --compactFile ./compact/simple_detector.xml --runType batch --steeringFile steering.py
from DDSim.DD4hepSimulation import DD4hepSimulation
SIM = DD4hepSimulation()

## if there is a user provided SDAction which needs additional parameters these can be passed as a dictionary
SIM.action.mapActions["MY_FIRST_CUBE"] = ( "NeutronSDAction", {"NeutronEnergyThreshold": "1*GeV"} )

SIM.filter.tracker = "edep0"

# Particle gun settings
SIM.numberOfEvents = 10
SIM.enableGun = True
SIM.gun.energy = "1*GeV"
SIM.gun.particle = "neutron"
SIM.gun.direction = "0 0 -1"
SIM.gun.multiplicity = 1
SIM.gun.position = "0 0 1*cm"

