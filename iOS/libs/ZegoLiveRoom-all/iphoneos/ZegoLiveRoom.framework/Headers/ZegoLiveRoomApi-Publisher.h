//
//  ZegoLiveRoomApi-Publisher.h
//  ZegoLiveRoom
//
//  Created by Randy Qiu on 2017/1/24.
//  Copyright © 2017年 zego. All rights reserved.
//


#import "ZegoLiveRoomApi.h"
#import "ZegoLiveRoomApiDefines-Publisher.h"
#import "ZegoVideoCapture.h"

@protocol ZegoLivePublisherDelegate;


@interface ZegoLiveRoomApi (Publisher)

- (bool)setPublisherDelegate:(id<ZegoLivePublisherDelegate>)publisherDelegate;

// * live publish
/// \brief 设置本地预览视图
/// \param[in] view 用于渲染本地预览视频的视图
/// \return true 成功，false 失败
- (bool)setPreviewView:(ZEGOView *)view;

/// \brief 启动本地预览
/// \return true 成功，false 失败
- (bool)startPreview;

/// \brief 结束本地预览
/// \return true 成功，false 失败
- (bool)stopPreview;

/// \brief 开始直播
/// \param[in] streamID 流ID
/// \param[in] title 直播的名称
/// \param[in] flag 直播属性 参考 ZegoApiPublishFlag 定义
/// \return true，成功，等待 [ZegoLivePublisherDelegate -onPublishStateUpdate:streamID:streamInfo:] 回调，false 失败
- (bool)startPublishing:(NSString *)streamID title:(NSString *)title flag:(int)flag;

/// \brief 开始直播，流附加信息
/// \param[in] streamID 流ID
/// \param[in] title 直播的名称
/// \param[in] flag 直播属性 参考 ZegoApiPublishFlag 定义
/// \param[in] extraInfo 流附加信息
/// \return true，成功，等待 [ZegoLivePublisherDelegate -onPublishStateUpdate:streamID:streamInfo:] 回调，false 失败
- (bool)startPublishing:(NSString *)streamID title:(NSString *)title flag:(int)flag extraInfo:(NSString *)extraInfo;

/// \brief 更新流附加信息
/// \param[in] extraInfo 流附加信息
/// \return true 成功，其他人会收到[ZegoLiveRoomDelegate -onStreamExtraInfoUpdated:roomID] 回调
/// \note 更新附加信息会覆盖上次设置的附加信息
- (bool)updateStreamExtraInfo:(NSString *)extraInfo;

/// \brief 停止直播
/// \return true 成功，false 失败
- (bool)stopPublishing;

/// \brief 回应连麦申请
/// \param[in] seq 回应的邀请 seq (通过 [ZegoLivePublisherDelegate -onJoinLiveRequest:fromUserID:fromUserName:seq:] 收到申请)
/// \param[in] rspResult 回应，0 为同意
/// \return true 成功，false 失败
- (bool)respondJoinLiveReq:(int)seq result:(int)rspResult;

/// \breif 邀请连麦
/// \note 该用户会收到 [ZegoLivePlayerDelegate -onInviteJoinLiveRequest:fromUserID:fromUserName:roomID:] 回调
/// \param[in] userID 欲邀请的用户 ID
/// \return true 成功，通过 blk 回调结果，false 失败
- (bool)inviteJoinLive:(NSString *)userID responseBlock:(ZegoResponseBlock)blk;

/// \breif 设置混流数据配置
/// \param config 配置
/// \return true 成功，false 失败
- (bool)setMixStreamConfig:(NSDictionary *)config;

/// \brief 更新混流配置
/// \param lstMixStreamInfo 混流配置列表，按列表顺序叠加涂层
/// \note lstMixStreamInfo 设置为nil时停止混流
/// \return true 成功，等待异步结果回调，否则失败
- (bool)updateMixInputStreams:(NSArray<ZegoMixStreamInfo*> *)lstMixStreamInfo;

