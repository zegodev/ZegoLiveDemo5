//
//  ZegoChatRoomApi.h
//  ZegoLiveRoom
//
//  Created by Strong on 2017/2/22.
//  Copyright © 2017年 zego. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ZegoLiveRoomApi.h"
#import "ZegoLiveRoomApiDefines-IM.h"

@protocol ZegoChatRoomDelegate;

typedef void(^ZegoLoginChatRoomBlock)(int errorCode);
typedef void(^ZegoChatRoomMessageCompletion)(int errorCode, unsigned long long messageId);
typedef void(^ZegoCreateGroupChatCompletion)(int errorCode, NSString *groupId);
typedef void(^ZegoGroupChatMessageCompletion)(int errorCode, NSString *groupId, unsigned long long messageId);
typedef void(^ZegoGroupChatInfoBlock)(int errorCode, NSString *groupId, ZegoConversationInfo *info);

typedef void(^ZegoVideoTalkCompletion)(int errorCode);

@interface ZegoLiveRoomApi (ChatRoom)

- (bool)setChatRoomDelegate:(id<ZegoChatRoomDelegate>) chatRoomDelegate;

/// \brief 登录聊天室
/// \return true成功，通过block回调
- (bool)loginChatRoomWithCompletion:(ZegoLoginChatRoomBlock)block;

/// \brief 退出聊天室
/// \return true成功，false失败
- (bool)logoutChatRoom;

/// \brief 聊天室发送广播消息
/// \param content 消息内容
/// \param type 消息类型，可以自定义
/// \param category 消息分类，可以自定义
/// \param priority 消息优先级
/// \param completionBlock 消息发送结果，回调server下发的messageId
/// \return true成功 false失败
- (bool)sendBroadMessage:(NSString *)content type:(ZegoMessageType)type category:(ZegoMessageCategory)category priority:(ZegoMessagePriority)priority completion:(ZegoChatRoomMessageCompletion)completionBlock;

/// \brief 创建一个讨论组(可以为两人）
/// \param groupName 讨论组名称
/// \param memberList 成员列表
/// \param completionBlock 创建结果，回调server下发的会话Id
/// \return true成功，false失败
- (bool)createGroupChat:(NSString *)groupName memberList:(NSArray<ZegoUser *> *)memberList completion:(ZegoCreateGroupChatCompletion)completionBlock;

/// \brief 获取讨论组相关信息
/// \param groupId 讨论组Id
/// \param completionBlock 获取结果，包括会话名称，会话成员，创建者等信息
/// \return true成功 false失败
- (bool)getGroupChatInfo:(NSString *)groupId completion:(ZegoGroupChatInfoBlock)completionBlock;

/// \brief 在讨论组中发送一条信息
/// \param content 消息内容
/// \param type 消息类型，可以自定义
/// \param groupId 会话Id
/// \param completionBlock 发送消息结果，回调server下发的messageId
/// \return true成功 false失败
- (bool)sendGroupChatMessage:(NSString *)content type:(ZegoMessageType)type groupId:(NSString *)groupId completion:(ZegoGroupChatMessageCompletion)completionBlock;

/// \brief 邀请视频通话
/// \param userList 邀请用户列表,用户必须在线
/// \param videoRoomId 通知用户要进入的视频通话房间Id
/// \param completionBlock 信令发送结果
/// \return 请求seq，正值为有效
- (int)requestVideoTalk:(NSArray<ZegoUser *> *)userList videoRoomId:(NSString *)videoRoomId completion:(ZegoVideoTalkCompletion)completionBlock;

/// \brief 取消视频通话
/// \param requestSeq requestVideoTalk返回的seq
/// \param completionBlock 信令发送结果
/// \return true 成功，false失败
- (bool)cancelVideoTalk:(int)requestSeq completion:(ZegoVideoTalkCompletion)completionBlock;

/// \brief 收到邀请视频通话后的回应
/// \param respondSeq 回调onRecvRequestVideoTalk返回的respondSeq
/// \param result 回应同意/拒绝视频通话
/// \param completionBlock 信令发送结果
/// \return true 成功，false 失败
- (bool)respondVideoTalk:(int)respondSeq respondResult:(bool)result completion:(ZegoVideoTalkCompletion)completionBlock;

@end

typedef NS_ENUM(NSInteger, ZegoChatRoomConnectState)
{
    Disconnected = 1,
    WaitingConnect,         //网络切换时，网络状态为WaitingConnect
                            //网络恢复时，会自动重连
    Connecting,
    Connected,
};

@protocol ZegoChatRoomDelegate <NSObject>

@optional

/// \brief 因为登录抢占原因等被挤出聊天室
/// \param reason 原因
- (void)onKickOutChatRoom:(int)reason;

/// \brief 网络连接状态
/// \param state 网络状态
- (void)onConnectState:(ZegoChatRoomConnectState)state;

/// \brief 聊天室成员更新回调
/// \param userList 成员更新列表
/// \param type 更新类型(增量，全量)
- (void)onChatRoomUserUpdate:(NSArray<ZegoUserState *> *)userList updateType:(ZegoUserUpdateType)type;

/// \brief 收到聊天室的广播消息
/// \param messageList 消息列表，包括消息内容，消息分类，消息类型，发送者等信息
- (void)onRecvBroadMessage:(NSArray<ZegoRoomMessage*> *)messageList;

/// \brief 收到讨论组消息
/// \param groupId 讨论组Id
/// \param message 会话消息，包括消息内容，消息类型，发送者，发送时间等信息
- (void)onRecvGroupChatMessage:(NSString *)groupId message:(ZegoConversationMessage *)message;

/// \brief 收到邀请视频通话的请求
/// \param respondSeq 请求的回应seq
/// \param fromUserId 请求者的UserId
/// \param fromUserName 请求者的UserName
/// \param videoRoomId 请求者要求进入的视频房间
- (void)onRecvRequestVideoTalk:(int)respondSeq fromUserId:(NSString *)fromUserId fromUserName:(NSString *)fromUserName videoRoomId:(NSString *)videoRoomId;

/// \brief 收到取消视频通话的请求
/// \param respondSeq onRecvRequestVideoTalk返回的respondSeq
/// \param fromUserId 请求者的UserId
/// \param fromUserName 请求者的UserName
- (void)onRecvCancelVideoTalk:(int)respondSeq fromUserId:(NSString *)fromUserId fromUserName:(NSString *)fromUserName;

/// \brief 收到视频通话请求的回应
/// \param requestSeq requestVideoTalk返回的seq
/// \param fromUserId 请求者的UserId
/// \param fromUserName 请求者的UserName
/// \param result 回应的内容(同意/拒绝)
- (void)onRecvRespondVideoTalk:(int)requestSeq fromUserId:(NSString *)fromUserId fromUserName:(NSString *)fromUserName respondResult:(bool)result;

@end
