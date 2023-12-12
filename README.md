#HOW TO DD4hep
==============

## Compile project

Source the paths to the commands of DD4hep and its dependencies, for example, from cvmfs

```shell
source /cvmfs/sft.cern.ch/lcg/views/dev4/latest/x86_64-centos7-gcc11-opt/setup.sh 
```

To compile,
```shell
cmake -B build -S . -D CMAKE_INSTALL_PREFIX=install
cmake --build build -- install
```

After that, the install directory will look like this:

```shell
$ tree install/
install/
└── lib
    ├── libsimple_example.components
    └── libsimple_example.so

1 directory, 2 files
```

To let the programs know where to find our freshly built detector, we have to update this env variable

```shell
export LD_LIBRARY_PATH=$PWD/install/lib:$LD_LIBRARY_PATH
```

## Things to do after every change

Compile and install after every modification of the c++ detector constructor code.

```bash
cmake --build build -- install
```

## Visualize and debug geometry

To display the geometry

```bash
geoDisplay ./compact/simple_detector.xml
```

To convert the geometry into ROOT

```bash
./dd4hep2root -c  ./compact/simple_detector.xml -o arc_v0.root
```


To show materials in along a given line (in this case from origin to (0,0,-100)cm), check if the modified volume has the proper material

```bash
materialScan ./compact/simple_detector.xml 0 0 -30 0 0 30
```

Possible issue if `USE_GEANT4_UNITS` enabled when building DD4hep.

Check overlaps, using Geant4 check
```shell
ddsim --compactFile ./compact/simple_detector.xml --runType run --part.userParticleHandler='' --macroFile overlap.mac >> overlapDump.txt
```

## Run simulation

To run the simulation of 0.27402359GeV alpha particle, we have to provide total energy (mass+kinetic) so 4GeV total:

```shell
ddsim --compactFile compact/simple_detector.xml --runType qt --enableGun --gun.particle alpha --gun.energy 4*GeV  -N 1 --outputFile example_edm4hep.root --macroFile vis.mac --gun.distribution uniform --gun.isotrop true
```

The track can be something like ![A track of 274.02359 MeV alpha in lead](https://mattermost.web.cern.ch/files/f1tnt4n4n7dabk7zpbnwxbyxdc/public?h=usG4tmiWGuhWBAujICrw-K5bv63s6TR0izjSpG4CvjM)

If we inspect the output rootfile,

```shell
root example_edm4hep.root
```

There is only one entry (1 event), so we can show the contents

```cpp
events->Show(0)
======> EVENT:0
 MCParticles     = (vector<edm4hep::MCParticleData>*)0xbaae4d0
 MCParticles.PDG = 1000020040
 MCParticles.generatorStatus = 1
 MCParticles.simulatorStatus = 150994944
 MCParticles.charge = 2.000000
 MCParticles.time = 0.000000
 MCParticles.mass = 3.72738
 MCParticles.vertex.x = 0
 MCParticles.vertex.y = 0
 MCParticles.vertex.z = 0
 MCParticles.endpoint.x = -0.910603
 MCParticles.endpoint.y = -1.9604
 MCParticles.endpoint.z = 0.601719
 MCParticles.momentum.x = -0.428134
 MCParticles.momentum.y = -0.854188
 MCParticles.momentum.z = 0.295066
 MCParticles.momentumAtEndpoint.x = -0.000000
 MCParticles.momentumAtEndpoint.y = -0.000000
 MCParticles.momentumAtEndpoint.z = 0.000000
 MCParticles.spin.x = 0.000000
 MCParticles.spin.y = 0.000000
 MCParticles.spin.z = 0.000000
 MCParticles.colorFlow.a = 0
 MCParticles.colorFlow.b = 0
 MCParticles.parents_begin = 0
 MCParticles.parents_end = 0
 MCParticles.daughters_begin = 0
 MCParticles.daughters_end = 0
 MY_HITS         = (vector<edm4hep::SimTrackerHitData>*)0xf1fb900
 MY_HITS.cellID  = 1
 MY_HITS.EDep    = 0.131812
 MY_HITS.time    = 0.015610
 MY_HITS.pathLength = 2.246921
 MY_HITS.quality = 0
 MY_HITS.position.x = -0.455302
 MY_HITS.position.y = -0.980202
 MY_HITS.position.z = 0.30086
 MY_HITS.momentum.x = -0.214067
 MY_HITS.momentum.y = -0.427094
 MY_HITS.momentum.z = 0.147533
 MY_HITS#0       = (vector<podio::ObjectID>*)0xe405300
 MY_HITS#0.index = 0
 MY_HITS#0.collectionID = 3
 EventHeader     = (vector<edm4hep::EventHeaderData>*)0xc526b00
 EventHeader.eventNumber = 0
 EventHeader.runNumber = 0
 EventHeader.timeStamp = 1675937276
 EventHeader.weight = 0.000000
 PARAMETERS      = (podio::GenericParameters*)0xfd8e4f0
```

Simulation options, like particle gun and physics lists, can be gathered in a so-called steering file ([one example](https://github.com/atolosadelgado/ARC_detector/blob/f96e1990e40c0b51588464b8c53c2980968ca937/arcsim.py)).
