#include <Framework/Files/FileManager.h>

#include <fstream>

using Framework::ErrorCode;
using Framework::Files::File;
using Framework::Files::FileManager;

FileManager::FileManager(const std::string& fileName, const std::string& directory, Mode_t mode)
	: m_mode(mode)
{
	CalculateKey(directory);

	if (mode & MODE_READ)
	{
		// create a temporary view of the file
		std::ifstream fileIn(fileName, std::ios::binary);

		// if the file was not opened properly, throw an error
		if (fileIn.good() == false)
			throw ErrorCode(ErrorCode_FILENOTFOUND);

		DecryptFiles(fileIn);

		fileIn.close();
	}
}

FileManager::FileManager(const std::string& fileName, const std::string& directory, Mode_t mode, ErrorCode& ec) noexcept
	: m_mode(mode)
{
	CalculateKey(directory);

	if (mode & MODE_READ)
	{
		// create a temporary view of the file
		std::ifstream fileIn(fileName, std::ios::binary);

		// if the file was not opened properly, throw an error
		if (fileIn.good() == false)
		{
			ec = ErrorCode_FILENOTFOUND;
			return;
		}

		DecryptFiles(fileIn);

		fileIn.close();
	}

	CalculateKey(directory);
}

File FileManager::GetFile(const std::string& fileName) const
{
	// try to find the file in our vector of files
	for (const auto& file : m_files)
	{
		if (file.GetName() == fileName)
			return file;
	}
	
	throw ErrorCode(ErrorCode_FILENOTFOUND);
}

File FileManager::GetFile(const std::string& fileName, ErrorCode& ec) const noexcept
{
	try
	{
		return GetFile(fileName);
	}
	catch (const ErrorCode& e)
	{
		ec = e;
	}
	return File("", "");
}

FileManager::Vec_t FileManager::GetFiles() const noexcept
{
	return m_files;
}

void FileManager::CalculateKey(const std::string& directory)
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

	constexpr size_t SIZE = 0x100;

	// output in some strange format as in AC
	char tmp[SIZE];
	snprintf(tmp, SIZE, "%d-%d-%d-%d-%d-%d-%d-%d",
		uint8_t(factor0), uint8_t(factor1), uint8_t(factor2), uint8_t(factor3),
		uint8_t(factor4), uint8_t(factor5), uint8_t(factor6), uint8_t(factor7));

	m_key = tmp;
}

void FileManager::DecryptFiles(std::ifstream& fileIn)
{
	/*
	 *	This mimics the behavior of FolderEncrypter::decryptFile
	 */

	while (true)
	{
		int32_t size;

		// make sure we are working on an open and accessible file
		if (fileIn.good() == false)
			throw ErrorCode(ErrorCode_FILENOTOPEN);

		// try to read the size of the name
		if (fileIn.read(reinterpret_cast<char*>(&size), 4).fail() == true)
			break;

		// corner case, just advance to position 8
		if (size == -1111)
		{
			fileIn.seekg(8);
			if (fileIn.read(reinterpret_cast<char*>(&size), 4).fail() == true)
				break;
		}

		std::string name(size, '\0');

		// read the name from the file
		if (fileIn.read(&name[0], size).fail() == true)
			break;

		// read the size of the file
		if (fileIn.read(reinterpret_cast<char*>(&size), 4).fail() == true)
			break;

		std::string decContents;

		if (size > 0)
		{
			std::vector<int> rawContents(size);

			// resize decContents to fit the contents
			decContents.resize(size);

			// read the file as integers into the vector
			if (fileIn.read(reinterpret_cast<char*>(&rawContents[0]), size * 4).fail() == true)
				break;

			// decrypt the contents
			for (size_t i = 0; i < size; ++i)
			{
				decContents[i] = static_cast<char>(rawContents[i]) - m_key[i % m_key.size()];
			}
		}
		
		// add the file to the vector
		m_files.emplace_back(name, decContents);
	}
}