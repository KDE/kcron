
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)

########## KCM Module ###############

set(kcron_crontablib_SRCS
   crontablib/cthost.cpp
   crontablib/ctcron.cpp
   crontablib/ctmonth.cpp
   crontablib/ctminute.cpp
   crontablib/cthour.cpp
   crontablib/ctdom.cpp
   crontablib/ctdow.cpp
   crontablib/cttask.cpp
   crontablib/ctunit.cpp
   crontablib/ctvariable.cpp
   crontablib/ctGlobalCron.cpp
   crontablib/ctSystemCron.cpp
   crontablib/ctInitializationError.cpp
   crontablib/ctSaveStatus.cpp
   crontablib/ctHelper.cpp
)


set(kcm_cron_SRCS 
   ${kcron_crontablib_SRCS}
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
)

install(TARGETS  kcm_cron  DESTINATION ${PLUGIN_INSTALL_DIR} )

install(FILES kcm_cron.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
