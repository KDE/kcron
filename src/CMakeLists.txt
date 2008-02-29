
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)

########## KCM Module ###############

set(kcm_cron_SRCS
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
