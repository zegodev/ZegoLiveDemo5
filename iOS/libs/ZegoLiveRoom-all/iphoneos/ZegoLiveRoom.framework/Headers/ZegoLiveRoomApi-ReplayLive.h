//
//  ZegoLiveRoomApi-ReplayLive.h
//  ZegoLiveRoom
//
//  Created by Strong on 2017/3/22.
//  Copyright © 2017年 zego. All rights reserved.
//

#import "ZegoLiveRoomApi.h"
#import <ReplayKit/ReplayKit.h>

@interface ZegoLiveRoomApi (ReplayLive)

/// \brief 初始化ReplayLive 
/// \note必须在InitSDK前调用
+ (void)prepareReplayLiveCapture;

/// \brief 处理视频数据
/// \params sampleBuffer ReplayLiveKit返回的视频数据
- (void)handleVideoInputSampleBuffer:(CMSampleBufferRef)sampleBuffer;

/// \brief 处理音频数据
/// \params sampleBuffer ReplayLiveKit返回的音频数据
/// \params sampleBufferType 类型 RPSampleBufferTypeAudioApp, RPSampleBufferTypeAudioMic
- (void)handleAudioInputSampleBuffer:(CMSampleBufferRef)sampleBuffer withType:(RPSampleBufferType)sampleBufferType;

@end
