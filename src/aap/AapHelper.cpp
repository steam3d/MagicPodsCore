#include "AapHelper.h"

namespace MagicPodsCore
{
    bool AapHelper::IsAapDevice(unsigned short vendorId, unsigned short productId)
    {
        if (vendorId == static_cast<unsigned short>(BtVendorIds::Apple))
        {
            for (auto &appleProductId : AllAppleProductIds)
            {
                if (productId == static_cast<unsigned short>(appleProductId))
                    return true;
            }
        }

        return false;
    }
}