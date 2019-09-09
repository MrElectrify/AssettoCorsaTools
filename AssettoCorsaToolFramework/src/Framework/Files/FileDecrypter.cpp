#include <Framework/Files/FileDecrypter.h>

#include <cstdint>
#include <vector>

using Framework::ErrorCode;
using Framework::Files::File;
using Framework::Files::FileDecrypter;

FileDecrypter::FileDecrypter(const std::string& fileName, const std::string& directory) 
	: m_file(fileName, std::ios_base::binary)
{
	// if the file was not opened properly, throw an error
	if (m_file.good() == false)
		throw ErrorCode(ErrorCode_FILENOTFOUND);

	CalculateKey(directory);
}

FileDecrypter::FileDecrypter(const std::string& fileName, const std::string& directory, ErrorCode& ec) noexcept
	: m_file(fileName, std::ios_base::binary)
{
	// no need to waste processing power on calculating the key if the file is not found
	if (m_file.good() == false)
	{
		ec = ErrorCode_FILENOTFOUND;
		return;
	}
	
	CalculateKey(directory);
}

File FileDecrypter::DecryptNext()
{
	/*
	 *	This mimics the behavior of FolderEncrypter::decryptFile
	 */

	int32_t size;

	// make sure we are working on an open and accessible file
	if (m_file.good() == false)
		throw ErrorCode(ErrorCode_FILENOTOPEN);

	// try to read the size of the name
	if (m_file.read(reinterpret_cast<char*>(&size), 4).fail() == true)
		throw ErrorCode(ErrorCode_EOF);

	// corner case, just advance to position 8
	if (size == -1111)
	{
		m_file.seekg(8);
		if (m_file.read(reinterpret_cast<char*>(&size), 4).fail() == true)
			throw ErrorCode(ErrorCode_EOF);
	}

	std::string name(size, '\0');
	
	// read the name from the file
	if (m_file.read(&name[0], size).fail() == true)
		throw ErrorCode(ErrorCode_EOF);

	// read the size of the file
	if (m_file.read(reinterpret_cast<char*>(&size), 4).fail() == true)
		throw ErrorCode(ErrorCode_EOF);

	std::string decContents;

	if (size > 0)
	{
		std::vector<int> rawContents(size);

		// resize decContents to fit the contents
		decContents.resize(size);

		// read the file as integers into the vector
		if (m_file.read(reinterpret_cast<char*>(&rawContents[0]), size * 4).fail() == true)
			throw ErrorCode(ErrorCode_EOF);

		// decrypt the contents
		for (size_t i = 0; i < size; ++i)
		{
			decContents[i] = static_cast<char>(rawContents[i]) - m_key[i % m_key.size()];
		}
	}

	return File(name, decContents);
}

File FileDecrypter::DecryptNext(ErrorCode& ec) noexcept
{
	try
	{
		return DecryptNext();
	}
	catch (const ErrorCode& e)
	{
		ec = e;
		return File("", "");
	}
}

std::vector<File> FileDecrypter::DecryptAll()
{
	// reset position for Next() to make sure we get all of the files
	m_file.seekg(0);

	std::vector<File> files;
	// fill our file vector with files until we reach the end of file
	try
	{
		while (true)
		{
			files.emplace_back(std::move(DecryptNext()));
		}
	}
	catch (const ErrorCode& e)
	{
		if (e != ErrorCode_EOF)
			throw e;
	}
	
	return files;
}

std::vector<File> FileDecrypter::DecryptAll(ErrorCode& ec) noexcept
{
	try
	{
		return DecryptAll();
	}
	catch (const ErrorCode& e)
	{
		ec = e;
		return std::vector<File>();
	}
}

File FileDecrypter::DecryptFile(const std::string& fileName)
{
	// reset position for Next() to make sure we get all of the files
	m_file.seekg(0);

	// keep getting files until we find the one we are looking for
	try
	{
		while (true)
		{
			File f = DecryptNext();
			if (f.GetName() == fileName)
				return f;
		}
	}
	catch (const ErrorCode& e)
	{
		if (e != ErrorCode_EOF)
			throw e;
	}

	// throw EOF if we can't find it
	throw ErrorCode(ErrorCode_FILENOTFOUND);
}

File FileDecrypter::DecryptFile(const std::string& fileName, ErrorCode& ec) noexcept
{
	try
	{
		return DecryptFile(fileName);
	}
	catch (const ErrorCode& e)
	{
		ec = e;
		return File("", "");
	}
}

FileDecrypter::~FileDecrypter()
{
	m_file.close();
}

void FileDecrypter::CalculateKey(const std::string& directory)
{
	/*
	 *	This mimics the behavior of Assetto Corsa's ksSecurity::keyFromString
	 */

	uint8_t factor0 = 0;

	// add all chars, as in AC
	for (auto c : directory)
		factor0 += c;

	// another strange sum
	int32_t factor1 = 0;
	for (size_t i = 0; i < directory.size() - 1; i += 2)
	{
		auto tmp = directory[i] * factor1;
		factor1 = tmp - directory[i + 1];
	}

	// another strange sum
	int32_t factor2 = 0;
	for (size_t i = 1; i < directory.size() - 3; i += 3)
	{
		auto tmp0 = directory[i] * factor2;
		auto tmp1 = tmp0 / (directory[i + 1] + 27);
		factor2 = -27 - directory[i - 1] + tmp1;
	}

	// yet another strange sum
	int8_t factor3 = -125;
	for (size_t i = 1; i < directory.size(); ++i)
	{
		factor3 -= directory[i];
	}

	// of course, another strange sum
	int32_t factor4 = 66;
	for (size_t i = 1; i < directory.size() - 4; i += 4)
	{
		auto tmp = (directory[i] + 15) * factor4;
		factor4 = (directory[i - 1] + 15) * tmp + 22;
	}

	// yup, you guessed it
	uint8_t factor5 = 101;
	for (size_t i = 0; i < directory.size() - 2; i += 2)
	{
		factor5 -= directory[i];
	}

	// not even a purpose in commenting these anymore
	int32_t factor6 = 171;
	for (size_t i = 0; i < directory.size() - 2; i += 2)
	{
		factor6 %= directory[i];
	}

	// last one, finally
	int32_t factor7 = 171;
	for (size_t i = 0; i < directory.size() - 1;)
	{
		auto tmp = factor7 / directory[i];
		factor7 = directory[++i] + tmp;
	}

	// output in some strange format as in AC
	char tmp[0x100];
	sprintf_s(tmp, "%d-%d-%d-%d-%d-%d-%d-%d",
		uint8_t(factor0), uint8_t(factor1), uint8_t(factor2), uint8_t(factor3),
		uint8_t(factor4), uint8_t(factor5), uint8_t(factor6), uint8_t(factor7));

	m_key = tmp;
}