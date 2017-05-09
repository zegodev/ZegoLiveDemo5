//
//  LiveRoomDefines-Publisher.h
//  zegoliveroom
//
//  Created by Randy Qiu on 2017/1/23.
//
//

#ifndef LiveRoomDefines_Publisher_h
#define LiveRoomDefines_Publisher_h

#include "./LiveRoomDefines.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        enum ZegoPublishFlag
        {
            ZEGO_JOIN_PUBLISH   = 0,        ///< 连麦
            ZEGO_MIX_STREAM     = 1 << 1,   ///< 混流
            ZEGO_SINGLE_ANCHOR  = 1 << 2,   ///< 单主播
        };
        
        struct ZegoPublishingStreamInfo
        {
            ZegoPublishingStreamInfo()
            {
                uiRtmpURLCount = 0;
                uiHlsURLCount = 0;
                uiFlvURLCount = 0;
            }
            const char* arrRtmpURLs[ZEGO_MAX_URL_COUNT];   ///< rtmp 播放 url 列表
            unsigned int uiRtmpURLCount;                            ///< rtmp url 个数
            
            const char* arrFlvURLs[ZEGO_MAX_URL_COUNT];
            unsigned int uiFlvURLCount;
            
            const char* arrHlsURLs[ZEGO_MAX_URL_COUNT];
            unsigned int uiHlsURLCount;
        };
        
    }

#ifndef ZegoAVDefines_h
    namespace AV
    {
        /// \brief 混流图层信息
        struct ZegoMixStreamConfig
        {
            char szStreamID[ZEGO_MAX_COMMON_LEN];   ///< 混流ID
            struct
            {
                int top;
                int left;
                int bottom;
                int right;
            } layout;
            
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
        };
    }
#endif
}
#endif /* LiveRoomDefines_Publisher_h */
