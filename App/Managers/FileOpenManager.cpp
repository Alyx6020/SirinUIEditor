#include "FileOpenManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <functional>

//Open
void FileOpenManager::Open(std::filesystem::path path)
{
	ProcesssFile(ProcessFileFormat::PATH, path, "");
}

void FileOpenManager::Open(std::string inpath, std::string section, int offset, int size)
{
	if (inpath.empty())
	{
		return;
	}

	std::filesystem::path path(section);
	std::string data;

	std::ifstream in(inpath, std::ios_base::in | std::ios_base::binary);
	if (in.is_open())
	{
		in.ignore(offset);
		data.resize(size);
		in.read((char*)&data.front(), size);
		in.close();
	}

	ProcesssFile(ProcessFileFormat::RAW, path, data);
}

int FileOpenManager::ProcesssFile(ProcessFileFormat format, std::filesystem::path path, std::string data = "")
{
	//Transform extension upper;
	std::string extension = path.extension().string();
	std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);

	if (!pFileTypeMap.contains(extension))
	{
		return 0;
	}

	FileType type = pFileTypeMap.find(extension)->second;

	switch (type)
	{
	case FileType::R3T:
	{
		R3TFile r3t;

		r3t.ext = extension;
		r3t.name = path.stem().string();
		r3t.path = path.string();

		ProcesssR3TFile(&r3t);

		Add(FileType::R3T, r3t);
		

		break;
	}
	default:
		return 0;
	}
	return 1;
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