//
//  ZegoLiveRoomApi-Player.h
//  ZegoLiveRoom
//
//  Created by Randy Qiu on 2017/1/24.
//  Copyright © 2017年 zego. All rights reserved.
//

#import "ZegoLiveRoomApi.h"
#import "ZegoLiveRoomApiDefines.h"

@protocol ZegoLivePlayerDelegate;
@protocol ZegoLiveApiRenderDelegate;
@protocol ZegoLiveApiAudioRecordDelegate;


@interface ZegoLiveRoomApi (Player)

- (bool)setPlayerDelegate:(id<ZegoLivePlayerDelegate>)playerDelegate;

/// \brief 播放直播流
/// \param[in] streamID 流 ID
/// \param[in] view 用来渲染播放视频的视图
/// \return true 成功，等待 [ZegoLivePlayerDelegate -onPlayStateUpdate:streamID:] 回调，false 失败
- (bool)startPlayingStream:(NSString *)streamID inView:(ZEGOView *)view;

- (bool)updatePlayView:(ZEGOView *)view ofStream:(NSString *)streamID;

/// \brief 停止播放
/// \param[in] streamID 流 ID
/// \return true 成功，false 失败
- (bool)stopPlayingStream:(NSString *)streamID;

/// \brief 申请连麦
/// \note 主播会收到 [ZegoLivePublisherDelegate -onJoinLiveRequest:fromUserID:fromUserName:roomID:] 回调
/// \return true 成功，通过 blk 回调结果，false 失败
- (bool)requestJoinLive:(ZegoResponseBlock)blk;

/// \brief 回应连麦邀请
/// \note 主播会收到 [ZegoLivePublisherDelegate -onInviteJoinLiveResponse:fromUserID:fromUserName:seq:] 回调
/// \param[in] seq 回应的邀请 seq (通过 [ZegoLivePlayerDelegate -onInviteJoinLiveRequest:fromUserID:fromUserName:seq:] 收到邀请)
/// \param[in] rspResult 回应，0 为同意
/// \return true 成功，false 失败
- (bool)respondInviteJoinLiveReq:(int)seq result:(int)rspResult;

/// \brief 开关硬件解码
/// \param bRequire 开关
/// \note ！！！打开硬编硬解开关需后台可控，避免碰到版本升级或者硬件升级时出现硬编硬解失败的问题
+ (bool)requireHardwareDecoder:(bool)bRequire;

/// \brief （声音输出）静音开关
/// \param bEnable true打开，false关闭
/// \return true：成功；false:失败
- (bool)enableSpeaker:(bool) bEnable;

/// \brief 手机内置扬声器开关
/// \param bOn true打开，false关闭
/// \return true：成功；false:失败
- (bool)setBuiltInSpeakerOn:(bool)bOn;

/// \biref 设置播放音量
/// \param volume 音量大小 0 ~ 100
- (void)setPlayVolume:(int)volume;

/// \brief 获取当前播放视频的音量
/// \param[in] streamID 播放流名
/// \return 对应视频的音量
- (float)getSoundLevelOfStream:(NSString *)streamID;

/// \brief 设置观看直播的View的模式
/// \param mode 模式，详见ZegoVideoViewMode
/// \param streamID 播放流名
/// \return true:调用成功；false:调用失败
- (bool)setViewMode:(ZegoVideoViewMode)mode ofStream:(NSString *)streamID;

/// \brief 设置播放渲染朝向
/// \param rotate 逆时针旋转角度(0/90/180/270)
/// \param streamID 播放流名
/// \return true 成功，false 失败
- (bool)setViewRotation:(int)rotate ofStream:(NSString *)streamID;

/// \brief 截取观看直播 view 图像
/// \param streamID 流名
/// \return true 成功，false 失败
- (bool)takeSnapshotOfStream:(NSString *)streamID withCompletionBlock:(ZegoSnapshotCompletionBlock)blk;

/// \brief 设置拉流质量监控周期
/// \param timeInMS 时间周期，单位为毫秒，取值范围：(500, 60000)
+ (void)setPlayQualityMonitorCycle:(unsigned int)timeInMS;

/// \brief 设置外部渲染
/// \param bEnable 是否外部渲染
/// \note 必须在InitSDK前调用
+ (void)enableExternalRender:(BOOL)bEnable;

/// \brief 外部渲染回调
/// \param renderDelegate 外部渲染回调协议
- (void)setRenderDelegate:(id<ZegoLiveApiRenderDelegate>)renderDelegate;

/// \brief 音频录制回调开关
/// \param enable true 启用音频录制回调；false 关闭音频录制回调
/// \return true 成功，false 失败
- (bool)enableAudioRecord:(BOOL)enable;

/// \brief 音频录制回调
/// \param audioRecordDelegate 音频录制回调协议
- (void)setAudioRecordDelegate:(id<ZegoLiveApiAudioRecordDelegate>)audioRecordDelegate;

/// \brief 获取 SDK 支持的最大同时播放流数
/// \return 最大支持播放流数
+ (int)getMaxPlayChannelCount;

@end


@protocol ZegoLivePlayerDelegate <NSObject>

/// \brief 播放流事件
/// \param[in] stateCode 播放状态码
/// \param[in] streamID 流ID
- (void)onPlayStateUpdate:(int)stateCode streamID:(NSString *)streamID;

@optional

/// \breif 收到连麦邀请
/// \param[in] seq 请求 seq
/// \param[in] userId 来源用户ID
/// \param[in] userName 来源用户名
/// \param[in] roomID 房间ID
- (void)onInviteJoinLiveRequest:(int)seq fromUserID:(NSString *)userId fromUserName:(NSString *)userName roomID:(NSString *)roomID;

/// \brief 视频宽高变化通知,startPlay后，如果视频宽度或者高度发生变化(首次的值也会)，则收到该通知
/// \param size 视频大小
/// \param streamID 流的唯一标识
- (void)onVideoSizeChangedTo:(CGSize)size ofStream:(NSString *)streamID;

/// \brief 观看质量更新
/// \param quality 0 ~ 3 分别对应优良中差
/// \param streamID 观看流ID
/// \param fps 帧率(frame rate)
/// \param kbs 码率(bit rate) kb/s
- (void)onPlayQualityUpdate:(int)quality stream:(NSString *)streamID videoFPS:(double)fps videoBitrate:(double)kbs;


@end

@protocol ZegoLiveApiRenderDelegate <NSObject>

/// \brief 从外部获取渲染需要的PixelBuffer地址
/// \param width 视频宽度
/// \param height 视频高度
/// \param stride 图像stride padding
- (CVPixelBufferRef)onCreateInputBufferWithWidth:(int)width height:(int)height stride:(int)stride;

/// \breif 视频数据拷贝完成
/// \param pixelBuffer 拷贝完的视频数据
/// \param streamID 流名
- (void)onPixelBufferCopyed:(CVPixelBufferRef)pixelBuffer ofStream:(NSString *)streamID;

@end

@protocol ZegoLiveApiAudioRecordDelegate <NSObject>

- (void)onAudioRecord:(NSData *)audioData sampleRate:(int)sampleRate numOfChannels:(int)numOfChannels bitDepth:(int)bitDepth;

@end

