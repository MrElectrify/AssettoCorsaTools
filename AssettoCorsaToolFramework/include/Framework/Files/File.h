#ifndef FRAMEWORK_FILES_FILE_H_
#define FRAMEWORK_FILES_FILE_H_

/*
 *	File
 *	9/8/19 21:21
 */

#include <string>

namespace Framework
{
	namespace Files
	{
		/*
		 *	File is a class that holds a file's name and contents
		 */
		class File
		{
		public:
			// constructor for a file, read-only once created
			File(const std::string& name, const std::string& contents);
			// move
			File(File&& other);

			std::string GetName() const noexcept;
			std::string GetContents() const noexcept;
		private:
			std::string m_name;
			std::string m_contents;
		};

	}
}

#endif