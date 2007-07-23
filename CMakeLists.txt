project(kcron)

configure_file (config-kcron.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kcron.h )



########### next target ###############

set(kcron_SRCS 
   cthost.cpp 
   ctcron.cpp 
   ctmonth.cpp 
   ctdom.cpp 
   ctdow.cpp 
   cttask.cpp 
   ctvariable.cpp 
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
   main.cpp )


kde4_add_executable(kcron ${kcron_SRCS})

target_link_libraries(kcron  ${KDE4_KDEPRINT_LIBS} ${QT_QT3SUPPORT_LIBRARY})

install(TARGETS  kcron  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kcron.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kcronui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kcron )

kde4_install_icons( ${ICON_INSTALL_DIR}   )

