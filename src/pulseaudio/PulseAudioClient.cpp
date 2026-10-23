// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// License: GPL-3.0

#include "PulseAudioClient.h"
#include "Logger.h"

#include <algorithm>
#include <chrono>
#include <pulse/proplist.h>
#include <thread>

namespace
{
    struct TemporaryPulseConnection
    {
        pa_mainloop* MainLoop{};
        pa_context* Context{};

        ~TemporaryPulseConnection()
        {
            if (Context)
            {
                pa_context_disconnect(Context);
                pa_context_unref(Context);
            }
            if (MainLoop)
                pa_mainloop_free(MainLoop);
        }

        bool Connect(const char* name)
        {
            MainLoop = pa_mainloop_new();
            if (!MainLoop)
                return false;

            Context = pa_context_new(pa_mainloop_get_api(MainLoop), name);
            if (!Context || pa_context_connect(Context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr) < 0)
                return false;

            const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
            while (std::chrono::steady_clock::now() < deadline)
            {
                if (pa_mainloop_iterate(MainLoop, 0, nullptr) < 0)
                    return false;

                const auto state = pa_context_get_state(Context);
                if (state == PA_CONTEXT_READY)
                    return true;
                if (state == PA_CONTEXT_FAILED || state == PA_CONTEXT_TERMINATED)
                    return false;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            return false;
        }

        bool Wait(pa_operation* operation)
        {
            if (!operation)
                return false;

            const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
            while (pa_operation_get_state(operation) == PA_OPERATION_RUNNING &&
                   std::chrono::steady_clock::now() < deadline)
            {
                if (pa_mainloop_iterate(MainLoop, 0, nullptr) < 0)
                {
                    pa_operation_unref(operation);
                    return false;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            const bool completed = pa_operation_get_state(operation) == PA_OPERATION_DONE;
            pa_operation_unref(operation);
            return completed;
        }
    };
}

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

                    out.index = info->index;
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
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-CardInfo"))
            return std::nullopt;

        std::pair<bool, CardInfo> card {false, {}};
            auto operation = pa_context_get_card_info_by_name(
                connection.Context,
                name.c_str(),
                [](pa_context*, const pa_card_info* info, int eol, void* userdata)
                {
                    auto* out = static_cast<std::pair<bool, CardInfo>*>(userdata);
                    if (eol || !info) return;

                    out->second.index = info->index;
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

            if (!connection.Wait(operation))
                return std::nullopt;

            return card.first ? std::optional<CardInfo>(card.second) : std::nullopt;
    }

    std::optional<CardInfo> PulseAudioClient::GetCardInfoByIndex(uint32_t index)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-CardInfo"))
            return std::nullopt;

        std::pair<bool, CardInfo> card {false, {}};
            auto operation = pa_context_get_card_info_by_index(
                connection.Context,
                index,
                [](pa_context*, const pa_card_info* info, int eol, void* userdata)
                {
                    auto* out = static_cast<std::pair<bool, CardInfo>*>(userdata);
                    if (eol || !info) return;

                    out->second.index = info->index;
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

            if (!connection.Wait(operation))
                return std::nullopt;

            return card.first ? std::optional<CardInfo>(card.second) : std::nullopt;
    }

    std::optional<uint32_t> PulseAudioClient::LoadModule(const std::string& name, const std::string& arguments)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-ModuleLoader"))
            return std::nullopt;

        uint32_t index = PA_INVALID_INDEX;
        auto operation = pa_context_load_module(
            connection.Context,
            name.c_str(),
            arguments.c_str(),
            [](pa_context*, uint32_t moduleIndex, void* userdata)
            {
                *static_cast<uint32_t*>(userdata) = moduleIndex;
            },
            &index);

        if (!connection.Wait(operation) || index == PA_INVALID_INDEX)
            return std::nullopt;
        return index;
    }

    bool PulseAudioClient::UnloadModule(uint32_t index)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-ModuleLoader"))
            return false;

        bool success = false;
        auto operation = pa_context_unload_module(
            connection.Context,
            index,
            [](pa_context*, int result, void* userdata)
            {
                *static_cast<bool*>(userdata) = result != 0;
            },
            &success);

        return connection.Wait(operation) && success;
    }

    bool PulseAudioClient::SetDefaultSource(const std::string& sourceName)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-DefaultSource"))
            return false;

        bool success = false;
        auto operation = pa_context_set_default_source(
            connection.Context,
            sourceName.c_str(),
            [](pa_context*, int result, void* userdata)
            {
                *static_cast<bool*>(userdata) = result != 0;
            },
            &success);

        return connection.Wait(operation) && success;
    }

    std::optional<std::string> PulseAudioClient::GetDefaultSinkName()
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-DefaultSink"))
            return std::nullopt;

