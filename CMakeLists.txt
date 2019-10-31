cmake_minimum_required(VERSION 3.7)

project(larg4)

find_package(cetbuildtools REQUIRED)

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules $ENV{ART_DIR}/Modules)

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS WERROR NO_UNDEFINED EXTRA_FLAGS -pedantic)

#if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined,--no-allow-shlib-undefined,--as-needed")
#set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULELINKER_FLAGS} -Wl,--no-undefined,--no-allow-shlib-undefined,--as-needed")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined,--no-allow-shlib-undefined,--as-needed")
#endif()

cet_report_compiler_flags()

find_ups_product(art)
find_ups_product(art_root_io)
find_ups_product(artg4tk)
find_ups_product(canvas)
find_ups_product(canvas_root_io)
find_ups_product(cetlib)
find_ups_product(clhep)
find_ups_product(fhiclcpp)
find_ups_geant4()
find_ups_product(larcore)
find_ups_product(larcoreobj)
find_ups_product(lardataalg)
find_ups_product(lardataobj)
find_ups_product(messagefacility)
find_ups_product(nusimdata)
find_ups_product(nug4)

include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# artg4tk
cet_find_library( ARTG4TK_ACTIONBASE NAMES artg4tk_actionBase PATHS ENV ARTG4TK_LIB NO_DEFAULT_PATH )
cet_find_library( ARTG4TK_GEANTINIT NAMES artg4tk_geantInit PATHS ENV ARTG4TK_LIB NO_DEFAULT_PATH )
cet_find_library( ARTG4TK_PLUGINDETECTORS_GDML NAMES artg4tk_pluginDetectors_gdml PATHS ENV ARTG4TK_LIB NO_DEFAULT_PATH )
cet_find_library(ARTG4TK_DATAPRODUCTS_G4DETECTORHITS NAMES artg4tk_DataProducts_G4DetectorHits PATHS ENV ARTG4TK_LIB NO_DEFAULT_PATH)
cet_find_library(ARTG4TK_SERVICES_ACTIONHOLDER_SERVICE NAMES artg4tk_services_ActionHolder_service PATHS ENV ARTG4TK_LIB NO_DEFAULT_PATH)
cet_find_library(ARTG4TK_SERVICES_PHYSICSLISTHOLDER_SERVICE NAMES artg4tk_services_PhysicsListHolder_service PATHS ENV ARTG4TK_LIB NO_DEFAULT_PATH)
cet_find_library(ARTG4TK_SERVICES_DETECTORHOLDER_SERVICE NAMES artg4tk_services_DetectorHolder_service PATHS ENV ARTG4TK_LIB NO_DEFAULT_PATH)

add_subdirectory(fcl)
add_subdirectory(gdml)
add_subdirectory(larg4)
add_subdirectory(ups)

# packaging utility
# this has to be last
include(UseCPack)
