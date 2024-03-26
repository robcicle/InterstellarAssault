#pragma once

#include "D3DUtil.h"

#include <SpriteFont.h>


// FontCache class: Manages and caches SpriteFonts to ensure unique font loading.
// It associates font file names with their corresponding SpriteFont resources.
class FontCache
{
public:
    // Data struct: Associates a file name with a SpriteFont resource.
    struct Data
    {
        Data() {} 
        // Overloaded constructor: Allows us to assign the fileName and sFont variables
        Data(const std::string& fName, DirectX::SpriteFont* s)
            : fileName(fName), sFont(s)
        {}
        std::string fileName;                  // File name of the font.
        DirectX::SpriteFont* sFont = nullptr;  // Pointer to the SpriteFont resource.
    };

    // Release function: Cleans up all loaded font resources.
    void Release();

    // LoadFont function: Loads a font from a file, or retrieves it if already loaded.
    DirectX::SpriteFont* LoadFont(ID3D11Device* pDevice, const std::string& fileName, const std::string& texName = "", bool appendPath = true);

    // SetAssetPath function: Sets the path to the font assets directory.
    void SetAssetPath(const std::string& path) {
        mAssetPath = path;
    }

    // GetAssetPath function: Returns the current font assets directory path.
    const std::string& GetAssetPath() const { return mAssetPath; }

    // Get function: Retrieves a Data instance by its texture name. Fast access method.
    Data& Get(const std::string& texName) {
        return mCache.at(texName);
    }

    // Get function: Retrieves a Data instance by its SpriteFont pointer. Slower access method.
    const Data& Get(DirectX::SpriteFont* sFont);

private:
    typedef std::unordered_map<std::string, FontCache::Data> MyMap; // Map associating texture names with Data instances.
    MyMap mCache;                                                   // Map holding all cached font data.

    std::string mAssetPath; // Path to the directory containing all font assets.
};