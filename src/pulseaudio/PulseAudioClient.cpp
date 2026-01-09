// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PulseAudioClient.h"
#include "Logger.h"

namespace MagicPodsCore
{

    PulseAudioClient::PulseAudioClient()
    {
        ml = pa_mainloop_new();
        ctx = pa_context_new(pa_mainloop_get_api(ml), "MagicPodsCore");
        pa_context_connect(ctx, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
        if (!WaitReady())
        {
            Free();
            return;
        }
        ready.store(true);

        pa_context_set_subscribe_callback(ctx, [](pa_context *c, pa_subscription_event_type_t t, uint32_t idx, void *userdata) {
            auto* self = static_cast<PulseAudioClient*>(userdata);
            auto facility = t & PA_SUBSCRIPTION_EVENT_FACILITY_MASK;
            auto type     = t & PA_SUBSCRIPTION_EVENT_TYPE_MASK;
            if (facility == PA_SUBSCRIPTION_EVENT_CARD) {

                pa_context_get_card_info_by_index(
                c,
                idx,
                [](pa_context*, const pa_card_info* info, int eol, void* userdata)
                {
                    auto* self = static_cast<PulseAudioClient*>(userdata);
                    CardInfo out;
                    if (eol || !info) return;

                    if (info->name) out.name = info->name;
                    if (info->active_profile && info->active_profile->name) {
                        out.activeProfile = info->active_profile->name;
                    }

                    for (uint32_t i = 0; i < info->n_profiles; ++i) {
                        const auto& p = info->profiles[i];
                        if (p.name) {
                            out.profiles.emplace_back(
                                p.name,
                                p.description ? p.description : ""
                            );
                        }
                    }
                
                    self->_onAudioCardPropertyChangedEvent.FireEvent(out);            
                },
                self);

            }
        }, this);
        pa_context_subscribe(ctx, static_cast<pa_subscription_mask_t>(PA_SUBSCRIPTION_MASK_CARD), nullptr, nullptr);

        th = std::thread(&PulseAudioClient::MainLoop, this);
    }

    PulseAudioClient::~PulseAudioClient()
    {
        StopLoop();
    }

    bool PulseAudioClient::SetCardProfile(const std::string &name, const std::string &profile)
    {
        if (!ready.load() || !ctx || !ml) return false;

        bool ok = false;
        pa_operation* op = pa_context_set_card_profile_by_name(
            ctx,
            name.c_str(),
            profile.c_str(),
            [](pa_context*, int success, void* userdata) {
                *static_cast<bool*>(userdata) = success;
            },
            &ok
        );

        if (!op) return false;
        while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        pa_operation_unref(op);

        return ok;
    }

    std::optional<CardInfo> PulseAudioClient::GetCardInfoByName(const std::string &name)
    {
        if (!ready.load() || !ctx || !ml) return std::nullopt;

        std::pair<bool, CardInfo> card {false, {}};
            pa_operation* op = pa_context_get_card_info_by_name(
                ctx,
                name.c_str(),
                [](pa_context*, const pa_card_info* info, int eol, void* userdata)
                {
                    auto* out = static_cast<std::pair<bool, CardInfo>*>(userdata);
                    if (eol || !info) return;

                    if (info->name) out->second.name = info->name;
                    if (info->active_profile && info->active_profile->name) {
                        out->second.activeProfile = info->active_profile->name;
                    }

                    for (uint32_t i = 0; i < info->n_profiles; ++i) {
                        const auto& p = info->profiles[i];
                        if (p.name) {
                            out->second.profiles.emplace_back(
                                p.name,
                                p.description ? p.description : ""
                            );
                        }
                    }

                    out->first = true;
                },
                &card
            );

            if (!op) return std::nullopt;
            while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            pa_operation_unref(op);

            return card.first ? std::optional<CardInfo>(card.second) : std::nullopt;
    }

    std::optional<CardInfo> PulseAudioClient::GetCardInfoByIndex(uint32_t index)
    {
        if (!ready.load() || !ctx || !ml) return std::nullopt;

        std::pair<bool, CardInfo> card {false, {}};
            pa_operation* op = pa_context_get_card_info_by_index(
                ctx,
                index,
                [](pa_context*, const pa_card_info* info, int eol, void* userdata)
                {
                    auto* out = static_cast<std::pair<bool, CardInfo>*>(userdata);
                    if (eol || !info) return;

                    if (info->name) out->second.name = info->name;
                    if (info->active_profile && info->active_profile->name) {
                        out->second.activeProfile = info->active_profile->name;
                    }

                    for (uint32_t i = 0; i < info->n_profiles; ++i) {
                        const auto& p = info->profiles[i];
                        if (p.name) {
                            out->second.profiles.emplace_back(
                                p.name,
                                p.description ? p.description : ""
                            );
                        }
                    }

                    out->first = true;
                },
                &card
            );

            if (!op) return std::nullopt;
            while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            pa_operation_unref(op);

            return card.first ? std::optional<CardInfo>(card.second) : std::nullopt;
    }

    std::string PulseAudioClient::GetNameFromMac(const std::string &mac)
    {
        std::string name = mac;
        std::replace(name.begin(), name.end(), ':', '_');
        std::transform(name.begin(), name.end(), name.begin(),
               [](unsigned char c){ return std::toupper(c); });

        return "bluez_card." + name;
    }

    void PulseAudioClient::MainLoop()
    {
        while(!stop){
            int retval = 0;
            pa_mainloop_iterate(ml, 1, &retval);
        }
    }

    bool PulseAudioClient::WaitReady(std::chrono::milliseconds timeout)
    {
        const auto start = std::chrono::steady_clock::now();
        while (true) {
            pa_mainloop_iterate(ml, 1, nullptr);
            const auto st = pa_context_get_state(ctx);
            if (st == PA_CONTEXT_READY) return true;
            if (st == PA_CONTEXT_FAILED || st == PA_CONTEXT_TERMINATED) return false;
            if (std::chrono::steady_clock::now() - start > timeout) return false;
        }
    }
    void PulseAudioClient::StopLoop()
    {
        if (!ready.load() || !ctx || !ml) return;
        stop.store(true);
        pa_mainloop_wakeup(ml);
        if (th.joinable()) th.join();
        Free();
    }

    void PulseAudioClient::Free()
    {
        if (ctx)
        {
            //pa_context_set_subscribe_callback(ctx, nullptr, nullptr);
            //pa_context_subscribe(ctx, PA_SUBSCRIPTION_MASK_NULL, nullptr, nullptr);
            
            pa_context_disconnect(ctx);
            pa_context_unref(ctx);
            ctx = nullptr;
        }
        if (ml)
        {
            pa_mainloop_free(ml);
            ml = nullptr;
        }
        ready.store(false);
    }
}