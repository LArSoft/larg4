# ======================================================================
#  larg4 main build file
#
#  . mrb slp   # Run if you have local products to set up
#  . mrbsetenv     # Needed to set up local development products
#  mrb b       # Build
#  mrb t       # Test (if applicable)
#  mrb i       # Install to local products
#  art -c <fhicl_file>  # Run!
#
# ======================================================================

# use cmake 3.7 or later
cmake_minimum_required (VERSION 3.7)

project(larg4)

# cetbuildtools contains our cmake modules
find_package(cetbuildtools REQUIRED)

list(APPEND CMAKE_MODULE_PATH
  $ENV{CANVAS_ROOT_IO_DIR}/Modules
  $ENV{ART_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
WERROR
EXTRA_FLAGS -pedantic
)

cet_report_compiler_flags()

# Locate some UPS areas and setup include paths
# These are needed for Art stuff

find_ups_product(cetbuildtools)
#
find_ups_product(artg4tk)
find_ups_product(art)
find_ups_product(canvas)
find_ups_product(fhiclcpp)
find_ups_product(messagefacility)
find_ups_product(larcoreobj)
find_ups_product(xerces_c)
find_ups_geant4()

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# Build these directories
add_subdirectory( larg4 )
add_subdirectory( fcl )
add_subdirectory( gdml )


# ups - table and config files

add_subdirectory(ups)


# packaging utility

include(UseCPack)