        std::optional<std::string> sinkName{};
        auto operation = pa_context_get_server_info(
            connection.Context,
            [](pa_context*, const pa_server_info* info, void* userdata)
            {
                if (info && info->default_sink_name)
                    *static_cast<std::optional<std::string>*>(userdata) = info->default_sink_name;
            },
            &sinkName);

        if (!connection.Wait(operation))
            return std::nullopt;
        return sinkName;
    }

    std::optional<SinkInfo> PulseAudioClient::GetSinkInfoByName(const std::string& name)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-SinkInfo"))
            return std::nullopt;

        std::optional<SinkInfo> sink{};
        auto operation = pa_context_get_sink_info_by_name(
            connection.Context,
            name.c_str(),
            [](pa_context*, const pa_sink_info* info, int eol, void* userdata)
            {
                if (eol || !info)
                    return;

                SinkInfo out{};
                out.index = info->index;
                out.card = info->card;
                out.volume = info->volume;
                if (info->name)
                    out.name = info->name;
                if (const char* cardName = info->proplist ? pa_proplist_gets(info->proplist, "device.name") : nullptr)
                    out.cardName = cardName;
                if (info->active_port && info->active_port->name)
                    out.activePort = info->active_port->name;
                *static_cast<std::optional<SinkInfo>*>(userdata) = std::move(out);
            },
            &sink);

