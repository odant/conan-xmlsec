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
 * TXFMCipher := Class that performs a symmetric encrypt/decrypt transform
 *
 * $Id: TXFMCipher.hpp 1818065 2017-12-13 20:55:19Z scantor $
 *
 */

#ifndef TXFMCIPHER_INCLUDE
#define TXFMCIPHER_INCLUDE

// XSEC Includes

#include <xsec/transformers/TXFMBase.hpp>
#include <xsec/enc/XSECCryptoSymmetricKey.hpp>
 
/**
 * \brief Transformer to handle symmetric encryption.
 *
 * Note that there is no particular XML DSIG/XENC transform associated
 * with encryption, but this is a convenient way to handle this process.
 * @ingroup internal
 */

class XSEC_EXPORT TXFMCipher : public TXFMBase {

public:

	// Constructors and destructors

    TXFMCipher(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,
                const XSECCryptoKey* key,
                bool encrypt,
                XSECCryptoSymmetricKey::SymmetricKeyMode mode = XSECCryptoSymmetricKey::MODE_CBC,
                unsigned int taglen = 0);
	virtual ~TXFMCipher();

	// Methods to get tranform output type and input requirement

	virtual TXFMBase::ioType getInputType(void) const;
	virtual TXFMBase::ioType getOutputType(void) const;
	virtual nodeType getNodeType(void) const;

	// TXFMCipher Unique

	void setKey(unsigned char * key, unsigned int keyLen);

	// Methods to set input data

	virtual void setInput(TXFMBase * newInput);

	// Methods to get output data

	virtual unsigned int readBytes(XMLByte * const toFill, const unsigned int maxToFill);
	
private:
	TXFMCipher();

	bool					m_doEncrypt;		// Are we in encrypt (or decrypt) mode
    unsigned int            m_taglen;           // Length of Authentication Tag for AEAD ciphers
	XSECCryptoKey			* mp_cipher;		// Crypto implementation
	bool					m_complete;
	unsigned char			m_inputBuffer[2050];
	unsigned char			m_outputBuffer[3072];	// Always keep 2K of data
	unsigned int			m_remaining;		// Amount remaining in output

};

#endif /* TXFMCIPHER_INCLUDE */

