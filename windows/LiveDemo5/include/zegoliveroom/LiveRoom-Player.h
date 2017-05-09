//
//  LiveRoom-Player.h
//  zegoliveroom
//
//  Created by Randy Qiu on 2017/1/23.
//
//

#ifndef LiveRoom_Player_h
#define LiveRoom_Player_h

#include "./LiveRoomDefines.h"
#include "./LiveRoomCallback-Player.h"


namespace ZEGO
{
    namespace LIVEROOM
    {
        /// \brief 获取 SDK 支持的最大同时播放流数
        /// \return 最大支持播放流数
        ZEGO_API int GetMaxPlayChannelCount();
        
        /// \brief 设置直播观众相关信息通知的回调
        /// \param[in] pCB 回调对象指针
        /// \return true 成功，false 失败
        ZEGO_API bool SetLivePlayerCallback(ILivePlayerCallback* pCB);
        
        /// \brief 播放直播流
        /// \param[in] pszStreamID 流 ID
        /// \param[in] pView 用来渲染播放视频的视图
        /// \param[in] pszParams 拉流参数
        /// \return true 成功，等待 ILivePlayerCallback::OnPlayStateUpdate 回调，false 失败
        ZEGO_API bool StartPlayingStream(const char* pszStreamID, void* pView, const char* pszParams = 0);
                
        /// \brief 停止播放
        /// \param[in] pszStreamID 流 ID
        /// \return true 成功，false 失败
        ZEGO_API bool StopPlayingStream(const char* pszStreamID);
        
        /// \brief 申请连麦
        /// \note 主播会收到 ILivePublisherCallback::OnJoinLiveRequest 回调
        /// \return 请求 seq，正值为有效，等待 ILivePlayerCallback::OnJoinLiveResponse 回调，回调带回的 seq 与此函数返回值一致
        ZEGO_API int RequestJoinLive();
        
        /// \brief 回应连麦邀请
        /// \note 主播会收到 OnInviteJoinLiveResponse 回调
        /// \param[in] seq 回应的邀请 seq (通过 ILivePlayerCallback::OnInviteJoinLiveRequest 收到邀请)
        /// \param[in] rspResult 回应，0 为同意
        /// \return true 成功，false 失败
        ZEGO_API bool RespondInviteJoinLiveReq(int seq, int rspResult);
        
        
        // * player config
        
        /// \brief 硬件解码加速
        /// \param bRequired 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool RequireHardwareDecoder(bool bRequired);
        
        /// \brief 播放声音开关
        /// \param bEnable 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool EnableSpeaker(bool bEnable);
        
        /// \brief 手机外放开关
        /// \param bOn 是否开启，true 声音从外放播放
        /// \return true 成功，false 失败
        ZEGO_API bool setBuiltInSpeakerOn(bool bOn);
        
        /// \brief 设置播放音量
        /// \param[in] volume 音量 0 ~ 100
        ZEGO_API void SetPlayVolume(int volume);
        
        /// \brief 获取当前播放视频的音量
        /// \param[in] pszStreamID 流名
        /// \return 对应视频的音量
        ZEGO_API float GetSoundLevel(const char* pszStreamID);
        
        /// \breif 设置播放视频模式
        /// \param[in] mode 模式
        /// \param[in] pszStreamID 流名
        /// \return true 成功，false 失败
        ZEGO_API bool SetViewMode(ZegoVideoViewMode mode, const char* pszStreamID);
        
        /// \brief 截图
        /// \param pszStreamID 流名
        /// \return true 成功，通过回调 ILivePlayerCallback::OnSnapshot 返回结果，false 失败
        ZEGO_API bool TakeSnapshot(const char* pszStreamID);
        
        
        // * advanced config
        
        /// \brief 设置播放渲染朝向
        /// \param[in] nRotation 旋转角度(0/90/180/270)
        /// \param[in] pszStreamID 流名
        /// \return true 成功，false 失败
        ZEGO_API bool SetViewRotation(int nRotation, const char* pszStreamID);
        
        /// \brief 设置拉流质量监控周期
        /// \param timeInMS 时间周期，单位为毫秒，取值范围：(500, 60000)
        /// \return true 设置成功，否则失败
        ZEGO_API bool SetPlayQualityMonitorCycle(unsigned int timeInMS);
        
        
        // * external video render
        
        /// \brief 设置是否使用外部视频渲染
        /// \param bEnable 是否开启
        ZEGO_API void EnableExternalRender(bool bEnable);
        
        /// \brief 设置外部渲染回调
        /// \param pCB 回调实现实例
        ZEGO_API void SetExternalRenderCallback(IZegoVideoRenderCallback* pCB);
        
        
        // * audio record
        
        /// \brief 音频录制回调开关
        /// \param bEnable true 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool EnableAudioRecord(bool bEnable);
        
        /// \brief 设置音频录制回调
        /// \param pCB 回调实现实例
        /// \return true 成功，false 失败
        ZEGO_API bool SetAudioRecordCallback(AV::IZegoAudioRecordCallback* pCB);
    }
}

#endif /* LiveRoom_Player_h */
