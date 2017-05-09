//
//  LiveRoom-Publisher.h
//  zegoliveroom
//
//  Created by Randy Qiu on 2017/1/23.
//
//

#ifndef LiveRoom_Publisher_h
#define LiveRoom_Publisher_h

#include "./LiveRoomDefines-Publisher.h"
#include "./LiveRoomCallback-Publisher.h"

namespace AVE
{
    class VideoCaptureFactory;
    class MediaCaptureFactory;
    class VideoFilterFactory;
}

namespace ZEGO
{
    namespace LIVEROOM
    {
        /// \brief 设置直播主播相关信息通知的回调
        /// \param[in] pCB 回调对象指针
        /// \return true 成功，false 失败
        ZEGO_API bool SetLivePublisherCallback(ILivePublisherCallback* pCB);
        
        /// \brief 设置本地预览试图
        /// \param[in] pView 用于渲染本地预览视频的视图
        /// \return true 成功，false 失败
        ZEGO_API bool SetPreviewView(void* pView);
        
        /// \brief 开启预览
        /// \return true 成功，false 失败
        ZEGO_API bool StartPreview();
        
        /// \brief 关闭预览
        /// \return true 成功，false 失败
        ZEGO_API bool StopPreview();
        
        /// \brief 设置/更新推流的附加信息
        /// \param[in] pszStreamExtraInfo 流附加信息
        /// \note 开播后流附加信息的更新，在房间的其他人会收到OnStreamExtraInfoUpdated通知
        ZEGO_API bool SetPublishStreamExtraInfo(const char *pszStreamExtraInfo);
        
        /// \brief 开始直播
        /// \param[in] pszTitle 直播的名称
        /// \param[in] pszStreamID 流ID
        /// \param[in] flag 直播属性 参考 enum ZegoPublishFlag 定义
        /// \param[in] pszParams 推流参数
        /// \return true，成功，等待 ILivePublisherCallback::OnPublishStateUpdate 回调，false 失败
        ZEGO_API bool StartPublishing(const char* pszTitle, const char* pszStreamID, int flag, const char* pszParams = 0);
        
        /// \brief 停止直播
        /// \param[in] flag 保留字段
        /// \param[in] pszMsg 自定义信息，server对接流结束回调包含此字段内容
        /// \return true 成功，false 失败
        ZEGO_API bool StopPublishing(int flag = 0, const char* pszMsg = 0);
        
        /// \brief 回应连麦申请
        /// \note 申请者会收到 OnJoinLiveResponse 回调
        /// \param[in] seq 收到的连麦申请 seq (通过 ILivePublisherCallback::OnJoinLiveRequest 收到申请)
        /// \param[in] rspResult 回应，0 为同意
        /// \return true 成功，false 失败
        ZEGO_API bool RespondJoinLiveReq(int seq, int rspResult);
        
        /// \breif 邀请连麦
        /// \note 该用户会收到 OnInviteJoinLiveRequest 回调
        /// \param[in] pszUserID 欲邀请的用户 ID
        /// \return 请求 seq，正值为有效，等待 ILivePublisherCallback::OnInviteJoinLiveResponse 回调，否则失败
        ZEGO_API int InviteJoinLive(const char* pszUserID);
        
        /// \breif 设置混流数据配置
        /// \param[in] pszMixStreamID 混流ID
        /// \param[in] nMixVideoWidth 混流后视频的宽
        /// \param[in] nMixVideoHeight 混流后视频的高
        /// \return true 成功，false 失败
        ZEGO_API bool SetMixStreamConfig(const char* pszMixStreamID, int nMixVideoWidth, int nMixVideoHeight);
        
        /// \brief 更新混流配置
        /// \param pConfigList 输入流配置数组首元素指针
        /// \param size 输入流个数
        ZEGO_API bool UpdateMixInputStreams(AV::ZegoMixStreamConfig* pConfigList, int size);
        
        /// \brief 硬件编码加速
        /// \param bRequired 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool RequireHardwareEncoder(bool bRequired);
        
        /// \brief 设置视频码率
        /// \param[in] nBitrate 码率
        /// \return true 成功，false 失败
        ZEGO_API bool SetVideoBitrate(int nBitrate);
        
        /// \brief 设置视频帧率
        /// \param[in] nFps 帧率
        /// \return true 成功，false 失败
        ZEGO_API bool SetVideoFPS(int nFps);
        
        /// \brief 设置视频编码输出分辨率
        /// \param[in] nWidth 宽
        /// \param[in] nHeight 高
        /// \return true 成功，false 失败
        ZEGO_API bool SetVideoEncodeResolution(int nWidth, int nHeight);
        
        /// \brief 设置视频采集分辨率
        /// \param[in] nWidth 宽
        /// \param[in] nHeight 高
        /// \return true 成功，false 失败
        ZEGO_API bool SetVideoCaptureResolution(int nWidth, int nHeight);
        
