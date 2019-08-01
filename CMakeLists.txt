project(xmlsec_2_0 CXX C)
cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

SET(CMAKE_INSTALL_SO_NO_EXE OFF) # Install shared libraries as executable (UNIX only)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# Check build type
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
set(_supported_build_type "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
if((NOT DEFINED CMAKE_BUILD_TYPE) OR (NOT ${CMAKE_BUILD_TYPE} IN_LIST _supported_build_type))
	message(FATAL_ERROR "Unknow build type, please specify.\n-DCMAKE_BUILD_TYPE=Debug (either Release, RelWithDebInfo, MinSizeRel)")
endif()

string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPER)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR}/lib)
if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR}/lib)
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin)
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin)
if(MSVC)
    set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_PDB_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER} ${CMAKE_BINARY_DIR}/bin)
endif()
if(NOT CMAKE_INSTALL_PREFIX OR CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT OR "${CMAKE_INSTALL_PREFIX}" STREQUAL "")
    set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install CACHE PATH "Install path" FORCE)
    message(STATUS "Autoset CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
    message(STATUS "Remove directory " ${CMAKE_INSTALL_PREFIX})
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX})
else()
    message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}")
endif()

# Set binary suffix
if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
	set(CMAKE_${CMAKE_BUILD_TYPE_UPPER}_POSTFIX "x64")
	set(CMAKE_DEBUG_POSTFIX "x64d")
