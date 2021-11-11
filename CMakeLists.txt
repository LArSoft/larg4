# ======================================================================
#  larg4 main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larg4/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(larg4 VERSION 09.06.01 LANGUAGES CXX)

find_package(cetbuildtools REQUIRED)

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules $ENV{ART_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)

cet_report_compiler_flags()

find_ups_product(art)
find_ups_product(art_root_io)
find_package(artg4tk REQUIRED)
find_ups_product(canvas)
find_ups_product(canvas_root_io)
find_ups_product(cetlib)
find_ups_product(clhep)
find_ups_product(fhiclcpp)
find_ups_geant4()
find_ups_product(larcore)
find_ups_product(larcoreobj)
find_ups_product(larcorealg)
find_ups_product(lardataalg)
find_ups_product(lardataobj)
find_ups_product(messagefacility)
find_ups_product(nusimdata)
find_ups_product(nug4)
find_ups_product(nurandom)
find_package(Range REQUIRED)

include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

add_subdirectory(fcl)
add_subdirectory(gdml)
add_subdirectory(larg4)
add_subdirectory(macros)
add_subdirectory(ups)

# packaging utility
# this has to be last
include(UseCPack)
