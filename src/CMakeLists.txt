
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)

set(kcron_SRCS
   genericListWidget.cpp
    
   tasksWidget.cpp
   taskWidget.cpp 

   variablesWidget.cpp
   variableWidget.cpp 
 
   taskEditorDialog.cpp 
   variableEditorDialog.cpp

   crontabWidget.cpp 

   kcronIcons.cpp
   kcronHelper.cpp

   kcron.cpp 
    
   crontabPrinter.cpp 
   crontabPrinterWidget.cpp 
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
