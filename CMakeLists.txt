cmake_minimum_required(VERSION 3.7)

project(larg4)

find_package(cetbuildtools REQUIRED)

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
find_ups_product(artg4tk)
find_ups_product(canvas)
find_ups_product(canvas_root_io)
find_ups_product(cetlib)
find_ups_product(clhep)
find_ups_product(fhiclcpp)
find_ups_geant4()
find_ups_product(larcore)
find_ups_product(lardataalg)
find_ups_product(lardataobj)
find_ups_product(larg4)
find_ups_product(messagefacility)
find_ups_product(nusimdata)
find_ups_product(nutools)

list(APPEND CMAKE_MODULE_PATH $ENV{CANVAS_ROOT_IO_DIR}/Modules $ENV{ART_DIR}/Modules)

include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)
include(UseCPack)

add_subdirectory(fcl)
add_subdirectory(gdml)
add_subdirectory(larg4)
add_subdirectory(ups)
