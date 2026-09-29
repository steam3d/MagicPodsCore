// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PixelBudsHelper.h"

namespace MagicPodsCore
{

    PixelBudsModelIds PixelBudsHelper::GetModelFromClass(unsigned int clazz)
    {
        if (clazz == PIXELBUDSPRO2CLASS)
        {
            return PixelBudsModelIds::PixelBudsPro2;
        }
        else if (clazz == PIXELBUDSPROCLASS)
        {
            return PixelBudsModelIds::PixelBudsPro;
        }
        else
        {
            return PixelBudsModelIds::Unknown;
        }
    }

    PixelBudsModelIds PixelBudsHelper::GetModelFromName(const std::string &name)
    {
        if (name.empty())
            return PixelBudsModelIds::Unknown;

        std::string _name = StringUtils::ToLowerCase(name);

        if (_name.find("pixel buds pro 2") != std::string::npos)
        {
            return PixelBudsModelIds::PixelBudsPro2;
        }
        else if (_name.find("pixel buds pro") != std::string::npos)
        {
            return PixelBudsModelIds::PixelBudsPro;
        }
        else
        {
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

    PixelBudsModelIds PixelBudsHelper::SearchModel(const std::vector<std::string> &guids, const std::optional<unsigned int> &clazz, const std::string &name)
    {
        if (!IsPixelBudsDevice(guids))
            return PixelBudsModelIds::Unknown;

        if (clazz.has_value())
        {
            if (PixelBudsModelIds model = GetModelFromClass(clazz.value()); model != PixelBudsModelIds::Unknown)
                return model;
        }

        return GetModelFromName(name);
    }

    std::string PixelBudsHelper::GetServiceGuid(PixelBudsModelIds model)
    {
        if (model == PixelBudsModelIds::PixelBudsPro ||
            model == PixelBudsModelIds::PixelBudsPro2)
        {
            return std::string(MAESTRO);
        }
        else
        {
            return "";
        }
    }
}
