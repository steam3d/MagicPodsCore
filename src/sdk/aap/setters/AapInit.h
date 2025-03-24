#pragma once

#include "AapRequest.h"

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // Initialization handshake with aap service
    // 00000400010002000000000000000000
    //
    // Also working (not recommend to use)
    // 00000400000000000000000000000000
    //
    // Responce (Unknown ussage)
    // 010004000000010002000500494e0500a54f
    // ---------------------------------------------------------------------------
    class AapInit : public AapRequest
    {
    public:
        AapInit();
        std::vector<unsigned char> Request() const override;
    };
}