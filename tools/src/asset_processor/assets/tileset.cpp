#include "tileset.h"
#include "util.h"

std::filesystem::path TilesetAsset::generateAssetPath() {
    std::filesystem::path pngPath = path;
    if (pngPath.extension() == ".lz") {
        pngPath.replace_extension("");
    }
    pngPath.replace_extension(".png");
    return pngPath;
}

void TilesetAsset::convertToHumanReadable(const std::vector<char>& baserom) {
    (void)baserom;

    std::filesystem::path toolsPath = "tools";
    std::vector<std::string> cmd;

    std::filesystem::path decompressedPath = path;

    if (isCompressed()) {
        // First decompress.
        decompressedPath.replace_extension("");
        cmd.push_back(toolsPath / "bin" / "gbagfx");
        cmd.push_back(path);
        cmd.push_back(decompressedPath);
        check_call(cmd);
        cmd.clear();
    }

    cmd.push_back(toolsPath / "bin" / "gbagfx");
    cmd.push_back(decompressedPath);
    cmd.push_back(assetPath);
    cmd.push_back("-mwidth");
    cmd.push_back("32");
    check_call(cmd);
}

void TilesetAsset::buildToBinary() {
    std::filesystem::path toolsPath = "tools";
    std::vector<std::string> cmd;

    std::filesystem::path decompressedPath = path;
    if (isCompressed()) {
        decompressedPath.replace_extension("");
    }

    cmd.push_back(toolsPath / "bin" / "gbagfx");
    cmd.push_back(assetPath);
    cmd.push_back(decompressedPath);
    check_call(cmd);

    if (isCompressed()) {
        // Compress.
        cmd.push_back(toolsPath / "bin" / "gbagfx");
        cmd.push_back(decompressedPath);
        cmd.push_back(path);
        check_call(cmd);
    }
}

bool TilesetAsset::isCompressed() {
    return path.extension() == ".lz";
}