if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/pcprep-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package pcprep)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT pcprep_Development
)

install(
    TARGETS pcprep_pcprep
    EXPORT pcprepTargets
    RUNTIME #
    COMPONENT pcprep_Runtime
    LIBRARY #
    COMPONENT pcprep_Runtime
    NAMELINK_COMPONENT pcprep_Development
    ARCHIVE #
    COMPONENT pcprep_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

if(BUILD_APP)
    install(
        TARGETS pcp
        EXPORT pcprepTargets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        COMPONENT pcprep_Runtime
    )
endif()

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    pcprep_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE pcprep_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(pcprep_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${pcprep_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT pcprep_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${pcprep_INSTALL_CMAKEDIR}"
    COMPONENT pcprep_Development
)

install(
    EXPORT pcprepTargets
    NAMESPACE pcprep::
    DESTINATION "${pcprep_INSTALL_CMAKEDIR}"
    COMPONENT pcprep_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
