
########### Build ###############

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/crontablib
	${CMAKE_CURRENT_SOURCE_DIR} 
)

########## KCM Module ###############
kcmutils_add_qml_kcm(kcm_cron)
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

   genericmodel.cpp genericmodel.h

   variablesmodel.cpp variablesmodel.h
   variable.cpp variable.h

   tasksmodel.cpp tasksmodel.h
   task.cpp task.h

   cronPrinter.cpp cronPrinter.h

   taskvalidator.cpp taskvalidator.h

   kcmCron.cpp kcmCron.h 
)

target_link_libraries(kcm_cron
    Qt6::Core
    Qt6::PrintSupport
    KF6::ConfigWidgets
    KF6::I18n
    KF6::KIOWidgets
    KF6::CoreAddons
    KF6::AuthCore
    KF6::KCMUtils
)

# For root permissions.
add_subdirectory(helper)
