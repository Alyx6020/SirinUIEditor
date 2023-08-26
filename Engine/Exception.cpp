#include "Exception.h"
#include <sstream>

Exception::Exception(int line, const char* file) noexcept
	:line(line),file(file)
{
}

const char* Exception::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	buffer = oss.str();
	return buffer.c_str();

}

const char* Exception::GetType() const noexcept
{
	return "BSP ExportTool: Exception";
}

int Exception::GetLine() const noexcept
{
	return line;
}

const std::string& Exception::GetFile() const noexcept
{
	return file;
}

std::string Exception::GetOriginString() const noexcept
{
	std::ostringstream oss;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	errno_t err = _splitpath_s(file.c_str(), drive, dir, fname, ext);

	std::string str(fname);

	oss << "File: " << fname << ext << std::endl << "Line: " << line;
	return oss.str();
}
