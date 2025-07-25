cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "6.17.0") # handled by release scripts
project(BluezQt VERSION ${KF_VERSION})

include(FeatureSummary)
find_package(ECM 6.16.0 NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEGitCommitHooks)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)

include(FeatureSummary)
include(ECMGenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMGenerateQDoc)
include(CMakePackageConfigHelpers)
include(ECMQtDeclareLoggingCategory)
include(ECMGeneratePkgConfigFile)
include(ECMQmlModule)
include(ECMDeprecationSettings)

set(EXCLUDE_DEPRECATED_BEFORE_AND_AT 0 CACHE STRING "Control the range of deprecated API excluded from the build [default=0].")

set(bluezqt_version_header "${CMAKE_CURRENT_BINARY_DIR}/src/bluezqt_version.h")
ecm_setup_version(PROJECT
    VARIABLE_PREFIX BLUEZQT
    VERSION_HEADER "${bluezqt_version_header}"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF6BluezQtConfigVersion.cmake"
    SOVERSION 6
)

# Dependencies
set(REQUIRED_QT_VERSION 6.7.0)

# Required Qt components to build this framework
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core DBus Network)

# Optional Qt components to build this framework
find_package(Qt6Test ${REQUIRED_QT_VERSION} CONFIG)
set_package_properties(Qt6Test PROPERTIES DESCRIPTION "Autotests for BluezQt" TYPE OPTIONAL)
find_package(Qt6Qml ${REQUIRED_QT_VERSION} CONFIG)
set_package_properties(Qt6Qml PROPERTIES DESCRIPTION "QML support for BluezQt" TYPE OPTIONAL)
find_package(Qt6QuickTest ${REQUIRED_QT_VERSION} CONFIG)
set_package_properties(Qt6QuickTest PROPERTIES DESCRIPTION "QML autotests for BluezQt" TYPE OPTIONAL)

ecm_set_disabled_deprecation_versions(
    QT 6.10.0
)

# Subdirectories
add_subdirectory(src)
add_subdirectory(tools/bluezapi2qt)

if (BUILD_TESTING)
    add_subdirectory(tests)
    if(Qt6Test_FOUND)
        add_subdirectory(autotests)
    endif()
endif()

if(Qt6Qml_FOUND)
    add_subdirectory(src/imports)
endif()

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF6BluezQt")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF6BluezQtConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF6BluezQtConfig.cmake"
    INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KF6BluezQtConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KF6BluezQtConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel)

install(EXPORT KF6BluezQtTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KF6BluezQtTargets.cmake
    NAMESPACE KF6::)

install(FILES
    ${bluezqt_version_header}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/BluezQt
    COMPONENT Devel)

ecm_generate_pkgconfig_file(BASE_NAME KF6BluezQt
  INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/BluezQt/
  DEPS "Qt6Core Qt6Network Qt6DBus"
INSTALL)

include(ECMFeatureSummary)
ecm_feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
