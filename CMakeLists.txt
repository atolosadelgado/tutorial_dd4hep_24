cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

#---------------------------
set( PackageName tutorial_dd4hep_24 )
#---------------------------

project(${PackageName})

find_package( DD4hep REQUIRED COMPONENTS DDCore DDRec DDG4 DDParsers )

find_package( ROOT REQUIRED COMPONENTS Geom GenVector )

find_package( Geant4 REQUIRED )

file(GLOB sources ./src/*.cpp )

add_dd4hep_plugin(${PackageName} SHARED ${sources})
target_link_libraries(${PackageName}   DD4hep::DDCore DD4hep::DDRec DD4hep::DDParsers ${ROOT_LIBRARIES})
