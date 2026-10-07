// MagicPodsCore: https://github.com/steam3d/MagicPodsCore
// Copyright: 2020-2026 Aleksandr Maslov <https://magicpods.app> & Andrei Litvintsev <a.a.litvintsev@gmail.com>
// Based on the original implementation by Luan Ademi
// License: GPL-3.0

#include "AacEldDecoder.h"
#include "Logger.h"

#include <algorithm>
#include <array>
#include <cmath>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavutil/channel_layout.h>
    #include <libavutil/log.h>
    #include <libavutil/mem.h>
    #include <libavutil/samplefmt.h>
    #include <libavutil/version.h>
}

namespace MagicPodsCore
{
    namespace
    {
        constexpr std::array<unsigned char, 4> AudioSpecificConfig{0xF8, 0xE6, 0x30, 0x00};
        constexpr int CodingRate = 48000;

        int16_t FloatToInt16(float sample)
        {
            return static_cast<int16_t>(std::round(std::clamp(sample, -1.0f, 1.0f) * 32767.0f));
        }
    }

    AacEldDecoder::AacEldDecoder() : _inputBuffer(MaxInputLength + AV_INPUT_BUFFER_PADDING_SIZE)
    {
        av_log_set_level(AV_LOG_FATAL);

        const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_AAC);
        if (!codec)
        {
            Logger::Error("AAC decoder is not available");
            return;
        }

        _context = avcodec_alloc_context3(codec);
        if (!_context)
            return;

        _context->extradata = static_cast<unsigned char*>(av_mallocz(AudioSpecificConfig.size() + AV_INPUT_BUFFER_PADDING_SIZE));
        if (!_context->extradata)
        {
            Free();
            return;
        }

        std::copy(AudioSpecificConfig.begin(), AudioSpecificConfig.end(), _context->extradata);
        _context->extradata_size = AudioSpecificConfig.size();
        _context->sample_rate = CodingRate;

        #if LIBAVUTIL_VERSION_MAJOR >= 57
        av_channel_layout_default(&_context->ch_layout, Channels);
        #else
        _context->channels = Channels;
        _context->channel_layout = AV_CH_LAYOUT_MONO;
        #endif

        if (avcodec_open2(_context, codec, nullptr) < 0)
        {
            Logger::Error("Failed to open the AAC-ELD decoder");
            Free();
            return;
        }

        _packet = av_packet_alloc();
        _frame = av_frame_alloc();
        if (!_packet || !_frame)
        {
            Free();
            return;
        }
    }

    AacEldDecoder::~AacEldDecoder()
    {
        Free();
    }

    bool AacEldDecoder::IsOpen() const
    {
        return _context && _packet && _frame;
    }

    bool AacEldDecoder::Decode(std::span<const unsigned char> accessUnit, std::vector<int16_t>& output)
    {
        if (!IsOpen() || accessUnit.empty() || accessUnit.size() > MaxInputLength)
            return false;

        std::copy(accessUnit.begin(), accessUnit.end(), _inputBuffer.begin());
        std::fill_n(_inputBuffer.begin() + accessUnit.size(), AV_INPUT_BUFFER_PADDING_SIZE, 0);
        _packet->data = _inputBuffer.data();
        _packet->size = accessUnit.size();

        if (avcodec_send_packet(_context, _packet) < 0 || avcodec_receive_frame(_context, _frame) < 0)
            return false;

        #if LIBAVUTIL_VERSION_MAJOR >= 57
        const int channelCount = _frame->ch_layout.nb_channels;
        #else
        const int channelCount = _frame->channels;
        #endif

        const int sampleCount = _frame->nb_samples;
        if (channelCount <= 0 || sampleCount <= 0)
        {
            av_frame_unref(_frame);
            return false;
        }

        const size_t total = static_cast<size_t>(sampleCount) * channelCount;
        output.reserve(output.size() + total);

        switch (static_cast<AVSampleFormat>(_frame->format))
        {
        case AV_SAMPLE_FMT_FLTP:
            for (int sample = 0; sample < sampleCount; ++sample)
            {
                for (int channel = 0; channel < channelCount; ++channel)
                {
                    const auto* plane = reinterpret_cast<const float*>(_frame->extended_data[channel]);
                    output.push_back(FloatToInt16(plane[sample]));
                }
            }
            break;
        case AV_SAMPLE_FMT_FLT:
        {
            const auto* samples = reinterpret_cast<const float*>(_frame->extended_data[0]);
            for (size_t i = 0; i < total; ++i)
                output.push_back(FloatToInt16(samples[i]));
            break;
        }
        case AV_SAMPLE_FMT_S16P:
            for (int sample = 0; sample < sampleCount; ++sample)
            {
                for (int channel = 0; channel < channelCount; ++channel)
                {
                    const auto* plane = reinterpret_cast<const int16_t*>(_frame->extended_data[channel]);
                    output.push_back(plane[sample]);
                }
            }
            break;
        case AV_SAMPLE_FMT_S16:
        {
            const auto* samples = reinterpret_cast<const int16_t*>(_frame->extended_data[0]);
            output.insert(output.end(), samples, samples + total);
            break;
        }
        default:
            av_frame_unref(_frame);
            return false;
        }

        av_frame_unref(_frame);
        return true;
    }

    void AacEldDecoder::Free()
    {
        if (_frame)
            av_frame_free(&_frame);
        if (_packet)
            av_packet_free(&_packet);
        if (_context)
            avcodec_free_context(&_context);
    }
}
