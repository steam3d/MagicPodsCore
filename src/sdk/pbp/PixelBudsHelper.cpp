// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsHelper.h"
#include "BtVendorIds.h"

namespace MagicPodsCore
{
    PixelBudsModelIds PixelBudsHelper::GetModelFromVidPid(unsigned short vendorId, unsigned short productId)
    {
        if (vendorId != static_cast<unsigned short>(BtVendorIds::Google))
            return PixelBudsModelIds::Unknown;

        switch (productId)
        {
        case static_cast<unsigned short>(PixelBudsModelIds::PixelBudsASeries):
            return PixelBudsModelIds::PixelBudsASeries;
        case static_cast<unsigned short>(PixelBudsModelIds::PixelBudsPro):
            return PixelBudsModelIds::PixelBudsPro;
        case static_cast<unsigned short>(PixelBudsModelIds::PixelBudsPro2):
            return PixelBudsModelIds::PixelBudsPro2;
        case static_cast<unsigned short>(PixelBudsModelIds::PixelBuds2a):
            return PixelBudsModelIds::PixelBuds2a;
        default:
            return PixelBudsModelIds::Unknown;
        }
    }

    bool PixelBudsHelper::IsPixelBudsDevice(const std::vector<std::string> &guids)
    {
        std::vector<std::string> _guids;
        _guids.reserve(guids.size());
        std::transform(guids.begin(), guids.end(), std::back_inserter(_guids), [](const std::string &guid)
                       { return StringUtils::ToLowerCase(guid); });

        return std::find(_guids.begin(), _guids.end(), MAESTRO) != _guids.end();
    }

    PixelBudsModelIds PixelBudsHelper::SearchModel(const std::vector<std::string> &guids, unsigned short vendorId, unsigned short productId)
    {
        if (!IsPixelBudsDevice(guids))
            return PixelBudsModelIds::Unknown;

        return GetModelFromVidPid(vendorId, productId);
    }

    std::string PixelBudsHelper::GetServiceGuid(PixelBudsModelIds model)
    {
        if (model == PixelBudsModelIds::PixelBudsASeries ||
            model == PixelBudsModelIds::PixelBudsPro ||
            model == PixelBudsModelIds::PixelBudsPro2 ||
            model == PixelBudsModelIds::PixelBuds2a)
        {
            return std::string(MAESTRO);
        }
        else
        {
            return "";
        }
    }
}
