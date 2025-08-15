cmake_minimum_required (VERSION 3.16.0 FATAL_ERROR)
project(kcron)

set (QT_MIN_VERSION "6.8.0")
set (KF_MIN_VERSION "6.11.0")

find_package (ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

add_definitions(-DTRANSLATION_DOMAIN="kcm_cron")

include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMQtDeclareLoggingCategory)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(ECMDeprecationSettings)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

find_package (Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    PrintSupport
    Quick
)

find_package(KF6 REQUIRED COMPONENTS
    ConfigWidgets
    CoreAddons
    Auth
    I18n
    KCMUtils
)

find_package(KF6KirigamiAddons 0.11.76 REQUIRED)

find_package(KF6DocTools ${KF_MIN_VERSION})
set_package_properties(KF6DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)

ecm_set_disabled_deprecation_versions(QT 6.9.0
    KF 6.13
)

add_subdirectory(src)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.kde.kcron.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

ecm_qt_install_logging_categories(
	EXPORT KCRON
        FILE kcron.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
ki18n_install(po)
if (KF6DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
