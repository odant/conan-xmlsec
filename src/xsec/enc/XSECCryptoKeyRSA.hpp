/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * XSEC
 *
 * XSECCryptoKeyRSA := RSA Keys
 *
 * Author(s): Berin Lautenbach
 *
 * $Id: XSECCryptoKeyRSA.hpp 1819347 2017-12-27 18:05:51Z scantor $
 *
 */

#ifndef XSECCRYPTOKEYRSA_INCLUDE
#define XSECCRYPTOKEYRSA_INCLUDE

#include <xsec/enc/XSECCryptoHash.hpp>
#include <xsec/enc/XSECCryptoKey.hpp>

/**
 * \ingroup crypto
 */

/**
 * \brief Interface class for RSA keys.
 *
 * The library uses classes derived from this to process RSA keys.
 */


class XSEC_EXPORT XSECCryptoKeyRSA : public XSECCryptoKey {

public :

    /**
     * \brief Padding type
     *
     * Type of padding to be used for RSA encrypt/decrypt operations
     */

    enum PaddingType {

        PAD_NONE        = 0,        /** No padding (Illegal for all enc ops) */
        PAD_PKCS_1_5    = 1,        /** PKCS 1.5 padding */
        PAD_OAEP        = 2         /** OAEP */

    };

    /** @name Constructors and Destructors */
    //@{

    XSECCryptoKeyRSA() {};
    virtual ~XSECCryptoKeyRSA() {};

    //@}

    /** @name Key Interface methods */
    //@{

    /**
     * \brief Return the type of this key.
     *
     * For RSA keys, this allows people to determine whether this is a
     * public key, private key or a key pair
     */

    virtual XSECCryptoKey::KeyType getKeyType() const {return KEY_NONE;}

    /**
     * \brief Replicate key
     */

    virtual XSECCryptoKey* clone() const = 0;

    //@}

    /** @name Mandatory RSA interface methods
     *
     * These methods are required by the library.
     */
    //@{

    /**
     * \brief Verify a SHA1 PKCS1 encoded signature
     *
     * The library will call this function to validate an RSA signature
     * The standard by default uses SHA1 in a PKCS1 encoding.
     *
     * @param hashBuf Buffer containing the pre-calculated (binary) digest
     * @param hashLen Length of the data in the digest buffer
     * @param base64Signature Buffer containing the Base64 encoded signature
     * @param sigLen Length of the data in the signature buffer
     * @param type The type of hash that the signature is accross - must be a SHA variant
     * @returns true if the signature was valid, false otherwise
     */

    virtual bool verifySHA1PKCS1Base64Signature(const unsigned char * hashBuf,
                                 unsigned int hashLen,
                                 const char* base64Signature,
                                 unsigned int sigLen,
                                 XSECCryptoHash::HashType type) const = 0;

    /**
     * \brief Create a signature
     *
     * The library will call this function to create a signature from
     * a pre-calculated digest.  The output signature is required to
     * be Base64 encoded such that it can be placed directly into the
     * XML document
     *
     * This call needs to do a PKCS1 encode for a SHA-1 signature.
     *
     * @param hashBuf Buffer containing the pre-calculated (binary) digest
     * @param hashLen Number of bytes of hash in the hashBuf
     * @param base64SignatureBuf Buffer to place the base64 encoded result
     * in.
     * @param base64SignatureBufLen Implementations need to ensure they do
     * not write more bytes than this into the buffer
     * @param type Hash method used to calculate the Hash - needs to be passed
     * in order to calculate correct OID to be embedded in signature
     */


    virtual unsigned int signSHA1PKCS1Base64Signature(unsigned char* hashBuf,
        unsigned int hashLen,
        char* base64SignatureBuf,
        unsigned int base64SignatureBufLen,
        XSECCryptoHash::HashType type) const = 0;

    /**
     * \brief Decrypt using private key
     *
     * The library will call this function to decrypt a piece of cipher
     * text using the private component of this key.
     *
     * @param inBuf cipher text to decrypt
     * @param plainBuf output buffer for decrypted bytes
     * @param inLength bytes of cipher text to decrypt
     * @param maxOutLength size of outputBuffer
     * @param padding Type of padding (PKCS 1.5 or OAEP)
     * @param hashURI Hash Method for OAEP encryption
     * @param mgfURI algorithm identifier for OAEP mask generation function
     * @param params raw OAEP parameter data, if any
     * @param paramslen OEP parameter length
     */

    virtual unsigned int privateDecrypt(const unsigned char* inBuf,
                                 unsigned char* plainBuf,
                                 unsigned int inLength,
                                 unsigned int maxOutLength,
                                 PaddingType padding,
                                 const XMLCh* hashURI=NULL,
                                 const XMLCh* mgfURI=NULL,
                                 unsigned char* params=NULL,
                                 unsigned int paramsLen=0) const = 0;

    /**
     * \brief Encrypt using a public key
     *
     * The library will call this function to encrypt a plain text buffer
     * using the public component of this key.
     *
     * @param inBuf plain text to decrypt
     * @param cipherBuf output buffer for decrypted bytes
     * @param inLength bytes of plain text to encrypt
     * @param maxOutLength size of outputBuffer
     * @param padding Type of padding (PKCS 1.5 or OAEP)
     * @param hashURI Hash Method for OAEP encryption
     * @param mgfURI algorithm identifier for OAEP mask generation function
     * @param params raw OAEP parameter data, if any
     * @param paramslen OEP parameter length
     */

    virtual unsigned int publicEncrypt(const unsigned char* inBuf,
                                 unsigned char* cipherBuf,
                                 unsigned int inLength,
                                 unsigned int maxOutLength,
                                 PaddingType padding,
                                 const XMLCh* hashURI=NULL,
                                 const XMLCh* mgfURI=NULL,
                                 unsigned char* params=NULL,
                                 unsigned int paramsLen=0) const = 0;

    /**
     * \brief Obtain the length of an RSA key
     *
     * @returns The length of the rsa key (in bytes)
     */

    virtual unsigned int getLength() const = 0;

    //@}

    /** @name Optional Interface methods
     *
     * These functions do not necessarily have to be implmented.  They
     * are used by XSECKeyInfoResolverDefault to try to create a key from
     * KeyInfo elements without knowing anything else.
     *
     * If an interface class does not implement these functions, a simple
     * stub that does nothing should be used.
     */

    //@{

    /**
     * \brief Load the modulus
     *
     * Load the modulus from a Base64 encoded string
     *
     * param b64 A buffer containing the encoded string
     * param len The length of the data in the buffer
     */

    virtual void loadPublicModulusBase64BigNums(const char* b64, unsigned int len) = 0;

    /**
     * \brief Load the exponent
     *
     * Load the exponent from a Base64 encoded string
     *
     * param b64 A buffer containing the encoded string
     * param len The length of the data in the buffer
     */

    virtual void loadPublicExponentBase64BigNums(const char* b64, unsigned int len) = 0;

    //@}

};

#endif /* XSECCRYPTOKEYRSA_INCLUDE */
