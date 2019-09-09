#include <Framework/Error.h>

#include <string.h>
#ifdef _WIN32
#include <Windows.h>
#undef GetMessage
#define strcpy(DEST, SOURCE) strcpy_s(DEST, SOURCE)
#elif __linux__
// use the xpg standard
#ifdef __cplusplus
extern "C"
{
#endif
	extern
		int __xpg_strerror_r(int errcode, char* buffer, size_t length);
#define strerror_r __xpg_strerror_r

#ifdef __cplusplus
}
#endif
#define strcpy(DEST, SOURCE) strncpy(DEST, SOURCE, sizeof(DEST) / sizeof(*DEST))
#endif

using Framework::_ErrorCode;
using Framework::ErrorCode;

ErrorCode::ErrorCode() : m_code(0)
{
	strcpy(m_message, "Code not set");
}

ErrorCode::ErrorCode(RawCode_t code) : m_code(code)
{
	switch (m_code)
	{
	case ErrorCode_SUCCESS:
		strcpy(m_message, "The operation completed successfully");
		break;
	case ErrorCode_FILENOTFOUND:
		strcpy(m_message, "The specified file was not found");
		break;
	case ErrorCode_FILENOTOPEN:
		strcpy(m_message, "A file is not open");
		break;
	case ErrorCode_FORMAT:
		strcpy(m_message, "The file format was invalid");
		break;
	case ErrorCode_EOF:
		strcpy(m_message, "The end of the file was reached");
		break;
	}
	return;
}

ErrorCode::RawCode_t ErrorCode::GetRawCode() const
{
	return m_code;
}

const char* ErrorCode::GetMessage() const
{
	return m_message;
}

ErrorCode::operator ErrorCode::RawCode_t() const
{
	return m_code;
}