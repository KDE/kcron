
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)

########## KCM Module ###############
add_library(kcm_cron MODULE)
ecm_qt_declare_logging_category(kcm_cron 
    HEADER kcm_cron_debug.h
    IDENTIFIER KCM_CRON_LOG
    CATEGORY_NAME org.kde.kcm.cron
    DESCRIPTION "kcm cron"
    EXPORT KCRON
)

target_sources(kcm_cron PRIVATE
   crontablib/cthost.cpp crontablib/cthost.h
   crontablib/ctcron.cpp crontablib/ctcron.h
   crontablib/ctmonth.cpp crontablib/ctmonth.h
   crontablib/ctminute.cpp crontablib/ctminute.h
   crontablib/cthour.cpp crontablib/cthour.h
   crontablib/ctdom.cpp crontablib/ctdom.h
   crontablib/ctdow.cpp crontablib/ctdow.h
   crontablib/cttask.cpp crontablib/cttask.h
   crontablib/ctunit.cpp crontablib/ctunit.h
   crontablib/ctvariable.cpp crontablib/ctvariable.h
   crontablib/ctSystemCron.cpp crontablib/ctSystemCron.h
   crontablib/ctInitializationError.cpp crontablib/ctInitializationError.h
   crontablib/ctSaveStatus.cpp crontablib/ctSaveStatus.h
   crontablib/ctHelper.cpp crontablib/ctHelper.h

   genericListWidget.cpp genericListWidget.h
    
   tasksWidget.cpp tasksWidget.h
   taskWidget.cpp taskWidget.h 

   variablesWidget.cpp variablesWidget.h
   variableWidget.cpp variableWidget.h 
 
   taskEditorDialog.cpp taskEditorDialog.h 
   variableEditorDialog.cpp variableEditorDialog.h

   crontabWidget.cpp crontabWidget.h 

   kcronHelper.cpp kcronHelper.h
    
   crontabPrinter.cpp crontabPrinter.h 
   crontabPrinterWidget.cpp crontabPrinterWidget.h 

   kcmCron.cpp kcmCron.h 
)


target_link_libraries(kcm_cron 
    Qt${QT_MAJOR_VERSION}::PrintSupport
    KF${KF_MAJOR_VERSION}::ConfigWidgets
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::AuthCore
)

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(kcm_cron KF6::KCMUtils)
endif()

install(TARGETS  kcm_cron  DESTINATION ${KDE_INSTALL_PLUGINDIR} )
# For root permissions.
add_subdirectory(helper)

install(FILES kcm_cron.desktop
        DESTINATION  ${KDE_INSTALL_KSERVICESDIR}
)