        /// \brief 美颜配置
        /// \param nFeature 美颜配置
        /// \return true 成功，false 失败
        ZEGO_API bool EnableBeautifying(int nFeature);
        
        /// \brief 设置美颜磨皮的采样步长
        /// \param step 采样半径 取值范围[1,16]
        ZEGO_API bool SetPolishStep(float step);
        
        /// \brief 设置美颜采样颜色阈值
        /// \param factor 取值范围[0,16]
        /// \return true 成功，否则失败
        ZEGO_API bool SetPolishFactor(float factor);
        
        /// \brief 设置美颜美白的亮度修正参数
        /// \param factor 取值范围[0,1]， 参数越大亮度越暗
        ZEGO_API bool SetWhitenFactor(float factor);
        
        /// \brief 设置锐化参数
        /// \param factor 取值范围[0,2]，参数边缘越明显
        ZEGO_API bool SetSharpenFactor(float factor);
        
        /// \brief 滤镜配置
        /// \param nIndex 滤镜索引
        /// \return true 成功，false 失败
        ZEGO_API bool SetFilter(int nIndex);
        
        /// \breif 设置预览视频模式
        /// \param[in] mode 模式
        /// \return true: 调用成功; false: 调用失败
        ZEGO_API bool SetPreviewViewMode(ZegoVideoViewMode mode);
        
        /// \brief 设置预览渲染朝向
        /// \param nRotation 旋转角度(0/90/180/270)
        /// \return true 成功，false 失败
        ZEGO_API bool SetPreviewRotation(int nRotation);
        
        /// \brief 音频采集开关
        /// \param bEnable 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool EnableMic(bool bEnable);
        
        /// \brief 摄像头开关
        /// \param bEnable 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool EnableCamera(bool bEnable);

        /// \brief 截预览图
        /// \return true 成功，通过回调返回结果，false 失败
        ZEGO_API bool TakeSnapshotPreview();
        
        /// \brief 音频采集监听开关
        /// \param bEnable 是否开启
        /// \return true 成功，false 失败
        ZEGO_API bool EnableLoopback(bool bEnable);
        
        /// \brief 设置监听音量
        /// \param[in] volume 音量 0 ~ 100
        ZEGO_API void SetLoopbackVolume(int volume);
        
        /// \brief 混音开关
        /// \param bEnable true 启用混音输入；false 关闭混音输入
        ZEGO_API bool EnableAux(bool bEnable);
        
        /// \brief 混音输入播放静音开关
        /// \param bMute true: aux 输入播放静音；false: 不静音
        ZEGO_API bool MuteAux(bool bMute);
        
        /// \brief 获取当前采集的音量
        /// \return 当前采集音量大小
        ZEGO_API float GetCaptureSoundLevel();
        
        /// \brief 设置水印的图片路径
        /// \param filePath 图片路径
        ZEGO_API void SetWaterMarkImagePath(const char *filePath);
        
        /// \brief 设置水印在采集video中的位置
        /// \note  左上角为坐标系原点,区域不能超过编码分辨率设置的大小
        ZEGO_API void SetPublishWaterMarkRect(int left, int top, int right, int bottom);
        
        /// \brief 设置水印在预览video中的位置
        /// \note 左上角为坐标系原点,区域不能超过preview的大小
        ZEGO_API void SetPreviewWaterMarkRect(int left, int top, int right, int bottom);
        
        /// \brief 设置外部采集模块
        /// \param factory 工厂
        /// \note 必须在 InitSDK 前调用，并且不能置空
        ZEGO_API void SetVideoCaptureFactory(AVE::VideoCaptureFactory* factory);
        
        /// \brief 设置外部采集模块(包含音频)
        /// \param factory 工厂
        /// \note 必须在 InitSDK 前调用，并且不能置空
        ZEGO_API void SetMediaCaptureFactory(AVE::MediaCaptureFactory* factory);
        
        /// \brief 设置外部滤镜模块
        /// \param factory 工厂对象
        /// \note 必须在 InitSDK 前调用，并且不能置空
        ZEGO_API void SetVideoFilterFactory(AVE::VideoFilterFactory* factory);
        
        /// \brief 设置音频前处理函数
        /// \param prep 前处理函数指针
        ZEGO_API void SetAudioPrep(void(*prep)(const short* inData, int inSamples, int sampleRate, short *outData));
        
        /// \brief 设置编码器码率控制策略
        /// \param strategy 策略配置，参考 ZegoVideoEncoderRateControlStrategy
        /// \param encoderCRF 当策略为恒定质量（ZEGO_RC_VBR/ZEGO_RC_CRF）有效，取值范围 [0~51]，越小质量越好，建议取值范围 [18, 28]
        ZEGO_API void SetVideoEncoderRateControlConfig(int strategy, int encoderCRF);
    }
}
#endif /* LiveRoom_Publisher_h */
