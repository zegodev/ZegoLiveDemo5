//
//  ZegoLiveRoomDefines.h
//  zegoliveroom
//
//  Copyright © 2017年 ZEGO. All rights reserved.
//

#ifndef ZegoLiveRoomDefines_h
#define ZegoLiveRoomDefines_h

#ifndef ZegoAVDefines_h

#define ZEGO_MAX_COMMON_LEN         (512)
#define ZEGO_MAX_LONG_LEN           (4*1024)
#define ZEGO_MAX_URL_COUNT          (10)
#define ZEGO_MAX_EVENT_INFO_COUNT   (10)

#if defined(_MSC_VER) || defined(__BORLANDC__)
#	define _I64_				"I64"
#	define _i64_				"i64"
#	define _64u_				"%I64u"
#	define _I64uw_				L"%llu"L
#	define _i64uw_				L"%llu"L
#else
#	define _I64_				"ll"
#	define _i64_				"ll"
#	define _64u_				"%llu"
#	define _I64w_				"ll"
#	define _I64uw_				L"%llu" L
#	define _i64uw_				L"%llu" L
#	define __int64				long long
#endif

#endif

#ifdef WIN32
#ifdef ZEGO_EXPORTS
#define ZEGO_API __declspec(dllexport)
#else
#define ZEGO_API __declspec(dllimport)
#endif
#else
#define ZEGO_API __attribute__((visibility("default")))
#endif

namespace ZEGO
{
    namespace LIVEROOM
    {
        enum ZEGONetType
        {
            ZEGO_NT_NONE = 0,
            ZEGO_NT_LINE = 1,
            ZEGO_NT_WIFI = 2,
            ZEGO_NT_2G = 3,
            ZEGO_NT_3G = 4,
            ZEGO_NT_4G = 5,
            ZEGO_NT_UNKNOWN = 32
        };
        
        enum ZegoRoomRole
        {
            Anchor = 1,
            Audience = 2,
        };
        
        enum ZegoRoomError
        {
            LoginNetworkError   = 101,
            LoginPushError      = 102,
            LoginServerError    = 103,
            LoginNetChangeError = 104,    //网络切换临时状态，网络恢复后会自动重连
        };
        
        struct ZegoStreamInfo
        {
            ZegoStreamInfo()
            {
                szUserId[0] = '\0';
                szUserName[0] = '\0';
                szStreamId[0] = '\0';
                szExtraInfo[0] = '\0';
            }
            
            char szUserId[ZEGO_MAX_COMMON_LEN];
            char szUserName[ZEGO_MAX_COMMON_LEN];
            char szStreamId[ZEGO_MAX_COMMON_LEN];
            char szExtraInfo[ZEGO_MAX_COMMON_LEN];
        };
        
        
        enum ZegoStreamUpdateType
        {
            StreamAdded     = 2001,
            StreamDeleted   = 2002,
        };
        
        const int kLiveRoomErrorBase = 0x01000000;
        
        
        /// \brief 回调状态码定义
        enum LiveRoomState
        {
            Begin = 0,                      ///< 开始
            
            FatalError = 3,                 ///< 直播遇到严重的问题（如出现，请联系 ZEGO 技术支持）
            
            CreateStreamError = 4,          ///< 创建直播流失败
            FetchStreamError = 5,           ///< 获取流信息失败
            NoStreamError = 6,              ///< 无流信息
            MediaServerNetWorkError = 7,    ///< 媒体服务器连接失败
            DNSResolveError = 8,            ///< DNS 解释失败
            
            NotLoginError = 9,              ///< 未登陆
            LogicServerNetWrokError = 10,   ///< 逻辑服务器网络错误

            PublishBadNameError = 105,
            AddStreamError  = 0x1 | kLiveRoomErrorBase,
            ParameterError  = 0x2 | kLiveRoomErrorBase,
            MultiLoginError = 0x3 | kLiveRoomErrorBase,
        };
        
        const int kInvalidSeq = -1;
        
        enum ZegoVideoViewMode
        {
            ZegoVideoViewModeScaleAspectFit = 0,    ///< 等比缩放，可能有黑边
            ZegoVideoViewModeScaleAspectFill = 1,   ///< 等比缩放填充整View，可能有部分被裁减
            ZegoVideoViewModeScaleToFill = 2,       ///< 填充整个View
        };
    }
    
#ifndef ZegoAVDefines_h
    namespace AV
    {
        struct EventInfo
        {
            unsigned int uiInfoCount;
            char* arrKeys[ZEGO_MAX_EVENT_INFO_COUNT];
            char* arrValues[ZEGO_MAX_EVENT_INFO_COUNT];
        };

        enum AudioDeviceType
        {
            AudioDevice_Input = 0,
            AudioDevice_Output,
        };

        struct DeviceInfo
        {
            char szDeviceId[ZEGO_MAX_COMMON_LEN];
            char szDeviceName[ZEGO_MAX_COMMON_LEN];
        };

        enum DeviceState
        {
            Device_Added = 0,
            Device_Deleted,
        };
    }
#endif
}

#endif /* ZegoLiveRoomDefines_h */
