/**********************************************************************************
 * Header file for the libsign namespace.
 *
 * Copyright (C) 2004 Joe Fox All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *********************************************************************************/

#ifndef __LibSign_h__
#define __LibSign_h__

#include <vector>

#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/x509.h>

#include "readelf.h"

namespace signelf
{
	typedef std::vector<unsigned char> UCharArray;

	// hash a given section in the pBfd and add it to pSha
	void hashSection(readelf::CReadElf *pElf, const char *szSectionName, SHA_CTX *pSha);

	// generate a hash of the .text and .data sections of the binary
	UCharArray hashLib(const char *szBinFile);

	// sign a given hash with the key stored in szKeyBuf;
	UCharArray signHash(const unsigned char *szHashBuf, unsigned int nHashSize, unsigned char *szKeyBuf, unsigned int nKeySize);

	// verify a libs signature (assumed to be stored in .lsesig section of szBinFile) with the key given in szKeyBuf
	bool verifyLib(unsigned char *szKeyBuf, unsigned int nKeySize, const char *szBinFile);

	// helper function, useful for debugging
	void hexPrint(const char *szName, const char *szBuf, const unsigned int nLength);
}

#endif
