#ifndef FFMPEGRECORD_H
#define FFMPEGRECORD_H

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <filesystem>
#include <string>

#include <glm/glm.hpp>

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/time.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}

#include <record.h>

/**
 * @brief An FFmpeg based Record.
 * @remark Records to H265 with format determined from
 * file extension
 * @remark Generally higher quality than jo_mpeg.
 */
class FFmpegRecord : public Record
{

public:

    /**
     * @brief Construct a new FFmpeg based Record
     * from a file path, resolution and fps.
     *
     * @param file the video file path.
     * @param resolution the resolution.
     * @param fps the frames per second.
     */
    FFmpegRecord
    (
        std::filesystem::path file,
        glm::ivec2 resolution,
        uint8_t fps
    )
    :
      Record(file, resolution, fps)
    {
        outputFormat = av_guess_format(nullptr, file.c_str(), nullptr);
        if (!outputFormat)
        {
            throw std::runtime_error("Failed to create FFmpeg output format");
        }
        outputFormat->video_codec = AV_CODEC_ID_H265;

        if (avformat_alloc_output_context2(&outputContext, outputFormat, nullptr, file.c_str()))
        {
            throw std::runtime_error("Failed to create FFmpeg output context");
        }

        codec = avcodec_find_encoder(outputFormat->video_codec);
        if (!codec)
        {
            throw std::runtime_error("Failed to create FFmpeg codec");
        }

        AVStream * stream = avformat_new_stream(outputContext, codec);
        if (!stream)
        {
            throw std::runtime_error("Failed to create FFmpeg stream");
        }

        context = avcodec_alloc_context3(codec);
        if (!context)
        {
            throw std::runtime_error("Failed to create FFmpeg codec context");
        }

        stream->codecpar->codec_id = outputFormat->video_codec;
        stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        stream->codecpar->width = resolution.x;
        stream->codecpar->height = resolution.y;
        stream->codecpar->format = AV_PIX_FMT_YUV420P;
        stream->codecpar->bit_rate = bitrate;
        avcodec_parameters_to_context(context, stream->codecpar);
        context->time_base = (AVRational){ 1, fps };
        context->max_b_frames = 2;
        context->gop_size = 12;
        context->framerate = (AVRational){ fps, 1 };

        av_opt_set(context, "preset", "medium", 0);
        avcodec_parameters_from_context(stream->codecpar, context);
    }

    ~FFmpegRecord()
    {
        if (videoFrame)
        {
            av_frame_free(&videoFrame);
        }
        if (context)
        {
            avcodec_free_context(&context);
        }
        if (outputContext)
        {
            avformat_free_context(outputContext);
        }
        if (scaleContext)
        {
            sws_freeContext(scaleContext);
        }
    }

    /**
     * @brief Open the video file.
     *
     * @param info print extra information.
     * @remark including header writing etc.
     */
    void open(bool info = false)
    {
        if (fileOpen) { return; }
        if ((avcodec_open2(context, codec, NULL)) < 0)
        {
            throw std::runtime_error("Failed to open FFmpeg codec");
        }

        if (!(outputFormat->flags & AVFMT_NOFILE))
        {
            if ((avio_open(&outputContext->pb, file.c_str(), AVIO_FLAG_WRITE)) < 0)
            {
                throw std::runtime_error("Failed to video open file");
            }
        }

        if ((avformat_write_header(outputContext, NULL)) < 0)
        {
            throw std::runtime_error("Failed to write video header");
        }

        if (info) { av_dump_format(outputContext, 0, file.c_str(), 1); }
        fileOpen = true;
    }

    /**
     * @brief Close the video file.
     *
     * @remark including trailer writing etc.
     */
    void close()
    {
        if (!fileOpen) { return; }
        fileOpen = false;
        AVPacket * pkt = av_packet_alloc();
        pkt->data = NULL;
        pkt->size = 0;

        while(true)
        {
            avcodec_send_frame(context, NULL);
            if (avcodec_receive_packet(context, pkt) == 0)
            {
                av_interleaved_write_frame(outputContext, pkt);
                av_packet_unref(pkt);
            }
            else
            {
                break;
            }
        }

        av_write_trailer(outputContext);
        if (!(outputFormat->flags & AVFMT_NOFILE))
        {
            if (avio_close(outputContext->pb) < 0)
            {
                throw std::runtime_error("Failed to close file");
            }
        }
    }

private:

    AVFrame * videoFrame = nullptr;
    AVCodec * codec = nullptr;
    AVStream * stream = nullptr;
    AVCodecContext * context = nullptr;
    SwsContext * scaleContext = nullptr;
    AVFormatContext * outputContext = nullptr;
    AVOutputFormat * outputFormat = nullptr;

    uint32_t bitrate = 2000000;
    uint16_t frameCounter = 0;

    void write(const uint8_t * frame)
    {
        if (!videoFrame)
        {
            videoFrame = av_frame_alloc();
            videoFrame->format = AV_PIX_FMT_YUV420P;
            videoFrame->width = context->width;
            videoFrame->height = context->height;
            if ((av_frame_get_buffer(videoFrame, 0)) < 0)
            {
                throw std::runtime_error("Failed to allocate FFmpeg frame");
            }
        }
        if (!scaleContext)
        {
            scaleContext = sws_getContext
            (
                context->width,
                context->height,
                AV_PIX_FMT_RGBA,
                context->width,
                context->height,
                AV_PIX_FMT_YUV420P,
                SWS_BICUBIC,
                0,
                0,
                0
            );
        }

        // Fixed, stride should be 4x for RBGA data.
        int stride[1] = {4 * context->width};

        sws_scale
        (
            scaleContext,
            (const uint8_t * const *)&frame,
            stride,
            0,
            context->height,
            videoFrame->data,
            videoFrame->linesize
        );

        videoFrame->pts = (1.0 / float(fps)) * 90000 * (frameCounter++);
        if ((avcodec_send_frame(context, videoFrame)) < 0)
        {
            throw std::runtime_error("Failed to send frame");
        }

        AVPacket * pkt = av_packet_alloc();
        pkt->data = NULL;
        pkt->size = 0;
        pkt->flags |= AV_PKT_FLAG_KEY;
        if (avcodec_receive_packet(context, pkt) == 0)
        {
            av_interleaved_write_frame(outputContext, pkt);
            av_packet_unref(pkt);
        }
    }
};

#endif /* FFMPEGRECORD_H */
