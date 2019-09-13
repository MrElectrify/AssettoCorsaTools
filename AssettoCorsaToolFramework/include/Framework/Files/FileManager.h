#ifndef FRAMEWORK_FILES_FILEMANAGER_H_
#define FRAMEWORK_FILES_FILEMANAGER_H_

/*
 *	File Manager implementation
 *	9/12/19	22:47
 */

#include <Framework/Error.h>
#include <Framework/Files/File.h>

#include <string>
#include <vector>

namespace Framework
{
	namespace Files
	{
		/*
		 *	FileManager uses Assetto Corsa's encrpyption techniques
		 *	to decrypt and encrypt .acd files, and create a virtual filesystem
		 *	which contains these files, which can then be manipulated
		 */
		class FileManager
		{
		public:
			using Key_t = std::string;
			using Vec_t = std::vector<File>;

			typedef enum MODE
			{
				MODE_READ = (1 << 0),						// read the file only, no encrypted output capabilites
				MODE_WRITE = (1 << 1),						// write and encryption ability
				MODE_READWRITE = MODE_READ | MODE_WRITE,	// read and write ability
			} Mode_t;

			// Default constructor, throws ErrorCode on error. Assumes directory only includes the name of the directory, and no other part of the path
			FileManager(const std::string& fileName, const std::string& directory, Mode_t mode);
			// Overload that does not throw, stores ErrorCode in ec on error. Assumes directory only includes the name of the directory, and no other part of the path
			FileManager(const std::string& fileName, const std::string& directory, Mode_t mode, ErrorCode& ec) noexcept;

			// Gets a single file by name. Throws ErrorCode on error
			File GetFile(const std::string& fileName) const;
			// Gets a single file by name. Stores ErrorCode in ec on error
			File GetFile(const std::string& fileName, ErrorCode& ec) const noexcept;

			// Gets all files
			Vec_t GetFiles() const noexcept;
		private:
			// calculate the decryption key
			void CalculateKey(const std::string& directory);
			// decrypt all of the files and populate m_files, used with MODE_READ
			void DecryptFiles(std::ifstream& fs);

			Key_t m_key;
			Mode_t m_mode;

			Vec_t m_files;
		};
	}
}

#endif