//
//  LiveRoomCallback-Player.h
//  zegoliveroom
//
//  Created by Randy Qiu on 2017/1/23.
//
//

#ifndef LiveRoomCallback_Player_h
#define LiveRoomCallback_Player_h

namespace ZEGO
{
    namespace LIVEROOM
    {
        class ILivePlayerCallback
        {
        public:
            /// \brief 播放状态更新
            /// \param[in] stateCode 状态码
            /// \param[in] pszStreamID 流ID
            virtual void OnPlayStateUpdate(int stateCode, const char* pszStreamID) = 0;
            
            /// \brief 观看质量更新
            /// \param[in] pszStreamID 观看流ID
            /// \param[in] quality 0 ~ 3 分别对应优良中差
            /// \param[in] videoFPS 帧率
            /// \param[in] videoKBS 码率
            virtual void OnPlayQualityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS) {};
            
            /// \brief 收到请求连麦结果
            /// \param[in] result 结果
            /// \param[in] pszFromUserId 来源用户ID
            /// \param[in] pszFromUserName 来源用户名
            /// \param[in] seq 请求 seq
            virtual void OnJoinLiveResponse(int result, const char* pszFromUserId, const char* pszFromUserName, int seq) {};
            
            /// \breif 收到连麦邀请
            /// \param[in] seq 请求 seq
            /// \param[in] pszFromUserId 来源用户ID
            /// \param[in] pszFromUserName 来源用户名
            /// \param[in] pszRoomID 房间ID
            virtual void OnInviteJoinLiveRequest(int seq, const char* pszFromUserId, const char* pszFromUserName, const char* pszRoomID) {};
            
            
            virtual void OnVideoSizeChanged(const char* pStreamID, int nWidth, int nHeight) {};
            virtual void OnSnapshot(void *pImage, const char* pszStreamID) {};
            
            virtual ~ILivePlayerCallback() {}
        };
        
        class IZegoVideoRenderCallback
        {
        public:
            /// \brief 视频帧数据回调
            /// \param[in] pData 视频数据起始地址，windows 平台上回调数据格式为 RGBA32
            /// \param[in] dataLen 视频数据长度
            /// \param[in] pszStreamID 流名
            /// \param[in] width 视频宽
            /// \param[in] height 视频高
            /// \param[in] strides 每个平面一行字节数（RGBA 只需考虑 strides[0]）
            virtual void OnVideoDataCallback(const unsigned char *pData, int dataLen, const char* pszStreamID, int width, int height, int strides[4]) = 0;
        };
    }
    
#ifndef ZegoAVCallback_h
    namespace AV
    {
        class IZegoAudioRecordCallback
        {
        public:
            virtual void OnAudioRecordCallback(const unsigned char *pData, int data_len, int sample_rate, int num_channels, int bit_depth) = 0;
        };
    }
#endif
    
}


#endif /* LiveRoomCallback_Player_h */