/// \brief 开关硬件编码
/// \param bRequire 开关
/// \note ！！！打开硬编硬解开关需后台可控，避免碰到版本升级或者硬件升级时出现硬编硬解失败的问题
+ (bool)requireHardwareEncoder:(bool)bRequire;

/// \brief 设置视频配置
/// \param config 配置参数
/// \return true 成功，false 失败
- (bool)setAVConfig:(ZegoAVConfig *)config;

#if TARGET_OS_IPHONE
/// \brief 设置手机姿势，用于校正主播输出视频朝向
/// \param orientation 手机姿势
- (bool)setAppOrientation:(UIInterfaceOrientation)orientation;
#endif

/// \brief 主播方开启美颜功能
/// \param feature 美颜特性
/// \return true: 成功；false: 失败
- (bool)enableBeautifying:(int)feature;

/// \brief 设置美颜磨皮的采样步长
/// \param step 采样半径 取值范围[1,16]
- (bool)setPolishStep:(float)step;

/// \brief 设置美颜采样颜色阈值
/// \param factor 取值范围[0,16]
/// \return true 成功，否则失败
- (bool)setPolishFactor:(float)factor;

/// \brief 设置美颜美白的亮度修正参数
/// \param factor 取值范围[0,1]， 参数越大亮度越暗
- (bool)setWhitenFactor:(float)factor;

/// \brief 设置锐化参数
/// \param factor 取值范围[0,2]，参数边缘越明显
- (bool)setSharpenFactor:(float)factor;

/// \brief 配置滤镜
/// \param filter 滤镜种类
- (bool)setFilter:(ZegoFilter)filter;

/// \brief 设置本地预览视频View的模式
/// \param mode 模式，详见ZegoVideoViewMode
/// \return true:调用成功；false:调用失败
- (bool)setPreviewViewMode:(ZegoVideoViewMode)mode;

/// \brief 设置预览渲染朝向
/// \param rotate 旋转角度
/// \return true 成功，false 失败
/// \note 使用setAppOrientation 替代
- (bool)setPreviewRotation:(int)rotate;

/// \brief 是否启用前摄像头预览镜像
/// \param enable true 启用，false 不启用
/// \return true 成功，否则失败
- (bool)enablePreviewMirror:(bool)enable;

/// \brief 是否启用摄像头采集结果镜像
/// \param enable true 启用，false 不启用
/// \return true 成功，否则失败
- (bool)enableCaptureMirror:(bool)enable;

/// \brief 是否开启码率控制（在带宽不足的情况下码率自动适应当前带宽)
/// \param enable true 启用，false 不启用
/// \return true 成功，否则失败
- (bool)enableRateControl:(bool)enable;

/// \brief 设置是否使用前置摄像头
/// \param bFront 使用前置摄像头
/// \return true:调用成功；false:调用失败
- (bool)setFrontCam:(bool)bFront;

/// \brief 开启关闭麦克风
/// \param bEnable true打开，false关闭
/// \return true:调用成功；false:调用失败
- (bool)enableMic:(bool)bEnable;

/// \brief 开启关闭视频采集
/// \param bEnable true打开，false关闭
/// \return true:调用成功；false:调用失败
- (bool)enableCamera:(bool)bEnable;

/// \brief 开关手电筒
/// \param bEnable true打开，false关闭
/// \return true：成功；false:失败
- (bool)enableTorch:(bool) bEnable;

- (bool)takePreviewSnapshot:(ZegoSnapshotCompletionBlock)blk;

/// \brief 开启采集监听
/// \param bEnable true打开，false关闭
/// \return true:调用成功；false:调用失败
- (bool)enableLoopback:(bool)bEnable;

/// \biref 设置采集监听音量
/// \param volume 音量大小 0 ~ 100
- (void)setLoopbackVolume:(int)volume;

/// \brief 混音开关
/// \param enable true 启用混音输入；false 关闭混音输入
/// \return true 成功，否则失败
- (bool)enableAux:(BOOL)enable;

