/**********************************************************************************
 * Implementation for readelf. This is a helper class for parsing elf files
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
#include <iterator>

#include <iterator>

#include "readelf.h"

namespace
{
	typedef std::vector<Elf32_Shdr> SectionArray;
}

/*
 * CReadElf elf(.../libappinfo.so);
 * CharArray dataBuf = elf.getSection(".data");
 * CharArray textBuf = elf.getSection(".text");
 *
 * hash dataBuf and textBuf
 *
 */
namespace readelf
{

CReadElf::CReadElf(const char *szFileName)
{
	// open the file
	m_fElf.open(szFileName);

	// if we got the file
	if(m_fElf)
	{
		// read in the elf struct
		m_fElf.read(reinterpret_cast<char*>(&m_elf), sizeof(m_elf));

		// load the header struct
		SectionArray arSections(m_elf.e_shnum);
		m_fElf.seekg(m_elf.e_shoff);
		m_fElf.read(reinterpret_cast<char*>(&arSections[0]), arSections.size() * m_elf.e_shentsize);

		// read in the section string table
		m_fElf.seekg(arSections[m_elf.e_shstrndx].sh_offset);
		CharArray arSectionNames(arSections[m_elf.e_shstrndx].sh_size);
		m_fElf.read(&arSectionNames[0], arSectionNames.size());

		// now generate a map of names to sections
		for(SectionArray::iterator itSec = arSections.begin(), itEnd = arSections.end()
				; itSec != itEnd
				; ++itSec)
		{
			// go to the offset in the sectionnames as indicated by the section. That is the name
			// of the section (null terminated).
			m_mpSections.insert(std::make_pair(&arSectionNames[0] + itSec->sh_name, *itSec));
		}
	}
}

CReadElf::~CReadElf()
{
}

UCharArray CReadElf::getSection(const char *szSectionName)
{
	UCharArray arRetval;

	// try to find the section
	SectionMap::iterator itSec = m_mpSections.find(szSectionName);

	// if we found the section
	if(itSec != m_mpSections.end())
	{
		// seek to the section indicated
		m_fElf.seekg(itSec->second.sh_offset);

		// read the sections contents in
		arRetval.resize(itSec->second.sh_size);
		m_fElf.read(reinterpret_cast<char*>(&arRetval[0]), arRetval.size());
	}

	return arRetval;
}
}
