project(kcron)

find_package(KDE4 REQUIRED)

include(KDE4Defaults)
include(MacroLibrary)

include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckPrototypeExists)
include(CheckTypeSize)

set(CMAKE_REQUIRED_DEFINITIONS ${_KDE4_PLATFORM_DEFINITIONS})
if(WIN32)
  set(CMAKE_REQUIRED_LIBRARIES ${KDEWIN32_LIBRARIES})
  set(CMAKE_REQUIRED_INCLUDES  ${KDEWIN32_INCLUDES})
endif(WIN32)
add_definitions(${QT_DEFINITIONS} ${QT_QTDBUS_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

# Help Eclipse to parse errors more efficiently
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif(CMAKE_COMPILER_IS_GNUCC)
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif(CMAKE_COMPILER_IS_GNUCXX)
#


##
# To specify a different install prefix, use :
# cmake -D CMAKE_INSTALL_PREFIX=build .
#
# To help Eclipse discover include paths, use :
# cmake -D CMAKE_VERBOSE_MAKEFILE=true .
##

add_subdirectory(src) 
add_subdirectory(doc)


