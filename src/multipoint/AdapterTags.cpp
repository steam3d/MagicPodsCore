// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0
// which group-table rows are us

#include "AdapterTags.h"

#include "Logger.h"
#include "sdk/sgb/smep/MultipointInfo.h"

#include <sdbus-c++/sdbus-c++.h>

#include <algorithm>
#include <map>

namespace MagicPodsCore
{
    std::vector<std::string> AdapterTags::AdapterAddresses()
    {
        std::vector<std::string> out{};
        try
        {
            auto rootProxy = sdbus::createProxy("org.bluez", "/");
            std::map<sdbus::ObjectPath, std::map<std::string, std::map<std::string, sdbus::Variant>>> managedObjects{};
            rootProxy->callMethod("GetManagedObjects")
                .onInterface("org.freedesktop.DBus.ObjectManager")
                .storeResultsTo(managedObjects);

            for (const auto& [objectPath, interfaces] : managedObjects)
            {
                const auto adapter = interfaces.find("org.bluez.Adapter1");
                if (adapter == interfaces.end())
                {
                    continue;
                }
                const auto address = adapter->second.find("Address");
                if (address == adapter->second.end())
                {
                    continue;
                }
                out.push_back(address->second.get<std::string>());
            }
        }
        catch (const sdbus::Error& error)
        {
            Logger::Warn("AdapterTags: cannot read adapter addresses: %s", error.getMessage());
        }
        return out;
    }

    std::vector<uint16_t> AdapterTags::LocalTags()
    {
        std::vector<uint16_t> out{};
        for (const auto& address : AdapterAddresses())
        {
            const uint16_t tag = MultipointState::TagFromAddress(address);
            if (tag != 0 && std::find(out.begin(), out.end(), tag) == out.end())
            {
                out.push_back(tag);
            }
        }
        return out;
    }
}
