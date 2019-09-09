#ifndef FRAMEWORK_FILEDECRYPTER_H_
#define FRAMEWORK_FILEDECRYPTER_H_

/*
 * File decrypter implementation
 * 9/8/19 20:24
 */

#include <fstream>
#include <string>
#include <vector>

#include <Framework/Files/File.h>
#include <Framework/Error.h>

namespace Framework
{
	namespace Files
	{
		/*
		 *	FileDecrypter uses Assetto Corsa's encrpyption techniques
		 *	to decrypt .acd files, and create a virtual filesystem
		 *	which contains these files, which can then be manipulated
		 */
		class FileDecrypter
		{
		public:
			// default constructor, throws ErrorCode on error. assumes directory only includes the name of the directory, and no other part of the path
			FileDecrypter(const std::string& fileName, const std::string& directory);
			// overload that does not throw, stores ErrorCode in ec on error. assumes directory only includes the name of the directory, and no other part of the path
			FileDecrypter(const std::string& fileName, const std::string& directory, ErrorCode& ec) noexcept;

			// get and decrypt the next file, and return the contents as a File. throws ErrorCode on error
			File DecryptNext();
			// get and decrypt the next file, and return the contents as a File. stores error in ec on error.
			File DecryptNext(ErrorCode& ec) noexcept;

			// get and decrypt all files, and return the contents as a vector of Files. throws ErrorCode on error. resets position
			std::vector<File> DecryptAll();
			// get and decrypt all files, and return the contents as a vector of Files. stores ErrorCode in ec on error. resets position
			std::vector<File> DecryptAll(ErrorCode& ec) noexcept;

			// get and decrypt a specific file, and return the contents as a File. throws ErrorCode on error, FILENOTFOUND if not found. resets position
			File DecryptFile(const std::string& fileName);
			// get and decrypt a specific file, and return the contents as a File. stores ErrorCode in ec on error, FILENOTFOUND if not found. resets position
			File DecryptFile(const std::string& fileName, ErrorCode& ec) noexcept;

			// close handles
			~FileDecrypter();
		private:
			// calculate the decryption key
			void CalculateKey(const std::string& directory);

			std::ifstream m_file;
			std::string m_key;
		};
	}
}

#endif