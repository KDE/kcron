
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)

########## KCM Module ###############
add_library(kcm_cron MODULE)
ecm_qt_declare_logging_category(kcm_cron HEADER kcm_cron_debug.h IDENTIFIER KCM_CRON_LOG CATEGORY_NAME org.kde.kcm.cron DESCRIPTION "kcm cron" EXPORT KCRON)

target_sources(kcm_cron PRIVATE
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

   genericListWidget.cpp
    
   tasksWidget.cpp
   taskWidget.cpp 

   variablesWidget.cpp
   variableWidget.cpp 
 
   taskEditorDialog.cpp 
   variableEditorDialog.cpp

   crontabWidget.cpp 

   kcronHelper.cpp
    
   crontabPrinter.cpp 
   crontabPrinterWidget.cpp 

   kcmCron.cpp 
)


target_link_libraries(kcm_cron 
    Qt::PrintSupport
    KF5::ConfigWidgets
    KF5::I18n
    KF5::KIOWidgets
)

install(TARGETS  kcm_cron  DESTINATION ${KDE_INSTALL_PLUGINDIR} )

install(FILES kcm_cron.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR} )
