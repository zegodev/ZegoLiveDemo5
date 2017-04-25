//
//  ZegoLiveRoomApiDefines.h
//  ZegoLiveRoom
//
//  Copyright © 2017年 zego. All rights reserved.
//

#ifndef ZegoLiveRoomApiDefines_h
#define ZegoLiveRoomApiDefines_h

#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#define ZEGOView UIView
#define ZEGOImage UIImage
#elif TARGET_OS_OSX
#import <AppKit/AppKit.h>
#define ZEGOView NSView
#define ZEGOImage NSImage
#endif

#ifdef __cplusplus
#define ZEGO_EXTERN     extern "C"
#else
#define ZEGO_EXTERN     extern
#endif

ZEGO_EXTERN NSString *const kZegoRtmpUrlListKey;        ///< rtmp 播放 url 列表，值为 NSArray<NSString *>
ZEGO_EXTERN NSString *const kZegoHlsUrlListKey;         ///< hls 播放 url 列表，值为 NSArray<NSString *>
ZEGO_EXTERN NSString *const kZegoFlvUrlListKey;         ///< flv 播放 url 列表，值为 NSArray<NSString *>

ZEGO_EXTERN NSString *const kZegoMixStreamIDKey;        ///< 混流ID，值为 NSString
ZEGO_EXTERN NSString *const kZegoMixStreamResolution;   ///< 混流输出大小，值为 NSValue


typedef enum
{
    ZEGO_ANCHOR = 1,
    ZEGO_AUDIENCE = 2,
} ZegoRole;

typedef enum
{
    ZEGO_STREAM_ADD     = 2001,
    ZEGO_STREAM_DELETE  = 2002,
} ZegoStreamType;

typedef enum {
    ZegoVideoViewModeScaleAspectFit     = 0,    ///< 等比缩放，可能有黑边
    ZegoVideoViewModeScaleAspectFill    = 1,    ///< 等比缩放填充整View，可能有部分被裁减
    ZegoVideoViewModeScaleToFill        = 2,    ///< 填充整个View
} ZegoVideoViewMode;


enum ZegoApiPublishFlag
{
    ZEGO_JOIN_PUBLISH   = 0,
    ZEGO_MIX_STREAM     = 1 << 1,
    ZEGO_SINGLE_ANCHOR  = 1 << 2,
};


@interface ZegoStream : NSObject

@property (nonatomic, copy) NSString *userID;
@property (nonatomic, copy) NSString *userName;
@property (nonatomic, copy) NSString *streamID;
@property (nonatomic, copy) NSString *extraInfo;

@end

typedef void(^ZegoSnapshotCompletionBlock)(ZEGOImage* img);

enum ZegoAPIModuleType
{
    ZEGOAPI_MODULE_AUDIO            = 0x4 | 0x8,    ///< 音频采集播放设备
};

#endif /* ZegoLiveRoomApiDefines_h */
