project(kcron)

# Help Eclipse to parse errors more efficiently
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
endif(CMAKE_COMPILER_IS_GNUCC)
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
endif(CMAKE_COMPILER_IS_GNUCXX)
remove_definitions( -DQT_NO_CAST_FROM_ASCII )
remove_definitions( -DQT_NO_CAST_TO_ASCII )


##
# To specify a different install prefix, use :
# cmake -D CMAKE_INSTALL_PREFIX=build .
#
# To help Eclipse discover include paths, use :
# cmake -D CMAKE_VERBOSE_MAKEFILE=true .
##

add_subdirectory(src) 


