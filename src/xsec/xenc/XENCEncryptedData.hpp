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
 * XENCEncryptedData := Definition for holder object for EncryptedData 
 * element
 *
 * $Id: XENCEncryptedData.hpp 1826046 2018-03-06 21:06:58Z scantor $
 *
 */

#ifndef XENCENCRYPTEDDATA_INCLUDE
#define XENCENCRYPTEDDATA_INCLUDE

// XSEC Includes

#include <xsec/framework/XSECDefs.hpp>
#include <xsec/xenc/XENCEncryptedType.hpp>
#include <xsec/xenc/XENCCipherData.hpp>

class XSECEnv;

/**
 * @ingroup xenc
 */

/**
 * @brief Interface definition for the EncryptedData object
 *
 * The \<EncryptedData\> element is an abstract type which builds
 * on the EncryptedType element for encrypted data (as opposed to
 * encrypted keys).
 *
 * In general, this class should not be used directly.  For most
 * applications, callers will want to use the XENCCipher class
 * instead.
 *
 * The schema for EncryptedData is as follows:
 * 
 * \verbatim
  <element name='EncryptedData' type='xenc:EncryptedDataType'/>
  <complexType name='EncryptedDataType'>
    <complexContent>
     <extension base='xenc:EncryptedType'>
     </extension>
    </complexContent>
  </complexType>
\endverbatim
 */


class XSEC_EXPORT XENCEncryptedData : public virtual XENCEncryptedType {

	/** @name Constructors and Destructors */
	//@{

protected:

	XENCEncryptedData() {};

public:

	virtual ~XENCEncryptedData() {};


	/**
	* \brief Create a new object.
	*
	* Create a new object.
	*
	* @param env environment instance
	* @param type type of cipher data
	* @param algorithm algorithm to use
	* @param value value to use
	*
	* @returns the new object
	*/
	static XENCEncryptedData* create(
		const XSECEnv* env,
		XENCCipherData::XENCCipherDataType type,
		const XMLCh * algorithm,
		const XMLCh * value);

	/** @name Get Interface Methods */
	//@{


	//@}

private:

	// Unimplemented
	XENCEncryptedData(const XENCEncryptedData &);
	XENCEncryptedData & operator = (const XENCEncryptedData &);


};

#endif /* XENCENCRYPTEDDATA_INCLUDE */
