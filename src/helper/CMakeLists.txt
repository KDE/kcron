
add_executable(kcron_helper kcronhelper.cpp kcronhelper.h)
target_link_libraries(kcron_helper PRIVATE KF5::AuthCore Qt${QT_MAJOR_VERSION}::Core KF5::AuthCore)

ecm_qt_declare_logging_category(kcron_helper
    HEADER kcm_cron_helper_debug.h
    IDENTIFIER KCM_CRON_HELPER_LOG
    CATEGORY_NAME org.kde.kcm.cron_helper
    DESCRIPTION "kcm cron helper"
    EXPORT KCRON_HELPER
)

# The following need to be disabled in debug builds, if lacking root permissions.
# Build the project to generate the files, then install them manually:
# /usr/share/dbus-1/system-services/ <- local.kcron.crontab.service
# /usr/share/polkit-1/actions/ <- local.kcron.crontab.policy
# /usr/share/dbus-1/system.d/ <- local.kcron.crontab.conf
install(TARGETS kcron_helper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})
kauth_install_helper_files(kcron_helper local.kcron.crontab root)
kauth_install_actions(local.kcron.crontab local.kcron.crontab.actions)
