# ======================================================================
#  larg4 main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larg4/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(larg4 VERSION 09.07.02 LANGUAGES CXX)

find_package(cetbuildtools REQUIRED)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)

cet_report_compiler_flags()

find_package(cetlib_except REQUIRED PUBLIC)
find_package(cetlib REQUIRED PUBLIC)
find_package(fhiclcpp REQUIRED PUBLIC)
find_package(messagefacility REQUIRED PUBLIC)
find_package(canvas REQUIRED PUBLIC)
find_package(canvas_root_io REQUIRED)
find_package(art REQUIRED PUBLIC)
find_package(art_root_io REQUIRED PUBLIC)
find_package(nusimdata REQUIRED PUBLIC)
find_package(FFTW3 REQUIRED PUBLIC)
find_package(artg4tk REQUIRED)
find_package(Range-v3 REQUIRED PUBLIC)
find_package(nug4 REQUIRED PUBLIC)
find_package(nurandom REQUIRED PUBLIC)
find_package(CLHEP COMPONENTS Vector REQUIRED PUBLIC)
find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(Geant4 REQUIRED PUBLIC)
find_package(ROOT COMPONENTS Core Tree Hist Physics REQUIRED PUBLIC)

find_ups_product(larcore)
find_ups_product(larcoreobj)
find_ups_product(larcorealg)
find_ups_product(lardataalg)
find_ups_product(lardataobj)

include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# allow the use of #include "Geant4/.."
include_directories("${Geant4_INCLUDE_DIRS}/../")

add_subdirectory(fcl)
add_subdirectory(gdml)
add_subdirectory(larg4)
add_subdirectory(macros)
add_subdirectory(ups)

# packaging utility
# this has to be last
include(UseCPack)
