#include <Framework/Files/File.h>

using Framework::Files::File;

File::File(const std::string& name, const std::string& contents)
	: m_name(name), m_contents(contents) {}

File::File(File&& other)
{
	m_name = std::move(other.m_name);
	m_contents = std::move(other.m_contents);

	other.m_name = "";
	other.m_contents = "";
}

std::string File::GetName() const noexcept
{
	return m_name;
}

std::string File::GetContents() const noexcept
{
	return m_contents;
}