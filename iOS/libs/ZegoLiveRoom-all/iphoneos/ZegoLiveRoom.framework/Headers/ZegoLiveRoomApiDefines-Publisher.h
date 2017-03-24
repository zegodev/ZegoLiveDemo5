//
//  ZegoLiveRoomApiDefines-Publisher.h
//  ZegoLiveRoom
//
//  Created by Randy Qiu on 2017/1/24.
//  Copyright © 2017年 zego. All rights reserved.
//

#ifndef ZegoLiveRoomApiDefines_Publisher_h
#define ZegoLiveRoomApiDefines_Publisher_h

typedef enum {
    ZegoAVConfigPreset_Verylow  = 0,
    ZegoAVConfigPreset_Low      = 1,
    ZegoAVConfigPreset_Generic  = 2,
    ZegoAVConfigPreset_High     = 3,    ///< 手机端直播建议使用High配置，效果最优
    ZegoAVConfigPreset_Veryhigh = 4
} ZegoAVConfigPreset;

typedef enum {
    ZegoAVConfigVideoFps_Verylow    = 5,
    ZegoAVConfigVideoFps_Low        = 10,
    ZegoAVConfigVideoFps_Generic    = 15,
    ZegoAVConfigVideoFps_High       = 20,
    ZegoAVConfigVideoFps_Veryhigh   = 25,
    ZegoAVConfigVideoFps_Superhigh  = 30
} ZegoAVConfigVideoFps;

typedef enum {
    ZegoAVConfigVideoBitrate_Verylow    = 250*1000,
    ZegoAVConfigVideoBitrate_Low        = 300*1000,
    ZegoAVConfigVideoBitrate_Generic    = 480*1000,
    ZegoAVConfigVideoBitrate_High       = 600*1000,
    ZegoAVConfigVideoBitrate_Veryhigh   = 800*1000,
    ZegoAVConfigVideoBitrate_Superhigh  = 1000*1000
} ZegoAVConfigVideoBitrate;

/// \brief 推流视频配置
@interface ZegoAVConfig : NSObject

+ (instancetype)presetConfigOf:(ZegoAVConfigPreset)preset;

@property (assign) CGSize videoEncodeResolution;    ///< 视频编码输出分辨率
@property (assign) CGSize videoCaptureResolution;   ///< 视频采集分辨率
@property (assign) int fps;                         ///< 视频帧率
@property (assign) int bitrate;                     ///< 视频码率

@end

enum ZegoAPIPublishFlag
{
    ZEGOAPI_JOIN_PUBLISH   = 0,
    ZEGOAPI_MIX_STREAM     = 1 << 1,
    ZEGOAPI_SINGLE_ANCHOR  = 1 << 2,
};

/// \brief 混流图层信息
@interface ZegoMixStreamInfo : NSObject

@property (copy) NSString *streamID;    ///< 要混流的单流ID
@property int top;
@property int left;
@property int bottom;
@property int right;

/**
 *  原点在左上角，top/bottom/left/right 定义如下：
 *
 *  (left, top)-----------------------
 *  |                                |
 *  |                                |
 *  |                                |
 *  |                                |
 *  -------------------(right, bottom)
 */

@end

typedef enum : NSUInteger {
    ZEGO_FILTER_NONE        = 0,    ///< 不使用滤镜
    ZEGO_FILTER_LOMO        = 1,    ///< 简洁
    ZEGO_FILTER_BLACKWHITE  = 2,    ///< 黑白
    ZEGO_FILTER_OLDSTYLE    = 3,    ///< 老化
    ZEGO_FILTER_GOTHIC      = 4,    ///< 哥特
    ZEGO_FILTER_SHARPCOLOR  = 5,    ///< 锐色
    ZEGO_FILTER_FADE        = 6,    ///< 淡雅
    ZEGO_FILTER_WINE        = 7,    ///< 酒红
    ZEGO_FILTER_LIME        = 8,    ///< 青柠
    ZEGO_FILTER_ROMANTIC    = 9,    ///< 浪漫
    ZEGO_FILTER_HALO        = 10,   ///< 光晕
    ZEGO_FILTER_BLUE        = 11,   ///< 蓝调
    ZEGO_FILTER_ILLUSION    = 12,   ///< 梦幻
    ZEGO_FILTER_DARK        = 13    ///< 夜色
} ZegoFilter;

/// \brief 美颜特性
typedef enum : NSUInteger {
    ZEGO_BEAUTIFY_NONE          = 0,        ///< 无美颜
    ZEGO_BEAUTIFY_POLISH        = 1,        ///< 磨皮
    ZEGO_BEAUTIFY_WHITEN        = 1 << 1,   ///< 全屏美白，一般与磨皮结合使用：ZEGO_BEAUTIFY_POLISH | ZEGO_BEAUTIFY_WHITEN
    ZEGO_BEAUTIFY_SKINWHITEN    = 1 << 2,   ///< 皮肤美白
    ZEGO_BEAUTIFY_SHARPEN       = 1 << 3
} ZegoBeautifyFeature;

#endif /* ZegoLiveRoomApiDefines_Publisher_h */
