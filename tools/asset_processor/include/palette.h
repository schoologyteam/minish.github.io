#include "asset.h"

class PaletteAsset : public BaseAsset {
public:
    using BaseAsset::BaseAsset;
    virtual void convertToHumanReadable(const std::vector<char>& baserom);
};