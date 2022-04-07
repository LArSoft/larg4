# ======================================================================
#  larg4 main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/larg4/ups/setup_for_development <-d|-p>
#  buildtool [-I /install/path] -bt[i] -jN
# ======================================================================

cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
# cmake_policy(VERSION 3.19) ### MIGRATE-ACTION-COMPATIBILITY (migrate-3.09.03) - Uncomment to preserve compatibility with older CMake versions

find_package(cetmodules REQUIRED)
project(larg4 VERSION 09.10.03 LANGUAGES CXX)


include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)

cet_report_compiler_flags() ### MIGRATE-ACTION-RECOMMENDED (migrate-3.09.03) - add args: REPORT_THRESHOLD VERBOSE

find_package(cetlib_except REQUIRED EXPORT)
find_package(cetlib REQUIRED EXPORT)
find_package(fhiclcpp REQUIRED EXPORT)
find_package(messagefacility REQUIRED EXPORT)
find_package(canvas REQUIRED EXPORT)
find_package(canvas_root_io REQUIRED)
find_package(art REQUIRED EXPORT)
find_package(art_root_io REQUIRED EXPORT)
find_package(nusimdata REQUIRED EXPORT)
find_package(FFTW3 REQUIRED EXPORT)
find_package(artg4tk REQUIRED)
find_package(Range-v3 REQUIRED EXPORT)
find_package(nug4 REQUIRED EXPORT)
find_package(nurandom REQUIRED EXPORT)
find_package(CLHEP COMPONENTS Vector REQUIRED EXPORT)
find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(Geant4 REQUIRED EXPORT)
find_package(ROOT COMPONENTS Core Tree Hist Physics REQUIRED EXPORT)

find_package(larcore REQUIRED)
find_package(larcoreobj REQUIRED)
find_package(larcorealg REQUIRED)
find_package(lardataalg REQUIRED)
find_package(lardataobj REQUIRED)

include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# allow the use of #include "Geant4/.."
include_directories("${Geant4_INCLUDE_DIRS}/../") ### MIGRATE-ACTION-RECOMMENDED (migrate-3.09.03) - avoid directory-scope functions: use target_link_libraries() with target semantics or target_include_directories() whenever possible

add_subdirectory(fcl)
add_subdirectory(gdml)
add_subdirectory(larg4)
add_subdirectory(macros)
add_subdirectory(test)

# packaging utility
# this has to be last
cet_cmake_config()
