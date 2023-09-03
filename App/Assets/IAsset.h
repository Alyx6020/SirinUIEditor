#pragma once
#include <string>
#include "Graphics.h"
#include <filesystem>


template<typename T>
size_t vectorsizeof(const typename std::vector<T>& vec)
{
	return sizeof(T) * vec.size();
}

class IAsset
{
public:
	virtual ~IAsset() = default;
	void* assetInfo = nullptr;
	std::string GetName()
	{
		return name;
	};
	virtual bool Save(const std::filesystem::path& path) { return true; };
	virtual bool Cache(const std::filesystem::path& path = "") { return true; };
	virtual bool Add(const std::string& name) { return true; };
	virtual bool Remove(const std::string& name) { return true; };
	virtual IAsset* Extract(const std::string& name = "") { return nullptr; };

	std::filesystem::path path;
	std::string fullpath;
	std::pair<uint32_t, uint32_t> assetRange = {};
protected:
	std::string name{};
};