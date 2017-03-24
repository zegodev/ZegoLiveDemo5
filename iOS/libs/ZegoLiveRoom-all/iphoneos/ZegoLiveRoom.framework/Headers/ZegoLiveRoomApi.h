//
//  ZegoLiveRoomApi.h
//  ZegoLiveRoom
//
//  Copyright © 2017年 zego. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ZegoLiveRoomApiDefines.h"




@protocol ZegoRoomDelegate;
@protocol ZegoLiveEventDelegate;
@protocol ZegoDeviceEventDelegate;

typedef void(^ZegoLoginCompletionBlock)(int errorCode, NSArray<ZegoStream*> *streamList);
typedef void(^ZegoResponseBlock)(int result, NSString *fromUserID, NSString *fromUserName);

@interface ZegoLiveRoomApi : NSObject

+ (NSString *)version;
+ (NSString *)version2;

/// \brief 是否启用测试环境
/// \param useTestEnv 是否使用测试环境
+ (void)setUseTestEnv:(bool)useTestEnv;

/// \brief 调试信息开关
/// \desc 建议在调试阶段打开此开关，方便调试。默认关闭
/// \param bOnVerbose 是否使用测试环境
+ (void)setVerbose:(bool)bOnVerbose;

/// \brief 设置业务类型
/// \param type 业务类型，默认为 0
/// \note 确保在创建接口对象前调用
+ (void)setBusinessType:(int)type;

/// \brief 设置是否使用聊天室功能
/// \param bChatRoom 是否开启
/// \note 在BusinessType为2时才生效
+ (void)setUseChatRoom:(bool)bChatRoom;

/// \brief 触发日志上报
+ (void)uploadLog;

/// \brief 设置用户ID及用户名
/// \param userID 用户ID
/// \param userName 用户名
+ (bool)setUserID:(NSString *)userID userName:(NSString *)userName;

/// \brief 初始化SDK
/// \param[in] appID Zego派发的数字ID, 各个开发者的唯一标识
/// \param[in] appSignature Zego派发的签名, 用来校验对应appID的合法性
/// \return api 实例，nil 表示初始化失败
- (instancetype)initWithAppID:(unsigned int)appID appSignature:(NSData*)appSignature;

- (bool)setRoomDelegate:(id<ZegoRoomDelegate>) roomDelegate;

/// \brief 登陆房间
/// \param[in] roomID 房间唯一ID
/// \param[in] role 成员角色
/// \return true 成功，通过 blk 回调，false 失败
- (bool)loginRoom:(NSString *)roomID role:(int)role withCompletionBlock:(ZegoLoginCompletionBlock)blk;

/// \brief 退出房间
/// \note 会停止所有的推拉流
/// \return true 成功，false 失败
- (bool)logoutRoom;


/// \brief 直播事件通知回调
/// \param liveEventDelegate 直播事件通知回调协议
/// \return true 成功，false 失败
- (bool)setLiveEventDelegate:(id<ZegoLiveEventDelegate>)liveEventDelegate;


// * devie

/// \brief 音视频设备错误通知回调
/// \param deviceEventDelegate 音视频设备错误通知回调协议
/// \return true 成功，false 失败
- (bool)setDeviceEventDelegate:(id<ZegoDeviceEventDelegate>)deviceEventDelegate;

/// \brief 暂停模块
/// \param moduleType 模块类型，参考 ZegoAPIModuleType
- (void)pauseModule:(int)moduleType;

/// \brief 恢复模块
/// \param moduleType 模块类型，参考 ZegoAPIModuleType
- (void)resumeModule:(int)moduleType;

@end


@protocol ZegoRoomDelegate <NSObject>

@optional

/// \brief 因为登陆抢占原因等被挤出房间
/// \param[in] reason 原因
/// \param[in] roomID 房间 ID
- (void)onKickOut:(int)reason roomID:(NSString *)roomID;

/// \brief 与 server 断开
/// \param[in] errorCode 错误码，0 位无错误
/// \param[in] roomID 房间 ID
- (void)onDisconnect:(int)errorCode roomID:(NSString *)roomID;

/// \brief 流信息更新
/// \param[in] type 更新类型
/// \param[in] streamList 直播流列表
/// \param[in] roomID 房间 ID
- (void)onStreamUpdated:(int)type streams:(NSArray<ZegoStream*> *)streamList roomID:(NSString *)roomID;

/// \brief 流附加信息更新
/// \param[in] streamList 附加信息更新的流列表
/// \param[in] roomID 房间 ID
/// \note 主播推流成功后更新附加信息,在此回调中通知
- (void)onStreamExtraInfoUpdated:(NSArray<ZegoStream *> *)streamList roomID:(NSString *)roomID;

@end


typedef enum : NSUInteger {
    Play_BeginRetry = 1,
    Play_RetrySuccess = 2,
    
    Publish_BeginRetry = 3,
    Publish_RetrySuccess = 4,
} ZegoLiveEvent;


@protocol ZegoLiveEventDelegate <NSObject>

- (void)zego_onLiveEvent:(ZegoLiveEvent)event info:(NSDictionary<NSString*, NSString*>*)info;

@end

@protocol ZegoDeviceEventDelegate <NSObject>

- (void)zego_onDevice:(NSString *)deviceName error:(int)errorCode;

@end
