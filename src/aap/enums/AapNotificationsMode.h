#pragma once

namespace MagicPodsCore
{
    // ---------------------------------------------------------------------------
    // Request to headphones provide notification:
    // Set Notification Filter
    // Battery: Enabled
    // In Ear: Enabled
    // Role: Enabled
    // Connection: Enabled
    // Stream: Enabled
    // Relay: Enabled
    // Easy Pair: Enabled
    // Triangle: Enabled
    // Custom Message: Enabled
    //
    // iPhone sends first this:
    // 040004000f00fffffeff
    //
    // Then iPhone request capabilities and sends this:
    // 040004000f00ffffffff
    // 0  1  2  3  4  5  6  7  8  9
    //                         un
    // 04 00 04 00 0f 00 ff ff ff ff
    // 04 00 04 00 0f 00 ff ff fe ff
    //
    // ! It is unknown which bytes are responsible for what !
    // ---------------------------------------------------------------------------

    // Byte 8
    enum class AapNotificationsMode : unsigned char
    {
        // Always use this
        Unknown1 = 0xff,
        Unknown2 = 0xef,
    };
}