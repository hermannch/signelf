/**********************************************************************************
 * Tool to generate an unsigned char "szKeyBuf" of the given PEM encoded RSA keyfile
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

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdio>

#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/pem.h>

/*
 * Use the output of this app to generate privkey.h and pubkey.h
 * to be used in applications wanting to sign or verify binaries.
 */
int main(int argc, char **argv)
{
	if(argc == 3)
	{
		std::cerr << "loading keyfile" << std::endl;

		// load up the key
		BIO *bio;
		bio = BIO_new_file(argv[2], "r");
		if(bio)
		{
			char *nm = NULL;
			unsigned char *data = NULL;
			long len = 0;
			std::string sKeyType(argv[1]);
			if(sKeyType == "priv")
			{
				std::cerr << "Reading private key" << std::endl;
				PEM_bytes_read_bio(&data, &len, &nm, PEM_STRING_EVP_PKEY, bio, NULL, NULL);
			}
			else if(sKeyType == "pub")
			{
				std::cerr << "Reading public key" << std::endl;
				PEM_bytes_read_bio(&data, &len, &nm, PEM_STRING_PUBLIC, bio, NULL, NULL);
			}
			else
			{
				std::cerr << "Unknown keytype" << std::endl;
			}

			// if we got data
			if(data)
			{
				// output the keybuf
				std::cout << "unsigned char szKeyBuf[] = {" << std::endl;

				// output the first char (to get the delimiters correct)
				std::cout << "\t0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(data[0]);

				// now output the rest of the bytes
				for(long i = 1; i < len; ++i)
				{
					std::cout << ", 0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(data[i]);
					// every 14th char, output a newline
					if((i % 14) == 0)
					{
						std::cout << "\n\t";
					}
				}

				// now close the set
				std::cout << "};" << std::endl;

				OPENSSL_free(data);
			}
		}
		std::cerr << "Done reading key" << std::endl;
	}
	else
	{
		std::cerr << "usage: " << argv[0] << " <priv|pub> <keyfile>" << std::endl
				  << "where priv and pub indicate the type of key in" << std::endl
				  << "keyfile. Keyfile is a PEM encoded key generated from" << std::endl
				  << "openssl genrsa or openssl rsa (See openssl(1) for details)" << std::endl;
	}
	return 0;
}

