#include "FileOpenManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>
#include <Managers/AssetManager.h>

//Open

std::string FileOpenManager::GetFileContents(std::filesystem::path path)
{
	if (!std::filesystem::exists(path)) return "";
	try
	{
		std::ifstream in(path, std::ios::in | std::ios::binary);
		if (in)
		{
			std::ostringstream contents;
			contents << in.rdbuf();
			in.close();
			return(contents.str());
		}
	}
	catch (...)
	{
		return "";
	}
}

void FileOpenManager::Save(const std::string& originalFile, std::filesystem::path& path, const FileType& type)
{
	switch (type)
	{

	}
}

void FileOpenManager::Open(const std::filesystem::path& path, ProcessFileFormat format)
{
	try
	{
		ProcesssFile(format, path);
	}
	catch (const std::exception&)
	{
		return;
	}
}

void FileOpenManager::Open(const std::string& path, const std::string& section, int offset, int size)
{
	std::filesystem::path _path(section);
	std::string data;

	std::ifstream in(std::filesystem::path(path), std::ios_base::in | std::ios_base::binary);
	if (in.is_open())
	{
		in.ignore(offset);
		data.resize(size);
		in.read((char*)&data.front(), size);
		in.close();
	}

	ProcesssFile(ProcessFileFormat::RAW, _path, "", data);
}

void FileOpenManager::Open(const std::filesystem::path& name, const std::string& data, int offset, int size)
{
	std::string extension = name.extension().string();

	FileType type = fileManager::GetFileType(extension);

	ProcesssFile(ProcessFileFormat::RAW, name, name.stem().string(), data.substr(offset, size), type);
}

void FileOpenManager::Open(const std::filesystem::path& name, const std::filesystem::path& path, const std::string& data, int offset, int size)
{
	std::string extension = name.extension().string();
	FileType type = fileManager::GetFileType(extension);

	ProcesssFile(ProcessFileFormat::RAW, path, name.stem().string(), data.substr(offset, size), type);
}

int FileOpenManager::ProcesssFile(ProcessFileFormat format, const std::filesystem::path& path, const std::string& optionalName, const std::string& data, FileType type)
{
	std::string extension = path.extension().string();

	if (type == FileType::NUL)
	{
		type = fileManager::GetFileType(extension);
	}

	switch (type)
	{
	case FileType::SPR:
	{
		SprFile file;
		file.path = path.string();
		file.name = path.filename().string();
		file.data = GetFileContents(path);

		if (AssetManager::Instance().Register(FileType::SPR, file.path, file))
		{

		}

		break;
	}
	}

	return 0;
}


void FileOpenManager::Add(FileType type, BaseFile& file)
{
	switch (type)
	{
		case FileType::R3T:
		{
			R3TFile* temp = static_cast<R3TFile*>(&file);

			std::shared_ptr<R3TFile> item = std::make_shared<R3TFile>(temp->headers, temp->data);
		
			item->name = file.name;
			item->ext = file.ext;
			item->path = file.path;

			auto it = files.insert_or_assign({ FileType::R3T, file.name }, std::move(item));

			break;
		}
	}
}

void FileOpenManager::ProcesssR3TFile(R3TFile* file)
{
	std::ifstream in(file->path, std::ios_base::in | std::ios_base::binary);
	if (in.is_open())
	{
		int count;
		//Header count
		in.ignore(4);
		in.read((char*)&count, 4);
		file->headers.resize(count);
		in.read((char*)&file->headers.front(), sizeof(R3THeader) * count);

		for (auto& it : file->headers)
		{
			uint32_t size;
			in.read((char*)&size, 4);

			std::string str(size,' ');
			in.read(str.data(), size);

			for (size_t i = 0; i < 128; i++)
			{ 
				str.data()[i] ^= pDdsKey[i];
			}

			file->data.emplace_back(str);
		}
		in.close();
	}
}

