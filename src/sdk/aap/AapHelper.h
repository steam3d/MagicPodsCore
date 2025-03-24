#pragma once

#include "../AppleProductIds.h"
#include "../BtVendorIds.h"

namespace MagicPodsCore
{
    class AapHelper
    {
    private:
    public:
        static bool IsAapDevice(unsigned short vendorId, unsigned short productId);
    };
}
