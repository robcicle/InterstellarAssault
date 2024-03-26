#include "FontCache.h"

#include <filesystem>

using namespace DirectX;
using namespace DirectX::SimpleMath;


// Release function: Cleans up all loaded font resources.
void FontCache::Release()
{
	// Iterate through each cached font and delete its SpriteFont resource.
	for (auto& pair : mCache)
		delete pair.second.sFont;

	// Clear the cache to remove all entries.
	mCache.clear();
}

// LoadFont function: Loads a font from a file, or retrieves it if already loaded.
SpriteFont* FontCache::LoadFont(ID3D11Device* pDevice, const std::string& fileName,
	const std::string& texName, bool appendPath)
{
	// Determine the texture name, defaulting to the file stem if not provided.
	std::string name = texName;
	if (name.empty())
	{
		std::filesystem::path p(fileName);
		name = p.stem().string();
	}

	// Search the cache for the font.
	MyMap::iterator it = mCache.find(name);
	if (it != mCache.end())
		return (*it).second.sFont;

	// Prepare the full path for the font file if required.
	const std::string* pPath = &fileName;
	std::string path;
	if (appendPath)
	{
		path = mAssetPath + fileName;
		pPath = &path;
	}
	// Convert string path to wstring for API compatibility.
	std::wstring ws(pPath->begin(), pPath->end());
	// Load the font from the file.
	SpriteFont* sF = new SpriteFont(pDevice, ws.c_str());

	if (sF == nullptr)
	{
		DBOUT("Cannot load " << *pPath << "\n");
		assert(false);
	}
	// Save the loaded font into the cache.
	assert(sF);
	mCache.insert(MyMap::value_type(name, Data(fileName, sF)));
	return sF;
}

// Get function: Retrieves a Data instance by its SpriteFont pointer. Slower access method.
const FontCache::Data& FontCache::Get(SpriteFont* sFont) {

	MyMap::iterator it = mCache.begin();
	Data* p = nullptr;
	// Iterate through the cache to find the Data instance matching the SpriteFont pointer.
	while (it != mCache.end() && !p)
	{
		if ((*it).second.sFont == sFont)
			p = &(*it).second;
		++it;
	}
	assert(p); // Ensure the font was found.
	return *p;
}