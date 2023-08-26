#pragma once
#include <string>
#include <vector>

struct BaseFile
{
	std::string ext;
	std::string name;
	std::string path;
};

#pragma pack(push,r1,1)
struct R3THeader
{
	char filename[128];
};
#pragma pack(pop,r1)

struct R3TFile : BaseFile
{
	R3TFile() {};

	R3TFile(const std::vector<R3THeader>& headers, const std::vector<std::string>& data)
		:
		headers(headers),
		data(data)
	{
	}

	std::vector<R3THeader> headers;
	std::vector<std::string> data;
};


enum class FileType
{
	R3T,
	UNKNOWN
};

enum class ProcessFileFormat
{
	PATH,
	RAW
};

struct Key
{
	FileType type;
	std::string name;

	bool operator==(const Key& i) const
	{
		return type == i.type && name == i.name;
	}

	bool operator<(const Key& i) const
	{
		return type < i.type || (type == i.type && name < i.name);
	}
};