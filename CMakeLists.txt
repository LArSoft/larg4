# ======================================================================
#  larg4 main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larg4/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

find_package(cetmodules 3.10.00 REQUIRED) # For FindGeant4.cmake
project(larg4 VERSION 09.16.04 LANGUAGES CXX)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)

cet_report_compiler_flags(REPORT_THRESHOLD VERBOSE)

find_package(larcore REQUIRED EXPORT)
find_package(larcorealg REQUIRED EXPORT)
find_package(lardataalg REQUIRED EXPORT)
find_package(lardataobj REQUIRED EXPORT)

find_package(artg4tk REQUIRED EXPORT)

find_package(nug4 REQUIRED EXPORT)
find_package(nusimdata REQUIRED EXPORT)
find_package(nurandom REQUIRED EXPORT)

find_package(art_root_io REQUIRED EXPORT)
find_package(art REQUIRED EXPORT)
find_package(canvas_root_io REQUIRED)
find_package(canvas REQUIRED EXPORT)
find_package(messagefacility REQUIRED EXPORT)
find_package(fhiclcpp REQUIRED EXPORT)
find_package(cetlib REQUIRED EXPORT)
find_package(cetlib_except REQUIRED EXPORT)

find_package(CLHEP COMPONENTS Random Vector REQUIRED EXPORT)
find_package(Geant4 REQUIRED EXPORT)
find_package(ROOT COMPONENTS Core EG Tree Hist Physics RIO REQUIRED EXPORT)
find_package(Range-v3 REQUIRED EXPORT)

include(ArtDictionary)
include(CetMake)
include(BuildPlugins)

add_subdirectory(fcl)
add_subdirectory(gdml)
add_subdirectory(larg4)
add_subdirectory(macros)
add_subdirectory(test)

# packaging utility
# this has to be last
cet_cmake_config()
