#pragma once

namespace MagicPodsCore
{

    enum class SonyAncFilter : unsigned char
    {
        Ambient = 0x00,
        Wind = 0x01,
        Anc = 0x02,
    };

    enum class SonyAncFilterAmbientVoice : unsigned char
    {
        Off = 0x00,
        On = 0x01,
    };

    enum class SonyAncSwitch : unsigned char
    {
        Off = 0x00,
        On = 0x10,
        //Save current mode to headphones
        //When user click switch mode button on headphones, the headphones will switch next mode based on saved setting.
        //Default cycle: ANC -> Ambient -> Disable.
        //If you <see cref="SonyAncSwitch.OnAndSave"/> Ambient, the next mode will be "Disable" when user click headphones button.
        //if you <see cref="SonyAncSwitch.On"/> Ambient, the next mode will be any, since we are not changing the default cycle.
        //Option also write Volume and Voice settings for ambient mode.
        //Option can override Ambient mode to Wind mode, the headphones will speak "Ambient mode", but the real mode will be wind. This is strange, but works.  
        OnAndSave = 0x11,
    };
}