/// \brief 混音输入播放静音开关
/// \param bMute true: aux 输入播放静音；false: 不静音
/// \return true 成功，否则失败
- (bool)muteAux:(bool)bMute;

/// \brief 获取当前采集的音量
/// \return 当前采集音量大小
- (float)getCaptureSoundLevel;

/// \brief 设置水印的图片路径
/// \param filePath 图片路径。如果是完整路径则添加 file: 前缀，如：@"file:/var/image.png"；资产则添加 asset: 前缀，如：@"asset:watermark"
- (void)setWaterMarkImagePath:(NSString *)filePath;

/// \brief 设置水印在采集video中的位置
/// \note  左上角为坐标系原点,区域不能超过编码分辨率设置的大小
- (void)setPublishWaterMarkRect:(CGRect)waterMarkRect;

/// \brief 设置水印在预览video中的位置
/// \note 左上角为坐标系原点,区域不能超过preview的大小
- (void)setPreviewWaterMarkRect:(CGRect)waterMarkRect;

/// \brief 设置外部采集模块
/// \param factory 工厂对象
/// \note 必须在 InitSDK 前调用，并且不能置空
+ (void)setVideoCaptureFactory:(id<ZegoVideoCaptureFactory>)factory;

/// \brief 设置外部滤镜模块
/// \param factory 工厂对象
/// \note 必须在 Init 前调用，并且不能置空
+ (void)setVideoFilterFactory:(id<ZegoVideoFilterFactory>)factory;

/// \brief 设置音频前处理函数
/// \param prep 前处理函数指针
/// \note 必须在InitSDK前调用，并且不能置空
+ (void)setAudioPrep:(void(*)(const short* inData, int inSamples, int sampleRate, short *outData))prep;

@end


@protocol ZegoLivePublisherDelegate <NSObject>

/// \brief 推流状态更新
/// \param[in] stateCode 状态码
/// \param[in] streamID 流ID
/// \param[in] info 推流信息
- (void)onPublishStateUpdate:(int)stateCode streamID:(NSString *)streamID streamInfo:(NSDictionary *)info;

@optional

/// \brief 收到连麦请求
/// \param[in] seq 请求 seq
/// \param[in] userId 来源用户ID
/// \param[in] userName 来源用户名
/// \param[in] roomID 房间ID
- (void)onJoinLiveRequest:(int)seq fromUserID:(NSString *)userId fromUserName:(NSString *)userName roomID:(NSString *)roomID;

/// \brief 发布质量更新
/// \param quality 0 ~ 3 分别对应优良中差
/// \param streamID 发布流ID
/// \param fps 帧率(frame rate)
/// \param kbs 码率(bit rate) kb/s
- (void)onPublishQualityUpdate:(int)quality stream:(NSString *)streamID videoFPS:(double)fps videoBitrate:(double)kbs;

/// \brief 采集视频的宽度和高度变化通知
/// \param size 视频大小
- (void)onCaptureVideoSizeChangedTo:(CGSize)size;

/// \brief 混流配置更新结果回调
/// \param errorCode 错误码，0 表示没有错误
/// \param mixStreamID 混流ID
/// \param info 混流播放信息
- (void)onMixStreamConfigUpdate:(int)errorCode mixStream:(NSString *)mixStreamID streamInfo:(NSDictionary *)info;


/// \brief 混音数据输入回调
/// \param pData 数据缓存起始地址
/// \param pDataLen [in] 缓冲区长度；[out]实际填充长度，必须为 0 或是缓冲区长度，代表有无混音数据
/// \param pSampleRate 混音数据采样率
/// \param pChannelCount 混音数据声道数
/// \note 混音数据 bit depth 必须为 16
- (void)onAuxCallback:(void *)pData dataLen:(int *)pDataLen sampleRate:(int *)pSampleRate channelCount:(int *)pChannelCount;
@end

