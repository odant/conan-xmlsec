# XmlsecC Conan package
# Dmitriy Vetutnev, Odant, 2018


find_path(XmlsecC_INCLUDE_DIR
    NAMES xsec/framework/XSECVersion.hpp
    PATHS ${CONAN_INCLUDE_DIRS_XMLSEC-C}
    NO_DEFAULT_PATH
)

find_library(XmlsecC_LIBRARY
    NAMES xmlsec_2_0x64d xmlsec_2_0x64 xmlsec_2_0d xmlsec_2_0
    PATHS ${CONAN_LIB_DIRS_XMLSEC-C}
    NO_DEFAULT_PATH
)

if(XmlsecC_INCLUDE_DIR)

    file(STRINGS ${XmlsecC_INCLUDE_DIR}/xsec/framework/XSECVersion.hpp DEFINE_XmlsecC_MAJOR REGEX "^#define XSEC_VERSION_MAJOR")
    string(REGEX REPLACE "^.*XSEC_VERSION_MAJOR +([0-9]+).*$" "\\1" XmlsecC_VERSION_MAJOR "${DEFINE_XmlsecC_MAJOR}")

    file(STRINGS ${XmlsecC_INCLUDE_DIR}/xsec/framework/XSECVersion.hpp DEFINE_XmlsecC_MINOR REGEX "^#define XERCES_VERSION_MINOR")
    string(REGEX REPLACE "^.*XERCES_VERSION_MINOR +([0-9]+).*$" "\\1" XmlsecC_VERSION_MINOR "${DEFINE_XmlsecC_MINOR}")

    file(STRINGS ${XmlsecC_INCLUDE_DIR}/xsec/framework/XSECVersion.hpp DEFINE_XmlsecC_REVISION REGEX "^#define XERCES_VERSION_REVISION")
    string(REGEX REPLACE "^.*XERCES_VERSION_REVISION +([0-9]+).*$" "\\1" XmlsecC_VERSION_TWEAK "${DEFINE_XmlsecC_REVISION}")

    set(XmlsecC_VERSION_STRING "${XmlsecC_VERSION_MAJOR}.${XmlsecC_VERSION_MINOR}.${XmlsecC_VERSION_TWEAK}")
    set(XmlsecC_VERSION ${XmlsecC_VERSION_STRING})
    set(XmlsecC_VERSION_COUNT 3)

endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XmlsecC
    REQUIRED_VARS XmlsecC_INCLUDE_DIR XmlsecC_LIBRARY
    VERSION_VAR XmlsecC_VERSION
)


if(XmlsecC_FOUND AND NOT TARGET XmlsecC::XmlsecC)

    include(CMakeFindDependencyMacro)
    find_dependency(XercesC)
    find_dependency(OpenSSL)

    add_library(XmlsecC::XmlsecC UNKNOWN IMPORTED)

    set_target_properties(XmlsecC::XmlsecC PROPERTIES
        IMPORTED_LOCATION "${XmlsecC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${XmlsecC_INCLUDE_DIR}"
        INTERFACE_COMPILE_DEFINITIONS "${CONAN_COMPILE_DEFINITIONS_XMLSEC-C}"
        IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
    )
    set_property(TARGET XmlsecC::XmlsecC
        APPEND PROPERTY INTERFACE_LINK_LIBRARIES XercesC::XercesC OpenSSL::Crypto OpenSSL::SSL
    )

    mark_as_advanced(XmlsecC_INCLUDE_DIR XmlsecC_LIBRARY)

    set(XmlsecC_INCLUDE_DIRS ${XmlsecC_INCLUDE_DIR})
    set(XmlsecC_LIBRARIES ${XmlsecC_LIBRARY})
    set(XmlsecC_DEFINITIONS ${CONAN_COMPILE_DEFINITIONS_XMLSEC-C})

endif()

