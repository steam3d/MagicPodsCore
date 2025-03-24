#pragma once

namespace MagicPodsCore
{
    class AapHelper
    {
    private:
    public:
        static bool IsAapDevice(unsigned short vendorId, unsigned short productId);
    };
}
