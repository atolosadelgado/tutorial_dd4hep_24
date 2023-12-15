#steering.py
# ddsim --compactFile ./compact/simple_detector.xml --runType batch --steeringFile steering.py
from DDSim.DD4hepSimulation import DD4hepSimulation
SIM = DD4hepSimulation()

## if there is a user provided SDAction which needs additional parameters these can be passed as a dictionary
SIM.action.mapActions["MY_FIRST_CUBE"] = "mySDaction"

SIM.filter.tracker = ""
SIM.filter.calo    = ""
SIM.filter.filters = {}

# Particle gun settings
SIM.numberOfEvents = 10
SIM.enableGun = True
SIM.gun.energy = "100*GeV"
SIM.gun.particle = "proton"
SIM.gun.direction = "0 0 -1"
SIM.gun.multiplicity = 1
SIM.gun.position = "0 0 1*cm"

SIM.random.enableEventSeed = True
SIM.random.file = None
SIM.random.luxury = 1
SIM.random.replace_gRandom = True
SIM.random.seed = 1
SIM.random.type = None
