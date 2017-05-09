//
//  LiveRoomCallback-Publisher.h
//  zegoliveroom
//
//  Created by Randy Qiu on 2017/1/23.
//
//

#ifndef LiveRoomCallback_Publisher_h
#define LiveRoomCallback_Publisher_h

#include "./LiveRoomDefines-Publisher.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        class ILivePublisherCallback
        {
        public:
            /// \brief 推流状态更新
            /// \param[in] stateCode 状态码
            /// \param[in] pszStreamID 流ID
            /// \param[in] oStreamInfo 推流信息
            virtual void OnPublishStateUpdate(int stateCode, const char* pszStreamID, const ZegoPublishingStreamInfo& oStreamInfo) = 0;
            
            /// \brief 收到连麦请求
            /// \param[in] seq 请求 seq
            /// \param[in] pszFromUserId 来源用户ID
            /// \param[in] pszFromUserName 来源用户名
            /// \param[in] pszRoomID 房间ID
            virtual void OnJoinLiveRequest(int seq, const char *pszFromUserId, const char *pszFromUserName, const char *pszRoomID) {}
            
            /// \brief 收到邀请连麦结果
            /// \param[in] result 结果
            /// \param[in] pszFromUserId 来源用户ID
            /// \param[in] pszFromUserName 来源用户名
            /// \param[in] seq 请求 seq
            virtual void OnInviteJoinLiveResponse(int result, const char *pszFromUserId, const char *pszFromUserName, int seq) {}
            
            virtual void OnPublishQulityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS) {}
            
            virtual void OnCaptureVideoSizeChanged(int nWidth, int nHeight) {}
            
            virtual void OnPreviewSnapshot(void *pImage) {}
            
            virtual void OnAuxCallback(unsigned char *pData, int *pDataLen, int *pSampleRate, int *pNumChannels) {}
            
            virtual void OnUpdateMixStreamConfig(unsigned int uiErrorCode, const char* pszMixStreamID, const ZegoPublishingStreamInfo& oStreamInfo) {}
            
            virtual ~ILivePublisherCallback() {}
        };
    }
}
#endif /* LiveRoomCallback_Publisher_h */
