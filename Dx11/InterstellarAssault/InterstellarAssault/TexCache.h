#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <d3d11.h>

#include "D3DUtil.h"

// RECTF Struct: Defines a rectangle with floating-point coordinates.
struct RECTF
{
	float left, top, right, bottom;

	// Conversion operator to convert RECTF to Windows RECT structure.
	operator RECT() {
		return RECT{ (int)left,(int)top,(int)right,(int)bottom };
	}

	// Assignment operator to set RECTF values.
	RECTF operator =(RECTF rhs) {
		left = rhs.left;
		top = rhs.top;
		right = rhs.right;
		bottom = rhs.bottom;
		return *this;
	}

	// Addition operator to add another RECTF's dimensions.
	RECTF operator +=(RECTF rhs) {
		left += rhs.left;
		top += rhs.top;
		right += rhs.right;
		bottom += rhs.bottom;
		return *this;
	}

	// Subtraction operator to subtract another RECTF's dimensions.
	RECTF operator -=(RECTF rhs) {
		left -= rhs.left;
		top -= rhs.top;
		right -= rhs.right;
		bottom -= rhs.bottom;
		return *this;
	}
};

// TexCache Class: Manages texture resources to ensure each texture is only loaded once.
class TexCache
{
public:
	// Data Struct: Represents a texture along with its filename, dimensions, and frame data.
	struct Data
	{
		Data() {}
		Data(const std::string& fName, ID3D11ShaderResourceView* p, const DirectX::SimpleMath::Vector2& _dim)
			: fileName(fName), pTex(p), dim(_dim)
		{
			frames.clear();
		}
		Data(const std::string& fName, ID3D11ShaderResourceView* p, const DirectX::SimpleMath::Vector2& _dim, const std::vector<RECTF>* _frames)
			: fileName(fName), pTex(p), dim(_dim)
		{
			if (_frames)
				frames = *_frames;
		}
		std::string fileName;
		ID3D11ShaderResourceView* pTex = nullptr;
		DirectX::SimpleMath::Vector2 dim;  // Texture dimensions.
		std::vector<RECTF> frames;  // Frame data for animated textures.
	};

	// Release all textures managed by this cache.
	void Release();

	// Load texture if it's new, or return handle if already loaded.
	ID3D11ShaderResourceView* LoadTexture(ID3D11Device* pDevice, const std::string& fileName, const std::string& texName = "", bool appendPath = true, const std::vector<RECTF>* _frames = nullptr);

	// Set and get asset path for textures.
	void SetAssetPath(const std::string& path) { mAssetPath = path; }
	const std::string& GetAssetPath() const { return mAssetPath; }

	// Retrieve texture data by nickname for fast access.
	Data& Get(const std::string& texName) { return mCache.at(texName); }

	// Find a texture by handle (slower method).
	const Data& Get(ID3D11ShaderResourceView* pTex);

private:
	// Get the dimensions of a texture.
	DirectX::SimpleMath::Vector2 GetDimensions(ID3D11ShaderResourceView* pTex);

	typedef std::unordered_map<std::string, Data> MyMap;  // Hash map for texture data.
	MyMap mCache;  // Cache of texture data.

	std::string mAssetPath;  // Asset path for textures.
};