        if (!connection.Wait(operation))
            return std::nullopt;
        return sink;
    }

    std::vector<SinkInfo> PulseAudioClient::GetSinks()
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-SinkList"))
            return {};

        std::vector<SinkInfo> sinks{};
        auto operation = pa_context_get_sink_info_list(
            connection.Context,
            [](pa_context*, const pa_sink_info* info, int eol, void* userdata)
            {
                if (eol || !info)
                    return;

                SinkInfo out{};
                out.index = info->index;
                out.card = info->card;
                out.volume = info->volume;
                if (info->name)
                    out.name = info->name;
                if (const char* cardName = info->proplist ? pa_proplist_gets(info->proplist, "device.name") : nullptr)
                    out.cardName = cardName;
                if (info->active_port && info->active_port->name)
                    out.activePort = info->active_port->name;
                static_cast<std::vector<SinkInfo>*>(userdata)->push_back(std::move(out));
            },
            &sinks);

        if (!connection.Wait(operation))
            return {};
        return sinks;
    }

    bool PulseAudioClient::SetDefaultSink(const std::string& sinkName)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-DefaultSink"))
            return false;

        bool success = false;
        auto operation = pa_context_set_default_sink(
            connection.Context,
            sinkName.c_str(),
            [](pa_context*, int result, void* userdata)
            {
                *static_cast<bool*>(userdata) = result != 0;
            },
            &success);

        return connection.Wait(operation) && success;
    }

    bool PulseAudioClient::MoveSinkInputs(uint32_t sourceSinkIndex, uint32_t destinationSinkIndex)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-SinkInputMover"))
            return false;

        struct CallbackData
        {
            uint32_t SourceSinkIndex;
            std::vector<uint32_t> SinkInputIndexes{};
        } callbackData{sourceSinkIndex};

        auto listOperation = pa_context_get_sink_input_info_list(
            connection.Context,
            [](pa_context*, const pa_sink_input_info* info, int eol, void* userdata)
            {
                if (eol || !info)
                    return;

                auto& data = *static_cast<CallbackData*>(userdata);
                if (info->sink == data.SourceSinkIndex)
                    data.SinkInputIndexes.push_back(info->index);
            },
            &callbackData);

        if (!connection.Wait(listOperation))
            return false;

        bool allMoved = true;
        for (uint32_t sinkInputIndex : callbackData.SinkInputIndexes)
        {
            bool success = false;
            auto moveOperation = pa_context_move_sink_input_by_index(
                connection.Context,
                sinkInputIndex,
                destinationSinkIndex,
                [](pa_context*, int result, void* userdata)
                {
                    *static_cast<bool*>(userdata) = result != 0;
                },
                &success);

            if (!connection.Wait(moveOperation) || !success)
                allMoved = false;
        }

        return allMoved;
    }

    bool PulseAudioClient::SwitchOutput(const SinkInfo& source, const SinkInfo& destination)
    {
        if (source.name == destination.name)
            return true;

        if (!SetDefaultSink(destination.name))
        {
            Logger::Warn("Failed to set default audio output: %s", destination.name.c_str());
            return false;
        }

        if (!MoveSinkInputs(source.index, destination.index))
            Logger::Warn("Failed to move some audio streams to: %s", destination.name.c_str());

        Logger::Info("Audio output switched to: %s", destination.name.c_str());
        return true;
    }

    bool PulseAudioClient::SwitchCardToSpeakers(const CardInfo& card)
    {
        auto defaultSinkName = GetDefaultSinkName();
        if (!defaultSinkName.has_value())
            return false;

        auto currentSink = GetSinkInfoByName(defaultSinkName.value());
        if (!currentSink.has_value())
            return false;

        if (!currentSink->IsOnCard(card))
            return true;

        for (const auto& sink : GetSinks())
        {
            if (!sink.IsOnCard(card) && sink.activePort.find("speaker") != std::string::npos)
                return SwitchOutput(currentSink.value(), sink);
        }

        Logger::Warn("Failed to find speakers audio output");
        return false;
    }

    bool PulseAudioClient::SwitchToCard(const CardInfo& card)
    {
        auto defaultSinkName = GetDefaultSinkName();
        if (!defaultSinkName.has_value())
            return false;

        auto currentSink = GetSinkInfoByName(defaultSinkName.value());
        if (!currentSink.has_value())
            return false;

        for (const auto& sink : GetSinks())
        {
            if (sink.IsOnCard(card))
                return SwitchOutput(currentSink.value(), sink);
        }

        Logger::Warn("Failed to find card audio output: %s", card.name.c_str());
        return false;
    }

    bool PulseAudioClient::FadeSinkVolume(uint32_t sinkIndex, const pa_cvolume& from, const pa_cvolume& to)
    {
        if (!pa_cvolume_valid(&from) || !pa_cvolume_valid(&to) || from.channels != to.channels)
            return false;

        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-SinkVolume"))
            return false;

        constexpr int steps = 10;
        constexpr auto delay = std::chrono::milliseconds(25);

        for (int step = 1; step <= steps; step++)
        {
            pa_cvolume volume = from;
            for (uint8_t channel = 0; channel < volume.channels; channel++)
            {
                int64_t difference = static_cast<int64_t>(to.values[channel]) - from.values[channel];
                volume.values[channel] = static_cast<pa_volume_t>(from.values[channel] + difference * step / steps);
            }

            bool success = false;
            auto operation = pa_context_set_sink_volume_by_index(
                connection.Context,
                sinkIndex,
                &volume,
                [](pa_context*, int result, void* userdata)
                {
                    *static_cast<bool*>(userdata) = result != 0;
                },
                &success);

            if (!connection.Wait(operation) || !success)
                return false;

            if (step != steps)
                std::this_thread::sleep_for(delay);
        }

        return true;
    }

    std::vector<uint32_t> PulseAudioClient::GetModuleIndexesByArgument(const std::string& value)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-ModuleLoader"))
            return {};

        struct CallbackData
        {
            const std::string& Value;
            std::vector<uint32_t> Indexes{};
        } callbackData{value};

        auto operation = pa_context_get_module_info_list(
            connection.Context,
            [](pa_context*, const pa_module_info* info, int eol, void* userdata)
            {
                if (eol || !info || !info->argument)
                    return;

                auto& data = *static_cast<CallbackData*>(userdata);
                if (std::string_view{info->argument}.find(data.Value) != std::string_view::npos)
                    data.Indexes.push_back(info->index);
            },
            &callbackData);

        if (!connection.Wait(operation))
            return {};
        return callbackData.Indexes;
    }

    std::optional<std::string> PulseAudioClient::GetSourceConsumer(const std::string& sourceName)
    {
        TemporaryPulseConnection connection{};
        if (!connection.Connect("MagicPodsCore-SourceMonitor"))
            return std::nullopt;

        uint32_t sourceIndex = PA_INVALID_INDEX;
        auto sourceOperation = pa_context_get_source_info_by_name(
            connection.Context,
            sourceName.c_str(),
            [](pa_context*, const pa_source_info* info, int eol, void* userdata)
            {
                if (!eol && info)
                    *static_cast<uint32_t*>(userdata) = info->index;
            },
            &sourceIndex);

        if (!connection.Wait(sourceOperation) || sourceIndex == PA_INVALID_INDEX)
            return std::nullopt;

        struct CallbackData
        {
            uint32_t SourceIndex;
            std::optional<std::string> Application{};
        } callbackData{sourceIndex};

        auto outputOperation = pa_context_get_source_output_info_list(
            connection.Context,
            [](pa_context*, const pa_source_output_info* info, int eol, void* userdata)
            {
                if (eol || !info)
                    return;

                auto& data = *static_cast<CallbackData*>(userdata);
                if (info->source != data.SourceIndex || data.Application.has_value())
                    return;

                const char* application = info->proplist ? pa_proplist_gets(info->proplist, PA_PROP_APPLICATION_NAME) : nullptr;
                if (application)
                    data.Application = application;
                else if (info->name)
                    data.Application = info->name;
                else
                    data.Application = "Unknown application";
            },
            &callbackData);

        if (!connection.Wait(outputOperation))
            return std::nullopt;
        return callbackData.Application;
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
