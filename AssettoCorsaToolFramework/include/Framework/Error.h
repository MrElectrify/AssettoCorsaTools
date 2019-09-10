#ifndef FRAMEWORK_ERROR_H_
#define FRAMEWORK_ERROR_H_

/*
 *	Error Code Implementation
 *	9/8/19 20:30
 */

#include <cstdint>

namespace Framework
{
	enum _ErrorCode
	{
		ErrorCode_SUCCESS = 0,	// operation completed successfully
		ErrorCode_FILENOTFOUND,	// file was not found
		ErrorCode_FILENOTOPEN,	// a file is not open
		ErrorCode_FORMAT,		// file had an invalid format
		ErrorCode_EOF,			// end of file was reached	
	};

	/*
	 *	ErrorCode provides an abstraction which can
	 *	be used to compare as an enumeration, containing
	 *	a lower level _ErrorCode underneath
	 */
	class ErrorCode
	{
	public:
		using RawCode_t = int32_t;

		// constructs an empty ErrorCode
		ErrorCode();
		// constructs with a raw _ErrorCode
		ErrorCode(RawCode_t code);

		// get the raw error code
		RawCode_t GetRawCode() const;
		// get a message representing the error code
		const char* GetMessage() const;

		// implicit conversion to the raw error
		operator RawCode_t() const;
	private:
		RawCode_t m_code;
		char m_message[128];
	};
}

#endif