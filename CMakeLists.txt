cmake_minimum_required (VERSION 3.16.0 FATAL_ERROR)
project(kcron)

set (QT_MIN_VERSION "5.15.0")
set (KF5_MIN_VERSION "5.83.0")

find_package (ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

add_definitions(-DTRANSLATION_DOMAIN="kcron")

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMQtDeclareLoggingCategory)

find_package (Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Widgets
    PrintSupport
)

find_package(KF5 REQUIRED COMPONENTS
    ConfigWidgets
    DocTools
    I18n
    KIO
)
include(KDEGitCommitHooks)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055300)


add_subdirectory(src) 
add_subdirectory(doc)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.kcron.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_qt_install_logging_categories(
	EXPORT KCRON
        FILE kcron.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )


kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
