#include <filesystem>
#include "Assets/SprAsset.h"

namespace spr
{
	enum class SpriteViewMode : uint32_t
	{
		Fullsize = 0,
		Medium = 300,
		Small = 150,
	};

	struct SprPayload
	{
		char path[512] = {};
		uint32_t group;
		uint32_t frame;

		SprPayload(const std::string& _path, uint32_t group, uint32_t frame)
			:
			group(group), frame(frame)
		{
			memcpy(path, _path.data(), _path.size());
		}
	};

	class SprViewerGui
	{
	public:
		SprViewerGui(std::filesystem::path path, SprAsset* asset)
			:
			m_sprPath(path),
			spr(asset)
		{
			pUid = reinterpret_cast<intptr_t>(&*this);
		}
		~SprViewerGui() = default;
		void Render(std::pair<int, int> windowSize);

	public:
		bool pDestory = false;
		SprAsset* spr;
	private:
		bool pOpen = true;
		int64_t pUid = 0;
		std::filesystem::path m_sprPath;
		std::string pSelectedName;
	};

	extern std::vector<std::unique_ptr<SprViewerGui>> m_sprViewerGui;

	struct Directory
	{
		std::filesystem::path path;
		std::string filename;
		std::vector<Directory> Children;
		bool IsDirectory;
	};

	

	void AddDirectoryNodes(Directory& parentNode, std::filesystem::directory_iterator directoryIterator);
	Directory CreateDirectryFromPath(const std::filesystem::path& rootPath);
	void DisplayDirectory(const Directory& parentNode);
	
	void Load();
	void Render();
}