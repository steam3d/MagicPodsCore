#pragma once

namespace MagicPodsCore
{
    class AapHelper
    {
    public:
        static bool IsAapDevice(unsigned short vendorId, unsigned short productId);
    };
}
