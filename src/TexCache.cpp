#include "TexCache.h"

#include <DDSTextureLoader.h>
#include <filesystem>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

// Release function: Releases all textures stored in the cache.
void TexCache::Release()
{
	for (auto& pair : mCache)
		ReleaseCOM(pair.second.pTex);  // Release each texture resource.
	mCache.clear();  // Clear the texture cache.
}

// LoadTexture function: Loads a texture, either from the cache or from the file.
ID3D11ShaderResourceView* TexCache::LoadTexture(ID3D11Device* pDevice, const std::string& fileName, const std::string& texName,
	bool appendPath, const vector<RECTF>* frames)
{
	string name = texName;
	// Generate a texture name from the file name if texName is empty.
	if (name.empty())
	{
		std::filesystem::path p(fileName);
		name = p.stem().string();
	}

	// Search the cache for the texture.
	MyMap::iterator it = mCache.find(name);
	if (it != mCache.end())
		return (*it).second.pTex;  // Return the cached texture.

	// Prepare the file path for loading.
	const string* pPath = &fileName;
	string path;
	if (appendPath)
	{
		path = mAssetPath + fileName;  // Append the asset path.
		pPath = &path;
	}
	std::wstring ws(pPath->begin(), pPath->end());

	// Load the texture from the file.
	DDS_ALPHA_MODE alpha;
	ID3D11ShaderResourceView* pT;
	if (CreateDDSTextureFromFile(pDevice, ws.c_str(), nullptr, &pT, 0, &alpha) != S_OK)
	{
		DBOUT("Cannot load " << *pPath << "\n");
		assert(false);
	}

	// Save the loaded texture in the cache.
	assert(pT);
	mCache.insert(MyMap::value_type(name, Data(fileName, pT, GetDimensions(pT), frames)));
	return pT;
}

// Get function: Finds a texture by its DirectX handle.
const TexCache::Data& TexCache::Get(ID3D11ShaderResourceView* pTex)
{
	MyMap::iterator it = mCache.begin();
	Data* p = nullptr;
	while (it != mCache.end() && !p)
	{
		if ((*it).second.pTex == pTex)
			p = &(*it).second;
		++it;
	}
	assert(p);
	return *p;
}

// GetDimensions function: Retrieves the dimensions of the texture.
Vector2 TexCache::GetDimensions(ID3D11ShaderResourceView* pTex)
{
	assert(pTex);
	ID3D11Resource* res = nullptr;
	pTex->GetResource(&res);
	assert(res);
	ID3D11Texture2D* texture2d = nullptr;
	HRESULT hr = res->QueryInterface(&texture2d);
	Vector2 dim(0, 0);
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		texture2d->GetDesc(&desc);
		dim.x = static_cast<float>(desc.Width);
		dim.y = static_cast<float>(desc.Height);
	}
	ReleaseCOM(texture2d);
	ReleaseCOM(res);
	return dim;
}