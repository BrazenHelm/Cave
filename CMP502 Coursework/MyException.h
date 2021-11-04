#pragma once
#include <exception>
#include <string>

class MyException : public std::exception
{
public:
	MyException(int line, const char* file) noexcept;
	const char* what() const noexcept override;

	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept { return m_line; }
	const std::string& GetFile() const noexcept { return m_file; }
	std::string GetOriginString() const noexcept
	{
		std::string originString;
		originString += "Line: " + m_line;
		originString += ", File: " + m_file;
		return originString;
	}

private:
	int m_line;
	std::string m_file;

protected:
	mutable std::string whatBuffer;
};