elseif(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
	#set(CMAKE_${CMAKE_BUILD_TYPE_UPPER}_POSTFIX "32")
	set(CMAKE_DEBUG_POSTFIX "d")
else()
	message(FATAL_ERROR "Unknow architecture")
endif()

message(STATUS "Remove directory " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

message(STATUS "Install Conan dependencies")

set(BUILD_TYPE_FOR_CONAN ${CMAKE_BUILD_TYPE})
if((${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo") OR (${CMAKE_BUILD_TYPE} STREQUAL "MinSizeRel"))
	set(BUILD_TYPE_FOR_CONAN "Release")
endif()

include(conan)
conan_cmake_run(
    BUILD_TYPE ${BUILD_TYPE_FOR_CONAN}
    CONANFILE conanfile.py
    BASIC_SETUP CMAKE_TARGETS NO_OUTPUT_DIRS UPDATE
    BUILD outdated
)

message(STATUS "Create install rules for Conan dependencies")
file(GLOB CONAN_IMPORTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/*)
foreach(_import_file ${CONAN_IMPORTS})
    string(TOLOWER ${_import_file} _import_file_lower)
    if(${_import_file_lower} MATCHES "\\.dll$|\\.so$|\\.so\\.")
        message(STATUS "Create install rule as program for ${_import_file}")
        install(PROGRAMS ${_import_file} DESTINATION ${CMAKE_INSTALL_PREFIX})
	elseif(${_import_file_lower} MATCHES "\\.pdb$")
        # Install PDB if needed
		if(ODA_INSTALL_PDB_FILES) 
			if((${CMAKE_BUILD_TYPE} STREQUAL "Debug") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
				message(STATUS "Create install rule as file for ${_import_file}")
				install(FILES ${_import_file} DESTINATION ${CMAKE_INSTALL_PREFIX})
			endif()
		endif()
    endif()
endforeach()
message(STATUS -----------------------------------------------)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Windows settings
if(MSVC)
    add_definitions(
        -DNOMINMAX
        -D_CRT_SECURE_NO_DEPRECATE
        -D_CRT_SECURE_NO_WARNINGS
        -D_SCL_SECURE_NO_WARNINGS
        -D_WIN32_WINNT=0x0601
        -DWINVER=0x0601
        -DNTDDI_VERSION=0x06010000
        -D_UNICODE 
        -DUNICODE
        -DWIN32
    )

	if((${CMAKE_BUILD_TYPE} STREQUAL "Release") OR (${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
		# Deleting conflicting flags for Release and RelWithDebInfo
		#   (/Ob1 and /Ob2)
		foreach (var
				 CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER} CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER})
			string(REPLACE "/Ob1" "" ${var} "${${var}}")
			message(STATUS "${var}: ${${var}}")
		endforeach()

        add_definitions(-DNDEBUG)
	    add_compile_options(/O2 /Ob2 /Oi /Ot /GL /GF /Gm- /GS- /Gy- /fp:fast)

		foreach (var
                 CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS CMAKE_STATIC_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS)
			set(${var} "${${var}} /LTCG /OPT:REF /OPT:ICF")
			message(STATUS "${var}: ${${var}}")
		endforeach()

		foreach (var
				 CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO)
			string(REGEX REPLACE "[-/][iI][nN][cC][rR][eE][mM][eE][nN][tT][aA][lL][^ \\t\\r\\n]*" "/INCREMENTAL:NO" ${var} "${${var}}")
			string(REGEX REPLACE "[-/][Dd][Ee][Bb][Uu][Gg]" "/DEBUG:FULL" ${var} "${${var}}")
			message(STATUS "${var}: ${${var}}")
		endforeach()

	endif()

endif() # MSVC

if(("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        add_definitions(-D_DEBUG)
    endif()

endif() # Compiler GCC or Clang

include(GNUInstallDirs)

# Build projects
message(STATUS "-----------------------------------------------------")
message(STATUS "Source directory: ${CMAKE_SOURCE_DIR}")
message(STATUS "Build directory: ${CMAKE_BINARY_DIR}")
message(STATUS "Install directory: ${CMAKE_INSTALL_PREFIX}")

set(canon_SOURCES
	src/xsec/canon/XSECC14n20010315.cpp
	src/xsec/canon/XSECCanon.cpp
	src/xsec/canon/XSECXMLNSStack.cpp
)
set(canon_HEADERS
	xsec/canon/XSECC14n20010315.hpp
	xsec/canon/XSECCanon.hpp
	xsec/canon/XSECXMLNSStack.hpp
)

set(dsig_SOURCES
	src/xsec/dsig/DSIGAlgorithmHandlerDefault.cpp
	src/xsec/dsig/DSIGConstants.cpp
	src/xsec/dsig/DSIGKeyInfoDEREncoded.cpp
	src/xsec/dsig/DSIGKeyInfoExt.cpp
	src/xsec/dsig/DSIGKeyInfoList.cpp
	src/xsec/dsig/DSIGKeyInfoMgmtData.cpp
	src/xsec/dsig/DSIGKeyInfoName.cpp
	src/xsec/dsig/DSIGKeyInfoPGPData.cpp
	src/xsec/dsig/DSIGKeyInfoSPKIData.cpp
	src/xsec/dsig/DSIGKeyInfoValue.cpp
	src/xsec/dsig/DSIGKeyInfoX509.cpp
	src/xsec/dsig/DSIGObject.cpp
	src/xsec/dsig/DSIGReference.cpp
	src/xsec/dsig/DSIGReferenceList.cpp
	src/xsec/dsig/DSIGSignature.cpp
	src/xsec/dsig/DSIGSignedInfo.cpp
	src/xsec/dsig/DSIGTransform.cpp
	src/xsec/dsig/DSIGTransformBase64.cpp
	src/xsec/dsig/DSIGTransformC14n.cpp
	src/xsec/dsig/DSIGTransformEnvelope.cpp
	src/xsec/dsig/DSIGTransformList.cpp
	src/xsec/dsig/DSIGTransformXPath.cpp
	src/xsec/dsig/DSIGTransformXPathFilter.cpp
	src/xsec/dsig/DSIGTransformXSL.cpp
	src/xsec/dsig/DSIGXPathFilterExpr.cpp
	src/xsec/dsig/DSIGXPathHere.cpp
)
set(dsig_HEADERS
	xsec/dsig/DSIGAlgorithmHandlerDefault.hpp
	xsec/dsig/DSIGConstants.hpp
	xsec/dsig/DSIGKeyInfo.hpp
	xsec/dsig/DSIGKeyInfoDEREncoded.hpp
	xsec/dsig/DSIGKeyInfoExt.hpp
	xsec/dsig/DSIGKeyInfoList.hpp
	xsec/dsig/DSIGKeyInfoMgmtData.hpp
	xsec/dsig/DSIGKeyInfoName.hpp
	xsec/dsig/DSIGKeyInfoPGPData.hpp
	xsec/dsig/DSIGKeyInfoSPKIData.hpp
	xsec/dsig/DSIGKeyInfoValue.hpp
	xsec/dsig/DSIGKeyInfoX509.hpp
	xsec/dsig/DSIGObject.hpp
	xsec/dsig/DSIGReference.hpp
	xsec/dsig/DSIGReferenceList.hpp
	xsec/dsig/DSIGSignature.hpp
	xsec/dsig/DSIGSignedInfo.hpp
	xsec/dsig/DSIGTransform.hpp
	xsec/dsig/DSIGTransformBase64.hpp
	xsec/dsig/DSIGTransformC14n.hpp
	xsec/dsig/DSIGTransformEnvelope.hpp
	xsec/dsig/DSIGTransformList.hpp
	xsec/dsig/DSIGTransformXPath.hpp
	xsec/dsig/DSIGTransformXPathFilter.hpp
	xsec/dsig/DSIGTransformXSL.hpp
	xsec/dsig/DSIGXPathFilterExpr.hpp
	xsec/dsig/DSIGXPathHere.hpp
)

set(enc_NSS_SOURCES
	src/xsec/enc/NSS/NSSCryptoHash.cpp
	src/xsec/enc/NSS/NSSCryptoHashHMAC.cpp
	src/xsec/enc/NSS/NSSCryptoKeyDSA.cpp
	src/xsec/enc/NSS/NSSCryptoKeyHMAC.cpp
	src/xsec/enc/NSS/NSSCryptoKeyRSA.cpp
	src/xsec/enc/NSS/NSSCryptoProvider.cpp
	src/xsec/enc/NSS/NSSCryptoSymmetricKey.cpp
	src/xsec/enc/NSS/NSSCryptoX509.cpp
)
set(enc_NSS_HEADERS
	xsec/enc/NSS/NSSCryptoHash.hpp
	xsec/enc/NSS/NSSCryptoHashHMAC.hpp
	xsec/enc/NSS/NSSCryptoKeyDSA.hpp
	xsec/enc/NSS/NSSCryptoKeyHMAC.hpp
	xsec/enc/NSS/NSSCryptoKeyRSA.hpp
	xsec/enc/NSS/NSSCryptoProvider.hpp
	xsec/enc/NSS/NSSCryptoSymmetricKey.hpp
	xsec/enc/NSS/NSSCryptoX509.hpp
)

set(enc_WinCAPI_SOURCES
	src/xsec/enc/WinCAPI/WinCAPICryptoHash.cpp
	src/xsec/enc/WinCAPI/WinCAPICryptoHashHMAC.cpp
	src/xsec/enc/WinCAPI/WinCAPICryptoKeyDSA.cpp
	src/xsec/enc/WinCAPI/WinCAPICryptoKeyHMAC.cpp
	src/xsec/enc/WinCAPI/WinCAPICryptoKeyRSA.cpp
	src/xsec/enc/WinCAPI/WinCAPICryptoProvider.cpp
	src/xsec/enc/WinCAPI/WinCAPICryptoSymmetricKey.cpp
	src/xsec/enc/WinCAPI/WinCAPICryptoX509.cpp
)
set(enc_WinCAPI_HEADERS
	xsec/enc/WinCAPI/WinCAPICryptoHash.hpp
	xsec/enc/WinCAPI/WinCAPICryptoHashHMAC.hpp
	xsec/enc/WinCAPI/WinCAPICryptoKeyDSA.hpp
	xsec/enc/WinCAPI/WinCAPICryptoKeyHMAC.hpp
	xsec/enc/WinCAPI/WinCAPICryptoKeyRSA.hpp
	xsec/enc/WinCAPI/WinCAPICryptoProvider.hpp
	xsec/enc/WinCAPI/WinCAPICryptoSymmetricKey.hpp
	xsec/enc/WinCAPI/WinCAPICryptoX509.hpp
)

set(enc_OpenSSL_SOURCES
	src/xsec/enc/OpenSSL/OpenSSLCryptoBase64.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoHash.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoHashHMAC.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoKeyDSA.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoKeyEC.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoProvider.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoSymmetricKey.cpp
	src/xsec/enc/OpenSSL/OpenSSLCryptoX509.cpp
	src/xsec/enc/OpenSSL/OpenSSLSupport.cpp
)
set(enc_OpenSSL_HEADERS
	xsec/enc/OpenSSL/OpenSSLCryptoBase64.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoHash.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoHashHMAC.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoKeyDSA.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoKeyEC.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoKeyHMAC.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoKeyRSA.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoProvider.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoSymmetricKey.hpp
	xsec/enc/OpenSSL/OpenSSLCryptoX509.hpp
	xsec/enc/OpenSSL/OpenSSLSupport.hpp
)

set(enc_XSCrypt_SOURCES
	src/xsec/enc/XSCrypt/XSCryptCryptoBase64.cpp
)
set(enc_XSCrypt_HEADERS
	xsec/enc/XSCrypt/XSCryptCryptoBase64.hpp
)

set(enc_SOURCES
	src/xsec/enc/XSECCryptoBase64.cpp
	src/xsec/enc/XSECCryptoException.cpp
	src/xsec/enc/XSECCryptoUtils.cpp
	src/xsec/enc/XSECCryptoX509.cpp
	src/xsec/enc/XSECKeyInfoResolverDefault.cpp
)
set(enc_HEADERS
	xsec/enc/XSECCryptoBase64.hpp
	xsec/enc/XSECCryptoException.hpp
	xsec/enc/XSECCryptoHash.hpp
	xsec/enc/XSECCryptoKey.hpp
	xsec/enc/XSECCryptoKeyDSA.hpp
	xsec/enc/XSECCryptoKeyEC.hpp
	xsec/enc/XSECCryptoKeyHMAC.hpp
	xsec/enc/XSECCryptoKeyRSA.hpp
	xsec/enc/XSECCryptoProvider.hpp
	xsec/enc/XSECCryptoSymmetricKey.hpp
	xsec/enc/XSECCryptoUtils.hpp
	xsec/enc/XSECCryptoX509.hpp
	xsec/enc/XSECKeyInfoResolver.hpp
	xsec/enc/XSECKeyInfoResolverDefault.hpp
)

set(framework_SOURCES
	src/xsec/framework/XSECAlgorithmMapper.cpp
	src/xsec/framework/XSECEnv.cpp
	src/xsec/framework/XSECError.cpp
	src/xsec/framework/XSECException.cpp
	src/xsec/framework/XSECProvider.cpp
	src/xsec/framework/XSECURIResolverXerces.cpp
)
set(framework_HEADERS
	xsec/framework/XSECAlgorithmHandler.hpp
	xsec/framework/XSECAlgorithmMapper.hpp
	# XSECConfig.hpp.in
	xsec/framework/XSECDefs.hpp
	xsec/framework/XSECEnv.hpp
	xsec/framework/XSECError.hpp
	xsec/framework/XSECException.hpp
	xsec/framework/XSECProvider.hpp
	xsec/framework/XSECURIResolver.hpp
	xsec/framework/XSECURIResolverXerces.hpp
	xsec/framework/XSECVersion.hpp
	xsec/framework/XSECW32Config.hpp
)

set(transformers_SOURCES
	src/xsec/transformers/TXFMBase.cpp
	src/xsec/transformers/TXFMBase64.cpp
	src/xsec/transformers/TXFMC14n.cpp
	src/xsec/transformers/TXFMChain.cpp
	src/xsec/transformers/TXFMChar.cpp
	src/xsec/transformers/TXFMCipher.cpp
	src/xsec/transformers/TXFMConcatChains.cpp
	src/xsec/transformers/TXFMDocObject.cpp
	src/xsec/transformers/TXFMEnvelope.cpp
	src/xsec/transformers/TXFMHash.cpp
	src/xsec/transformers/TXFMOutputFile.cpp
	src/xsec/transformers/TXFMParser.cpp
	src/xsec/transformers/TXFMSB.cpp
	src/xsec/transformers/TXFMURL.cpp
	src/xsec/transformers/TXFMXPath.cpp
	src/xsec/transformers/TXFMXPathFilter.cpp
	src/xsec/transformers/TXFMXSL.cpp
)
set(transformers_HEADERS
	xsec/transformers/TXFMBase.hpp
	xsec/transformers/TXFMBase64.hpp
	xsec/transformers/TXFMC14n.hpp
	xsec/transformers/TXFMChain.hpp
	xsec/transformers/TXFMChar.hpp
	xsec/transformers/TXFMCipher.hpp
	xsec/transformers/TXFMConcatChains.hpp
	xsec/transformers/TXFMDocObject.hpp
	xsec/transformers/TXFMEnvelope.hpp
	xsec/transformers/TXFMHash.hpp
	xsec/transformers/TXFMOutputFile.hpp
	xsec/transformers/TXFMParser.hpp
	xsec/transformers/TXFMSB.hpp
	xsec/transformers/TXFMURL.hpp
	xsec/transformers/TXFMXPath.hpp
	xsec/transformers/TXFMXPathFilter.hpp
	xsec/transformers/TXFMXSL.hpp
)

set(utils_SOURCES
	src/xsec/utils/XSECAlgorithmSupport.cpp
	src/xsec/utils/XSECBinTXFMInputStream.cpp
	src/xsec/utils/XSECDOMUtils.cpp
	src/xsec/utils/XSECNameSpaceExpander.cpp
	src/xsec/utils/XSECPlatformUtils.cpp
	src/xsec/utils/XSECSafeBuffer.cpp
	src/xsec/utils/XSECSafeBufferFormatter.cpp
	src/xsec/utils/XSECSOAPRequestorSimple.cpp
	src/xsec/utils/XSECTXFMInputSource.cpp
	src/xsec/utils/XSECXPathNodeList.cpp
)
set(utils_HEADERS
	xsec/utils/XSECAlgorithmSupport.hpp
	xsec/utils/XSECAutoPtr.hpp
	xsec/utils/XSECBinTXFMInputStream.hpp
	xsec/utils/XSECDOMUtils.hpp
	xsec/utils/XSECNameSpaceExpander.hpp
	xsec/utils/XSECPlatformUtils.hpp
	xsec/utils/XSECSafeBuffer.hpp
	xsec/utils/XSECSafeBufferFormatter.hpp
	xsec/utils/XSECSOAPRequestor.hpp
	xsec/utils/XSECSOAPRequestorSimple.hpp
	xsec/utils/XSECTXFMInputSource.hpp
	xsec/utils/XSECXPathNodeList.hpp
)

set(xenc_impl_SOURCES
	src/xsec/xenc/impl/XENCAlgorithmHandlerDefault.cpp
	src/xsec/xenc/impl/XENCCipherDataImpl.cpp
	src/xsec/xenc/impl/XENCCipherImpl.cpp
	src/xsec/xenc/impl/XENCCipherReferenceImpl.cpp
	src/xsec/xenc/impl/XENCCipherValueImpl.cpp
	src/xsec/xenc/impl/XENCEncryptedDataImpl.cpp
	src/xsec/xenc/impl/XENCEncryptedKeyImpl.cpp
	src/xsec/xenc/impl/XENCEncryptedTypeImpl.cpp
	src/xsec/xenc/impl/XENCEncryptionMethodImpl.cpp
)
set(xenc_impl_HEADERS
	xsec/xenc/impl/XENCAlgorithmHandlerDefault.hpp
	xsec/xenc/impl/XENCCipherDataImpl.hpp
	xsec/xenc/impl/XENCCipherImpl.hpp
	xsec/xenc/impl/XENCCipherReferenceImpl.hpp
	xsec/xenc/impl/XENCCipherValueImpl.hpp
	xsec/xenc/impl/XENCEncryptedDataImpl.hpp
	xsec/xenc/impl/XENCEncryptedKeyImpl.hpp
	xsec/xenc/impl/XENCEncryptedTypeImpl.hpp
	xsec/xenc/impl/XENCEncryptionMethodImpl.hpp
)

set(xenc_HEADERS
	xsec/xenc/XENCCipher.hpp
	xsec/xenc/XENCCipherData.hpp
	xsec/xenc/XENCCipherReference.hpp
	xsec/xenc/XENCCipherValue.hpp
	xsec/xenc/XENCEncryptedData.hpp
	xsec/xenc/XENCEncryptedKey.hpp
	xsec/xenc/XENCEncryptedType.hpp
	xsec/xenc/XENCEncryptionMethod.hpp
)

set(xkms_impl_SOURCES
	src/xsec/xkms/impl/XKMSAuthenticationImpl.cpp
	src/xsec/xkms/impl/XKMSCompoundRequestImpl.cpp
	src/xsec/xkms/impl/XKMSCompoundResultImpl.cpp
	src/xsec/xkms/impl/XKMSKeyBindingAbstractTypeImpl.cpp
	src/xsec/xkms/impl/XKMSKeyBindingImpl.cpp
	src/xsec/xkms/impl/XKMSLocateRequestImpl.cpp
	src/xsec/xkms/impl/XKMSLocateResultImpl.cpp
	src/xsec/xkms/impl/XKMSMessageAbstractTypeImpl.cpp
	src/xsec/xkms/impl/XKMSMessageFactoryImpl.cpp
	src/xsec/xkms/impl/XKMSNotBoundAuthentication.cpp
	src/xsec/xkms/impl/XKMSPendingRequestImpl.cpp
	src/xsec/xkms/impl/XKMSPrototypeKeyBindingImpl.cpp
	src/xsec/xkms/impl/XKMSQueryKeyBindingImpl.cpp
	src/xsec/xkms/impl/XKMSRecoverKeyBindingImpl.cpp
	src/xsec/xkms/impl/XKMSRecoverRequestImpl.cpp
	src/xsec/xkms/impl/XKMSRecoverResultImpl.cpp
	src/xsec/xkms/impl/XKMSRegisterRequestImpl.cpp
	src/xsec/xkms/impl/XKMSRegisterResultImpl.cpp
	src/xsec/xkms/impl/XKMSReissueKeyBindingImpl.cpp
	src/xsec/xkms/impl/XKMSReissueRequestImpl.cpp
	src/xsec/xkms/impl/XKMSReissueResultImpl.cpp
	src/xsec/xkms/impl/XKMSRequestAbstractTypeImpl.cpp
	src/xsec/xkms/impl/XKMSRespondWithImpl.cpp
	src/xsec/xkms/impl/XKMSResponseMechanismImpl.cpp
	src/xsec/xkms/impl/XKMSResultImpl.cpp
	src/xsec/xkms/impl/XKMSResultTypeImpl.cpp
	src/xsec/xkms/impl/XKMSRevokeKeyBindingImpl.cpp
	src/xsec/xkms/impl/XKMSRevokeRequestImpl.cpp
	src/xsec/xkms/impl/XKMSRevokeResultImpl.cpp
	src/xsec/xkms/impl/XKMSRSAKeyPairImpl.cpp
	src/xsec/xkms/impl/XKMSStatusImpl.cpp
	src/xsec/xkms/impl/XKMSStatusRequestImpl.cpp
	src/xsec/xkms/impl/XKMSStatusResultImpl.cpp
	src/xsec/xkms/impl/XKMSUnverifiedKeyBindingImpl.cpp
	src/xsec/xkms/impl/XKMSUseKeyWithImpl.cpp
	src/xsec/xkms/impl/XKMSValidateRequestImpl.cpp
	src/xsec/xkms/impl/XKMSValidateResultImpl.cpp
	src/xsec/xkms/impl/XKMSValidityIntervalImpl.cpp
)
set(xkms_impl_HEADERS
	xsec/xkms/impl/XKMSAuthenticationImpl.hpp
	xsec/xkms/impl/XKMSCompoundRequestImpl.hpp
	xsec/xkms/impl/XKMSCompoundResultImpl.hpp
	xsec/xkms/impl/XKMSKeyBindingAbstractTypeImpl.hpp
	xsec/xkms/impl/XKMSKeyBindingImpl.hpp
	xsec/xkms/impl/XKMSLocateRequestImpl.hpp
	xsec/xkms/impl/XKMSLocateResultImpl.hpp
	xsec/xkms/impl/XKMSMessageAbstractTypeImpl.hpp
	xsec/xkms/impl/XKMSMessageFactoryImpl.hpp
	xsec/xkms/impl/XKMSNotBoundAuthenticationImpl.hpp
	xsec/xkms/impl/XKMSPendingRequestImpl.hpp
	xsec/xkms/impl/XKMSPrototypeKeyBindingImpl.hpp
	xsec/xkms/impl/XKMSQueryKeyBindingImpl.hpp
	xsec/xkms/impl/XKMSRecoverKeyBindingImpl.hpp
	xsec/xkms/impl/XKMSRecoverRequestImpl.hpp
	xsec/xkms/impl/XKMSRecoverResultImpl.hpp
	xsec/xkms/impl/XKMSRegisterRequestImpl.hpp
	xsec/xkms/impl/XKMSRegisterResultImpl.hpp
	xsec/xkms/impl/XKMSReissueKeyBindingImpl.hpp
	xsec/xkms/impl/XKMSReissueRequestImpl.hpp
	xsec/xkms/impl/XKMSReissueResultImpl.hpp
	xsec/xkms/impl/XKMSRequestAbstractTypeImpl.hpp
	xsec/xkms/impl/XKMSRespondWithImpl.hpp
	xsec/xkms/impl/XKMSResponseMechanismImpl.hpp
	xsec/xkms/impl/XKMSResultImpl.hpp
	xsec/xkms/impl/XKMSResultTypeImpl.hpp
	xsec/xkms/impl/XKMSRevokeKeyBindingImpl.hpp
	xsec/xkms/impl/XKMSRevokeRequestImpl.hpp
	xsec/xkms/impl/XKMSRevokeResultImpl.hpp
	xsec/xkms/impl/XKMSRSAKeyPairImpl.hpp
	xsec/xkms/impl/XKMSStatusImpl.hpp
	xsec/xkms/impl/XKMSStatusRequestImpl.hpp
	xsec/xkms/impl/XKMSStatusResultImpl.hpp
	xsec/xkms/impl/XKMSUnverifiedKeyBindingImpl.hpp
	xsec/xkms/impl/XKMSUseKeyWithImpl.hpp
	xsec/xkms/impl/XKMSValidateRequestImpl.hpp
	xsec/xkms/impl/XKMSValidateResultImpl.hpp
	xsec/xkms/impl/XKMSValidityIntervalImpl.hpp
)

set(xkms_SOURCES
	src/xsec/xkms/XKMSConstants.cpp
)
set(xkms_HEADERS
	xsec/xkms/XKMSAuthentication.hpp
	xsec/xkms/XKMSCompoundRequest.hpp
	xsec/xkms/XKMSCompoundResult.hpp
	xsec/xkms/XKMSConstants.hpp
	xsec/xkms/XKMSKeyBinding.hpp
	xsec/xkms/XKMSKeyBindingAbstractType.hpp
	xsec/xkms/XKMSLocateRequest.hpp
	xsec/xkms/XKMSLocateResult.hpp
	xsec/xkms/XKMSMessageAbstractType.hpp
	xsec/xkms/XKMSMessageFactory.hpp
	xsec/xkms/XKMSNotBoundAuthentication.hpp
	xsec/xkms/XKMSPendingRequest.hpp
	xsec/xkms/XKMSPrototypeKeyBinding.hpp
	xsec/xkms/XKMSQueryKeyBinding.hpp
	xsec/xkms/XKMSRecoverKeyBinding.hpp
	xsec/xkms/XKMSRecoverRequest.hpp
	xsec/xkms/XKMSRecoverResult.hpp
	xsec/xkms/XKMSRegisterRequest.hpp
	xsec/xkms/XKMSRegisterResult.hpp
	xsec/xkms/XKMSReissueKeyBinding.hpp
	xsec/xkms/XKMSReissueRequest.hpp
	xsec/xkms/XKMSReissueResult.hpp
	xsec/xkms/XKMSRequestAbstractType.hpp
	xsec/xkms/XKMSRespondWith.hpp
	xsec/xkms/XKMSResponseMechanism.hpp
	xsec/xkms/XKMSResult.hpp
	xsec/xkms/XKMSResultType.hpp
	xsec/xkms/XKMSRevokeKeyBinding.hpp
	xsec/xkms/XKMSRevokeRequest.hpp
	xsec/xkms/XKMSRevokeResult.hpp
	xsec/xkms/XKMSRSAKeyPair.hpp
	xsec/xkms/XKMSStatus.hpp
	xsec/xkms/XKMSStatusRequest.hpp
	xsec/xkms/XKMSStatusResult.hpp
	xsec/xkms/XKMSUnverifiedKeyBinding.hpp
	xsec/xkms/XKMSUseKeyWith.hpp
	xsec/xkms/XKMSValidateRequest.hpp
	xsec/xkms/XKMSValidateResult.hpp
	xsec/xkms/XKMSValidityInterval.hpp
)

set(SOURCES
	${canon_SOURCES}
	${dsig_SOURCES}
	${enc_NSS_SOURCES}
	${enc_WinCAPI_SOURCES}
	${enc_OpenSSL_SOURCES}
	${enc_XSCrypt_SOURCES}
	${enc_SOURCES}
	${framework_SOURCES}
	${transformers_SOURCES}
	${utils_SOURCES}
	${xenc_impl_SOURCES}
	${xkms_impl_SOURCES}
	${xkms_SOURCES}
)

set(xmlsec_HEADERS
	${canon_HEADERS}
	${dsig_HEADERS}
	${enc_NSS_HEADERS}
	${enc_WinCAPI_HEADERS}
	${enc_OpenSSL_HEADERS}
	${enc_XSCrypt_HEADERS}
	${enc_HEADERS}
	${framework_HEADERS}
	${transformers_HEADERS}
	${utils_HEADERS}
	${xenc_HEADERS}
	${xenc_impl_HEADERS}
	${xkms_impl_HEADERS}
	${xkms_HEADERS}
)


if(WIN32)
    set(SOURCES
        ${SOURCES}
		src/xsec/utils/winutils/XSECSOAPRequestorSimpleWin32.cpp
    )
else()
    set(SOURCES
        ${SOURCES}
		src/xsec/utils/unixutils/XSECSOAPRequestorSimpleUnix.cpp
    )
    set(xmlsec_HEADERS
        ${xmlsec_HEADERS}
        xsec/framework/XSECConfig.hpp
        xsec/framework/config.h
   ) 
endif()

set(RESOURCE)
if(WIN32)
	set(RESOURCE src/xsec/framework/version.rc)
endif()

find_package(OpenSSL REQUIRED)
find_package(XercesC REQUIRED)

add_library(${PROJECT_NAME} SHARED
	${SOURCES}
	${RESOURCE}
)

# Set output name executable targets, because earlier CMake versions which did not use this variable for executables
if(DEFINED CMAKE_${CMAKE_BUILD_TYPE_UPPER}_POSTFIX)

    set_target_properties(${PROJECT_NAME}
        PROPERTIES ${CMAKE_BUILD_TYPE_UPPER}_POSTFIX
        ${CMAKE_${CMAKE_BUILD_TYPE_UPPER}_POSTFIX}
    )

endif()

if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /bigobj)
endif()

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE ${CMAKE_SOURCE_DIR}
	PRIVATE ${CMAKE_SOURCE_DIR}/src
    PRIVATE $<TARGET_PROPERTY:XercesC::XercesC,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(${PROJECT_NAME}
	CONAN_PKG::openssl
	CONAN_PKG::xerces-c
)

add_definitions(
	-DXSEC_HAVE_OPENSSL
	-DXSEC_BUILDING_LIBRARY
	-DXSEC_XKMS_ENABLED
)

if(WIN32)
	add_definitions(
		-DXSEC_HAVE_WINCAPI
	)
	if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		add_definitions(
			-D_XSEC_DO_MEMDEBUG
		)
    endif()
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

foreach(header IN LISTS xmlsec_HEADERS)
    get_filename_component(headerDir "${header}" DIRECTORY)
    install(
        FILES "src/${header}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${headerDir}"
	)
endforeach()

if(MSVC)
	set(_pdb_name "${PROJECT_NAME}${CMAKE_${CMAKE_BUILD_TYPE_UPPER}_POSTFIX}")
	set_target_properties(${PROJECT_NAME} PROPERTIES PDB_NAME ${_pdb_name})
	install(FILES "${CMAKE_PDB_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE_UPPER}}/${_pdb_name}.pdb" DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()
