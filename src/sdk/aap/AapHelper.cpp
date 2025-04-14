// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2025 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "AapHelper.h"
#include "sdk/aap/enums/AapModelIds.h"
#include "BtVendorIds.h"

namespace MagicPodsCore
{
    bool AapHelper::IsAapDevice(unsigned short vendorId, unsigned short productId)
    {
        if (vendorId == static_cast<unsigned short>(BtVendorIds::Apple))
        {
            for (auto &appleProductId : AllAapModelsIds)
            {
                if (productId == static_cast<unsigned short>(appleProductId))
                    return true;
            }
        }

        return false;
    }
}