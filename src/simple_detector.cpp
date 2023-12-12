#include "DD4hep/DetFactoryHelper.h"

using namespace dd4hep;

// create the detector
static Ref_t createDetector(Detector &desc, xml::Handle_t handle, SensitiveDetector sens)
{
  xml::DetElement detElem = handle;

  // Get detector name and ID from compact file
  std::string detName = detElem.nameStr();
  int detID = detElem.id();

  // the handle provides direct access to material name field
  auto sensorMaterialName = detElem.attr<std::string>(_Unicode(material));;
  // materialStr function returns empty string if no material is defined
  // in order to intercept the error in python, we throw an exception
  if( sensorMaterialName.empty() )
    throw std::runtime_error("Detector " + detName + " requires material attribute!");

  // the handle also provides direct access to dimensions field
  xml::Component dims = detElem.dimensions();
  double size_x = dims.attr<double>(_Unicode(x));
  double size_y = dims.attr<double>(_Unicode(y));
  double size_z = dims.attr<double>(_Unicode(z));


  // In general, single fields may be accessed with attr function:
  auto zpos = detElem.attr<double>(_Unicode(zposition));

  // subsections of the detector can be accessed as this:
  // auto vesselElem      = detElem.child(_Unicode(vessel));
  // auto vesselVisStr    = vesselElem.attr<std::string>(_Unicode(vis)); // or using visStr
  // auto vesselMatStr    = vesselElem.attr<std::string>(_Unicode(material)); // or using materialStr
  // auto vesselThickness = vesselElem.attr<std::string>(_Unicode(thickness));

  // Setup which kind of sensitive detector is
  sens.setType("tracker");

  // Create the mother Detector element to be returned at the end of the function
  DetElement det(detName, detID);

  // How to define one detector:
  // Define geometrical shape
  Box sensSolid( size_x / 2.,
                 size_y / 2.,
                 size_z / 2.);

  // Define volume (shape+material)
  auto sensorMaterial = desc.material(sensorMaterialName);
  Volume sensVol(detName +"_sensor", sensSolid, sensorMaterial);
  //setup visual attr to the sensor volume
  auto sensorVis = desc.visAttributes(detElem.visStr());
  sensVol.setVisAttributes( sensorVis );

  // Associate this volume with the sensitive detector machinery
  sensVol.setSensitiveDetector(sens);


  // Place our mother volume in the world
  Volume wVol = desc.pickMotherVolume(det);

  PlacedVolume sensorPV = wVol.placeVolume(sensVol, Position(0, 0, zpos));

  // Assign the system ID to our mother volume
  sensorPV.addPhysVolID("system", detID);

  // Associate the silicon Placed Volume to the detector element.
  det.setPlacement(sensorPV);

  return det;
}

DECLARE_DETELEMENT(MYCUBE_T, createDetector)
