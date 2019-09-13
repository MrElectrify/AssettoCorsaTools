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
			using Data_t = std::string;

			// constructor for a file, read-only once created
			File(const Data_t& name, const Data_t& contents);

			Data_t GetName() const noexcept;
			Data_t GetContents() const noexcept;
		private:
			Data_t m_name;
			Data_t m_contents;
		};

	}
}

#endif