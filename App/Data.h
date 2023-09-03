#pragma once
#include <string>
#include <vector>



enum class FileType
{
	NUL,
	SPR,
	R3T,
	UNKNOWN
};


struct BaseFile
{
	FileType type = FileType::NUL;
	FileType subType = FileType::NUL;
	std::string ext;
	std::string name;
	std::string path;
};

struct SprFile : BaseFile
{
	std::string data;
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

struct AssetKey
{
	FileType type;
	std::string key;

	bool operator==(const AssetKey& i) const {
		return type == i.type && _stricmp(key.c_str(), i.key.c_str()) < 0;
	}

	bool operator<(const AssetKey& i)  const {
		return type < i.type || (type == i.type && key < i.key);
	}
};

inline std::string operator+ (const std::string& os, FileType type)
{
	switch (type)
	{
	case FileType::NUL:		return os + ".nul";
	case FileType::R3T:		return os + ".r3t";
	case FileType::SPR:		return os + ".spr";
	}
	return os + ".nul";
}

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