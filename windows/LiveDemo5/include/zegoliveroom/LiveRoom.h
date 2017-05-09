//
//  ZegoLiveRoom.h
//  zegoliveroom
//
//  Copyright © 2017年 Zego. All rights reserved.
//

#ifndef ZegoLiveRoom_h
#define ZegoLiveRoom_h

#include "./LiveRoomDefines.h"
#include "./LiveRoomCallback.h"
#include "./LiveRoomDefines-IM.h"

namespace ZEGO
{
	namespace LIVEROOM
	{
        ZEGO_API const char* GetSDKVersion();
        ZEGO_API const char* GetSDKVersion2();
        ZEGO_API bool SetNetType(int nNetType);
        ZEGO_API bool SetLogDir(const char* pszLogDir);
        ZEGO_API void UploadLog();
        ZEGO_API void SetVerbose(bool bVerbose);
        ZEGO_API void SetUseTestEnv(bool bTestEnv);
        ZEGO_API void SetUseAlphaEnv(bool bAlpha);
        ZEGO_API void SetPlatformInfo(const char* pszInfo);
        
        /// \brief 设置业务类型
        /// \param[in] nType 类型，默认为 0
        /// \return true: 调用成功; false: 调用失败
        ZEGO_API bool SetBusinessType(int nType);
        
        /// \brief 设置是否开启聊天室
        /// \note businessType为2时有效
        /// \param[in] bChatRoom
        /// \return true 调用成功; false 调用失败
        ZEGO_API bool SetUseChatRoom(bool bChatRoom);
        
        /// \brief 设置用户信息
        /// \param[in] pszUserID 用户唯一ID
        /// \param[in] pszUserName 用户名字
        /// \return true 成功，false 失败
        ZEGO_API bool SetUser(const char* pszUserID, const char* pszUserName);

        /// \brief 获取用户信息
        /// \return UserID
        ZEGO_API const char* GetUserID();
        
		/// \brief 初始化引擎
		/// \note jvm 与 ctx 仅用于 android
        /// \return true 成功，false 失败
		ZEGO_API bool InitPlatform(void* jvm = 0, void* ctx = 0);
        
        /// \brief 初始化SDK
        /// \param[in] uiAppID Zego派发的数字ID, 各个开发者的唯一标识
        /// \param[in] pBufAppSignature Zego派发的签名, 用来校验对应appID的合法性
        /// \param[in] nSignatureSize 签名长度（字节）
        /// \return true 成功，false 失败
        ZEGO_API bool InitSDK(unsigned int uiAppID, unsigned char* pBufAppSignature, int nSignatureSize);
        
        /// \brief 反初始化SDK
        /// \return true 成功，false 失败
        ZEGO_API bool UnInitSDK();
        
        /// \brief 设置直播房间相关信息通知的回调
        /// \param[in] pCB 回调对象指针
        /// \return true 成功，false 失败
        ZEGO_API bool SetRoomCallback(IRoomCallback* pCB);

        /// \brief 设置房间附加信息
        /// \param[in] audienceCreateRoom 观众是否可以创建房间
        /// \param[in] userStateUpdate 用户上线/下线状态是否广播
        ZEGO_API void SetRoomConfig(bool audienceCreateRoom, bool userStateUpdate);
        
        /// \brief 登陆房间
        /// \param[in] pszRoomID 房间唯一ID
        /// \param[in] role 成员角色
        /// \return true 成功，等待 IRoomCallback::OnLoginRoom 回调，false 失败
        ZEGO_API bool LoginRoom(const char* pszRoomID, int role, const char* pszRoomName = "");
        
        /// \brief 退出房间
        /// \note 会停止所有的推拉流
        /// \return true 成功，等待 IRoomCallback::OnLogoutRoom 回调，false 失败
        ZEGO_API bool LogoutRoom();
        
        /// \brief 发送自定义信令
        /// \param[in] memberList 信令发送成员列表
        /// \param[in] memberCount 成员个数
        /// \param[in] content 信令内容
        ZEGO_API int SendCustomCommand(ROOM::ZegoUser *memberList, unsigned int memberCount, const char *content);
        
        /// \breif 设置回调
        /// \param[in] pCB 回调对象指针
        ZEGO_API void SetLiveEventCallback(AV::IZegoLiveEventCallback* pCB);
        
        
        /// \brief 设置音频视频设备变化的回调
        /// \param[in] pCB 回调对象指针
        ZEGO_API void SetDeviceStateCallback(AV::IZegoDeviceStateCallback *pCB);
        
        
        //
        // * device
        //
        
        /// \brief 获取音频设备列表
        /// \param[in] deviceType 设备类型
        /// \return 音频设备列表
        ZEGO_API AV::DeviceInfo* GetAudioDeviceList(AV::AudioDeviceType deviceType, int& device_count);
        
        /// \brief 设置选用音频设备
        /// \param[in] deviceType 设备类型
        /// \param[in] pszDeviceID 设备 ID
        /// \return true: 调用成功; false: 调用失败
        ZEGO_API bool SetAudioDevice(AV::AudioDeviceType deviceType, const char* pszDeviceID);
        
        /// \brief 获取视频设备列表
        /// \return 视频频设备列表
        ZEGO_API AV::DeviceInfo* GetVideoDeviceList(int& device_count);
        
        /// \brief 释放设备列表
        /// \param parrDeviceList 设备列表
        ZEGO_API void FreeDeviceList(AV::DeviceInfo* parrDeviceList);
        
        /// \brief 设置选用视频设备
        /// \param[in] pszDeviceID 设备 ID
        /// \return true: 调用成功; false: 调用失败
        ZEGO_API bool SetVideoDevice(const char* pszDeviceID);
        
#ifdef WIN32
        /// \brief 系统声卡声音采集开关
        /// \param[in] bEnable 是否打开
        ZEGO_API void EnableMixSystemPlayout(bool bEnable);
        
        /// \brief 获取麦克风音量
        /// \param[in] deviceid 麦克风deviceid
        /// \return -1: 获取失败 0 ~100 麦克风音量
        /// \note 切换麦克风后需要重新获取麦克风音量
        ZEGO_API int GetMicDeviceVolume(const char *deviceId);
        
        /// \brief 设置麦克风音量
        /// \param[in] deviceid 麦克风deviceid volume 音量 0 ~ 100
        ZEGO_API void SetMicDeviceVolume(const char *deviceId, int volume);
        
        /// \brief 获取扬声器音量
        /// \param[in] deviceid 杨声器deviceid
        /// \return -1: 获取失败 0 ~100 扬声器音量
        /// \note 切换扬声器后需要重新获取音量
        ZEGO_API int GetSpeakerDeviceVolume(const char *deviceId);
        
        /// \brief 设置扬声器音量
        /// \param[in] deviceid 杨声器deviceid volume 音量 0 ~ 100
        ZEGO_API void SetSpeakerDeviceVolume(const char *deviceId, int volume);
#endif // WIN32
	}
}

#endif
