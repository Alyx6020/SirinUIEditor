#pragma once
#include "Data.h"
#include "IAsset.h"
#include <sstream>
#include "Graphics.h"
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTex.h>
#include <GraphicsResource.h>
#include <fstream>
#include <GraphicsResource.h>


struct SprHeader
{
	float version;
	uint32_t actions;
};

namespace spr
{
	struct Point 
	{
		double x, y = 0;
	};

	struct Rect {
		double x, y, w, h = 0;

		bool contains(const Point& p) {
			return p.x >= x && p.y >= y && p.x <= x + w && p.y <= y + h;
		}
	};

	enum class Center
	{
		Vertical,
		Horiziontal,
	};
}

struct Frame
{
	char format[4];
	int width;
	int height;
	int xwidth;
	int xheight;
	int linearSize;
	std::string data;

	std::string varName = "noname";
	spr::Rect rect = {};

	uint32_t group = -1;
	uint32_t frame = -1;
	float x = 0;
	float y = 0;
	float cursorx = 0;
	float cursory = 0;
	float tempx = 0;
	float tempy = 0;
	bool isDragged = false;
	std::filesystem::path path;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

	void Center(ImVec2 min, ImVec2 max, spr::Center);
};

struct Actions
{
	uint32_t frameCount;
	std::vector<Frame> frames;
};

class SprAsset : public IAsset
{
public:
	SprHeader m_header;
	std::vector<Actions> m_actionList;

	Frame* GetFrame(int group, int frame);

	SprAsset(const std::string& filename, const std::filesystem::path& path, const std::string& data, SprFile* file = nullptr);;

	std::string GetName();;
};
