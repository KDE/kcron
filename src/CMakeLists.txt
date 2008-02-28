
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)


set(kcron_generic_SRCS
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
    
   crontabPrinter.cpp 
   crontabPrinterWidget.cpp 
)

########### Standalone Executable ###############

set(kcron_SRCS
   ${kcron_generic_SRCS}
   kcron.cpp 
   main.cpp 
)


kde4_add_executable(kcron ${kcron_SRCS})

target_link_libraries(kcron 
    ${KDE4_KIO_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    kcron_crontablib
)

install(TARGETS  kcron  DESTINATION ${BIN_INSTALL_DIR} )

install(FILES kcron.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )


########### KCM Module ###############

set(kcm_cron_SRCS
   ${kcron_generic_SRCS}
   kcmCron.cpp 
)


kde4_add_plugin(kcm_cron ${kcm_cron_SRCS})

target_link_libraries(kcm_cron 
    ${KDE4_KIO_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    kcron_crontablib
)

install(TARGETS  kcm_cron  DESTINATION ${PLUGIN_INSTALL_DIR} )

install(FILES kcm_cron.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

########### install files ###############

install( FILES kcronui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kcron )

kde4_install_icons( ${ICON_INSTALL_DIR}   )
