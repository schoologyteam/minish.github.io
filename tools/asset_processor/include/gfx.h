#include "asset.h"

class GfxAsset : public BaseAsset {
public:
    using BaseAsset::BaseAsset;
    virtual void convertToHumanReadable(const std::vector<char>& baserom);
};