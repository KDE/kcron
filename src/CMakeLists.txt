
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

set(kcron_SRCS
   kticon.cpp 
   ktapp.cpp 
   ktview.cpp 
   ktlistitem.cpp 
   ktlistcron.cpp 
   ktlisttask.cpp 
   ktlisttasks.cpp 
   ktlistvar.cpp 
   ktlistvars.cpp 
   kttask.cpp 
   ktvariable.cpp 
   ktprint.cpp 
   ktprintopt.cpp 
   main.cpp 
)


########### Installation ###############

kde4_add_executable(kcron ${kcron_SRCS})

target_link_libraries(kcron 
    ${KDE4_KIO_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    kcron_crontablib
)

install(TARGETS  kcron  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcron.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kcronui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kcron )

kde4_install_icons( ${ICON_INSTALL_DIR}   )
