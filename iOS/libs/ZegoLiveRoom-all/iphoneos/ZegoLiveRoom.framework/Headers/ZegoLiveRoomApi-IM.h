//
//  ZegoLiveRoomApi.h
//  ZegoLiveRoom
//
//  Copyright © 2017年 zego. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ZegoLiveRoomApi.h"
#import "ZegoLiveRoomApiDefines-IM.h"


@protocol ZegoIMDelegate;

typedef void(^ZegoRoomMessageCompletion)(int errorCode, NSString *roomId, unsigned long long messageId);
typedef void(^ZegoCreateConversationCompletion)(int errorCode, NSString *roomId, NSString *conversationId);
typedef void(^ZegoConversationMessageCompletion)(int errorCode, NSString *roomId, NSString *conversationId, unsigned long long messageId);
typedef void(^ZegoConversationInfoBlock)(int errorCode, NSString *roomId, NSString *conversationId, ZegoConversationInfo *info);

@interface ZegoLiveRoomApi (IM)

/// \brief 设置IM回调
- (bool)setIMDelegate:(id<ZegoIMDelegate>)imDelegate;

/// \brief 房间发送广播消息
/// \param content 消息内容
/// \param type 消息类型，可以自定义
/// \param category 消息分类，可以自定义
/// \param priority 消息优先级
/// \param completionBlock 消息发送结果，回调server下发的messageId
/// \return true成功 false失败
- (bool)sendRoomMessage:(NSString *)content type:(ZegoMessageType)type category:(ZegoMessageCategory)category priority:(ZegoMessagePriority)priority completion:(ZegoRoomMessageCompletion)completionBlock;

/// \brief 在房间中创建一个会话
/// \param conversationName 会话名称
/// \param memberList 会话成员列表
/// \param completionBlock 创建结果，回调server下发的会话Id
/// \return true成功 false失败
- (bool)createConversation:(NSString *)conversationName memberList:(NSArray<ZegoUser *> *)memberList completion:(ZegoCreateConversationCompletion)completionBlock;

/// \brief 获取会话相关信息
/// \param conversationId 会话Id
/// \param completionBlock 获取结果，包括会话名称，会话成员，创建者等信息
/// \return true成功 false失败
- (bool)getConversationInfo:(NSString *)conversationId completion:(ZegoConversationInfoBlock)completionBlock;

/// \brief 在会话中发送一条消息
/// \param content 消息内容
/// \param type 消息类型，可以自定义
/// \param conversationId 会话Id
/// \param completionBlock 发送消息结果，回调server下发的messageId
/// \return true成功 false失败
- (bool)sendConversationMessage:(NSString *)content type:(ZegoMessageType)type conversationId:(NSString *)conversationId completion:(ZegoConversationMessageCompletion)completionBlock;

@end


@protocol ZegoIMDelegate <NSObject>

@optional
/// \brief 房间成员更新回调
/// \param userList 成员更新列表
/// \param type 更新类型(增量，全量)
- (void)onUserUpdate:(NSArray<ZegoUserState *> *)userList updateType:(ZegoUserUpdateType)type;

/// \brief 收到房间的广播消息
/// \param roomId 房间Id
/// \param messageList 消息列表，包括消息内容，消息分类，消息类型，发送者等信息
- (void)onRecvRoomMessage:(NSString *)roomId messageList:(NSArray<ZegoRoomMessage*> *)messageList;

/// \brief 收到会话消息
/// \param roomId 房间Id
/// \prarm conversationid 会话Id
/// \param message 会话消息，包括消息内容，消息类型，发送者，发送时间等信息
- (void)onRecvConversationMessage:(NSString *)roomId conversationId:(NSString *)conversationId message:(ZegoConversationMessage *)message;

@end
