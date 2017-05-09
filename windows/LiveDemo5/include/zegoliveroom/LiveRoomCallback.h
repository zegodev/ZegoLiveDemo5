//
//  ZegoLiveRoomCallback.h
//  ZegoRoom
//
//  Copyright © 2017年 ZEGO. All rights reserved.
//

#ifndef ZegoLiveRoomCallback_h
#define ZegoLiveRoomCallback_h

#include "./LiveRoomDefines.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        class IRoomCallback
        {
        public:
            /// \brief 登陆成功回调
            /// \param[in] errorCode 错误码，0 为无错误
            /// \param[in] pszRoomID 房间 ID
            /// \param[in] pStreamInfo 直播流列表
            /// \param[in] streamCount 直播流个数
            virtual void OnLoginRoom(int errorCode, const char *pszRoomID, const ZegoStreamInfo *pStreamInfo, unsigned int streamCount) = 0;
            
            /// \brief 退出房间回调
            /// \param[in] errorCode 错误码，0 位无错误
            /// \param[in] pszRoomID 房间 ID
            virtual void OnLogoutRoom(int errorCode, const char *pszRoomID) {}
            
            /// \brief 因为登陆抢占原因等被挤出房间
            /// \param[in] reason 原因
            /// \param[in] pszRoomID 房间 ID
            virtual void OnKickOut(int reason, const char *pszRoomID) = 0;
            
            /// \brief 与 server 断开
            /// \param[in] errorCode 错误码，0 位无错误
            /// \param[in] pszRoomID 房间 ID
            virtual void OnDisconnect(int errorCode, const char *pszRoomID) = 0;
        
            /// \brief 流信息更新
            /// \param[in] type 更新类型
            /// \param[in] pStreamInfo 直播流列表
            /// \param[in] streamCount 直播流个数
            /// \param[in] pszRoomID 房间 ID
            virtual void OnStreamUpdated(ZegoStreamUpdateType type, ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID) = 0;
            
            /// \brief 流附加信息更新
            /// \param[in] pStreamInfo 附加信息更新的流列表
            /// \param[in] streamCount 流个数
            /// \param[in] pszRoomID 房间 ID
            virtual void OnStreamExtraInfoUpdated(ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID) = 0;
            
            /// \brief 发送自定义信令结果
            /// \params[in] errorCode 错误码
            /// \params[in] requestSeq 请求seq
            virtual void OnCustomCommand(int errorCode, int requestSeq, const char *pszRoomID) = 0;
            
            /// \brief 收到自定义信令
            /// \brief[in] seq 收到的信令seq
            /// \brief[in] pszUserId 发送者UserId
            /// \brief[in] pszUserName 发送者UserName
            /// \brief[in] pszContent 收到的信令内容
            virtual void OnRecvCustomCommand(const char *pszUserId, const char *pszUserName, const char *pszContent, const char *pszRoomID) = 0;
            
            virtual ~IRoomCallback() {}
        };
    }
    
#ifndef ZegoAVCallback_h
    namespace AV
    {
        class IZegoLiveEventCallback
        {
        public:
            virtual void OnAVKitEvent(int event, EventInfo* pInfo) = 0;
        };
        
        class IZegoDeviceStateCallback
        {
        public:
#ifdef WIN32
            virtual void OnAudioDeviceStateChanged(AudioDeviceType deviceType, DeviceInfo *deviceInfo, DeviceState state) = 0;
            virtual void OnVideoDeviceStateChanged(DeviceInfo *deviceInfo, DeviceState state) = 0;
#endif
            virtual void OnDeviceError(const char* deviceName, int errorCode) {}
        };
    }
#endif
}

#endif /* ZegoLiveRoomCallback_h */
