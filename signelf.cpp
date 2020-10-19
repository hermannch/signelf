/**********************************************************************************
 * Implementation of the libsign functions declared in libsign.h
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

/*
 * Libsign is used to verify signatures in binary files. It expects a section in
 * the binary to be called ".lsesig". That section should contain an RSA signature
 * of the SHA1 hash of the .text and .data sections of the binary.
 */

#include <iostream>
#include <iomanip>
#include <iterator>

#include "readelf.h"
#include "signelf.h"

namespace signelf
{
	UCharArray hashLib(const char *szBinFile)
	{
		// create a buffer for the hash
		UCharArray arRetval;

		// create a SHA hash context
		SHA_CTX sha;
		SHA1_Init(&sha);

		readelf::CReadElf elf(szBinFile);
		// get the hash for the data section
		hashSection(&elf, ".data", &sha);
		// get the hash for the text section
		hashSection(&elf, ".text", &sha);
		// get the hash for the rodata section
		hashSection(&elf, ".rodata", &sha);
		// get the hash for the init section
		hashSection(&elf, ".init", &sha);
		// get the hash for the fini section
		hashSection(&elf, ".fini", &sha);
		// get the hash for the ctors section
		hashSection(&elf, ".ctors", &sha);
		// get the hash for the dtors section
		hashSection(&elf, ".dtors", &sha);
		// get the hash for the dynamic section
		hashSection(&elf, ".dynamic", &sha);
		// get the hash for the dynsym section
		hashSection(&elf, ".dynsym", &sha);
		// get the hash for the dynstr section
		hashSection(&elf, ".dynstr", &sha);
		// size the buffer large enough
		arRetval.resize(SHA_DIGEST_LENGTH);
		// resolve the hash
		SHA1_Final(&arRetval[0], &sha);


		return arRetval;
	}

	void hashSection(readelf::CReadElf *pElf, const char *szSectionName, SHA_CTX *pSHA)
	{
		// pick up the given section and generate a hash of the thing
		readelf::UCharArray section = pElf->getSection(szSectionName);
		if(!section.empty())
		{
			// hash them
			SHA1_Update(pSHA, &section[0], section.size());
		}
	}

	UCharArray signHash(const unsigned char *szHashBuf, unsigned int nHashSize, unsigned char *szKeyBuf, unsigned int nKeySize)
	{
		UCharArray arRetval;
		RSA *pKey = NULL;

		// wrap the keybuf in a bio
		BIO *pBio = NULL;
		pBio = BIO_new_mem_buf(szKeyBuf, nKeySize);

		// if we could wrap it
		if(pBio)
		{
			// Read the key from it
			d2i_RSAPrivateKey_bio(pBio, &pKey);
		}

		// if we could read the key
		if(pKey)
		{
			unsigned int nSigLen;
			arRetval.resize(RSA_size(pKey));

			// sign the hash and save it to arRetval
			RSA_sign(NID_sha1, szHashBuf, nHashSize, &arRetval[0], &nSigLen, pKey);
		}

		return arRetval;
	}

	bool verifyLib(unsigned char *szKeyBuf, unsigned int nKeySize, const char *szBinFile)
	{
		bool bResult = false;
		RSA *pKey = NULL;
		BIO *pBio;
		// load up the key
		pBio = BIO_new_mem_buf(szKeyBuf, nKeySize);
		if(pBio)
		{
			//		Load the key
			d2i_RSA_PUBKEY_bio(pBio, &pKey);
			BIO_free(pBio);
		}

		// if we could load the key
		if(pKey)
		{
			// get the signature from the binfile:w
			readelf::CReadElf elf(szBinFile);
			readelf::UCharArray szSig = elf.getSection(".lsesig");

			// if we could get the section
			if(!szSig.empty())
			{
				// pick up the hash of the binary
				UCharArray szHash = hashLib(szBinFile);

				// verify the signature
				bResult = (0 != RSA_verify(NID_sha1, &szHash[0], szHash.size(), &szSig[0], szSig.size(), pKey));
				RSA_free(pKey);
			}
		}

		return bResult;
	}

	void hexPrint(const char *szName, const char *szBuf, const unsigned int nLength)
	{
		std::cout << "hash (" << nLength << ") ---\n" << std::endl;
		for(unsigned int i=0 ; i < nLength ; ++i)
		{
			std::cout << std::hex << std::setw(2) << std::setfill('0') << szBuf[i];
		}
		std::cout << "\n---" << std::endl;
	}

}
