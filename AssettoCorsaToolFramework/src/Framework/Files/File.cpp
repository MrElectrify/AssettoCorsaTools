#include <Framework/Files/File.h>

using Framework::Files::File;

File::File(const Data_t& name, const Data_t& contents)
	: m_name(name), m_contents(contents) {}


File::Data_t File::GetName() const noexcept
{
	return m_name;
}

File::Data_t File::GetContents() const noexcept
{
	return m_contents;
}