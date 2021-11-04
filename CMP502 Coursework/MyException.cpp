#include "MyException.h"

#include <sstream>


MyException::MyException(int line, const char* file) noexcept :
	m_line(line), m_file(file)
{
}


const char* MyException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}


const char* MyException::GetType() const noexcept
{
	return "MyException";
}


