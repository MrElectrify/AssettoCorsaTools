#include <Framework/Error.h>

using Framework::_ErrorCode;
using Framework::ErrorCode;

ErrorCode::ErrorCode() : m_code(ErrorCode_SUCCESS)
{
	m_message = "The operation completed successfully";
}

ErrorCode::ErrorCode(RawCode_t code) : m_code(code)
{
	switch (m_code)
	{
	case ErrorCode_SUCCESS:
		m_message = "The operation completed successfully";
		break;
	case ErrorCode_FILENOTFOUND:
		m_message = "The specified file was not found";
		break;
	case ErrorCode_FILENOTOPEN:
		m_message = "A file is not open";
		break;
	case ErrorCode_FORMAT:
		m_message = "The file format was invalid";
		break;
	case ErrorCode_EOF:
		m_message = "The end of the file was reached";
		break;
	}
	return;
}

ErrorCode::RawCode_t ErrorCode::GetRawCode() const
{
	return m_code;
}

std::string ErrorCode::GetMessage() const
{
	return m_message;
}

ErrorCode::operator ErrorCode::RawCode_t() const
{
	return m_code;
}