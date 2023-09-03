#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <filesystem>
#include <map>
#include "data.h"
#include <map>

namespace fileManager
{
	static std::map<std::string, FileType> pFileTypeMap = {
		{".r3t",FileType::R3T},
		{".spr",FileType::SPR},
	};

	static inline FileType GetFileType(const std::string& extension)
	{
		std::string ext(extension);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

		auto iter = pFileTypeMap.find(ext);

		if (iter != pFileTypeMap.end())
		{
			return iter->second;
		}

		return FileType::NUL;
	}
}

class FileOpenManager
{
public:
	static FileOpenManager& Get()
	{
		static FileOpenManager instance;
		return instance;
	}

	std::string GetFileContents(std::filesystem::path path);

	//Save handler
	void Save(const std::string& originalFile, std::filesystem::path& path, const FileType& type = FileType::NUL);
	//Open handler
	void Open(const std::filesystem::path& path, ProcessFileFormat type = ProcessFileFormat::PATH);
	void Open(const std::string& path, const std::string& section, int offset, int size);
	void Open(const std::filesystem::path& name, const std::string& data, int offset, int size);
	void Open(const std::filesystem::path& name, const std::filesystem::path& path, const std::string& data, int offset, int size);

	BaseFile* Get(FileType type, const std::string& str)
	{
		auto iter = files.find({ type,str });

		if (iter != files.end())
		{
			return iter->second.get();
		}
		return nullptr;
	}

	void Erase(FileType type, const std::string& str)
	{
		files.erase({ type,str });
	}

private:

	unsigned char pDdsKey[128] = { 46, 128, 77, 118, 46, 248, 209, 240, 189, 63,
		134, 129, 88, 44, 63, 63, 46, 46, 103, 111, 63, 64, 63, 120, 60, 63, 241, 192, 165, 246, 59, 159, 193, 32, 63, 215, 200, 193, 233, 133,
		134, 189, 239, 86, 63, 161, 251, 46, 135, 134, 97, 76, 33, 59, 78, 180, 120, 87, 174, 151, 63, 46, 74, 46, 63, 76, 46, 68, 205, 197,
		95, 232, 233, 236, 235, 189, 190, 187, 247, 108, 46, 242, 228, 46, 63, 63, 151, 159, 157, 179, 33, 185, 118, 101, 84, 63, 230, 246, 198, 240,
		121, 219, 226, 178, 75, 46, 46, 235, 211, 211, 202, 171, 234, 199, 237, 156, 199, 217, 208, 101, 72, 180, 250, 53, 46, 46, 106, 155 };


	FileOpenManager() {};
	FileOpenManager(FileOpenManager const&) = delete;
	void operator=(FileOpenManager const&) = delete;


	void Add(FileType, BaseFile& file);

	

	int ProcesssFile(ProcessFileFormat format, const std::filesystem::path& path, const std::string& optionalName = "", const std::string& data = "", FileType type = FileType::NUL);
	void ProcesssR3TFile(R3TFile* file);

	std::map<std::string, FileType> pFileTypeMap = {
		{".R3T",FileType::R3T}
	};

	std::map<Key, std::shared_ptr<BaseFile>> files;

public:
};