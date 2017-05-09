//
//  ChatRoomCallback.h
//  zegoliveroom
//
//  Created by Strong on 2017/2/21.
//
//

#ifndef ChatRoomCallback_h
#define ChatRoomCallback_h

#include "./LiveRoomDefines.h"
#include "./LiveRoomDefines-IM.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        class IChatRoomCallback
        {
        public:
            /// \brief 登录聊天室成功回调
            /// \param[in] errorCode 错误码， 0为无错误
            virtual void OnLoginChatRoom(int errorCode) = 0;
            
            /// \brief 退出聊天室
            /// \param[in] errorCode 错误码
            virtual void OnLogoutChatRoom(int errorCode) {}
            
            /// \brief 因为登录抢占原因等被挤出房间
            /// \param[in] reason原因
            virtual void OnKickOutChatRoom(int reason) = 0;
            
            /// \brief 当前网络连接状态
            /// \param[in] state 当前网络连接状态
            /// \param[in] errorCode 错误码
            virtual void OnConnectState(int state, int errorCode) = 0;
            
            /// \brief 发送广播消息结果
            /// \param[in] errorCode 错误码
            /// \param[in] sendSeq 发送seq
            /// \param[in] messageId server返回的消息id
            virtual void OnBroadMessageResult(int errorCode, int sendSeq, unsigned long long messageId) = 0;
            
            /// \brief 收到广播消息
            /// \params[in] pMessage 第一条消息指针
            /// \params[in] messageCount 消息条数
            virtual void OnRecvBroadMessage(ROOM::ZegoRoomMessage *pMessage, unsigned int messageCount) = 0;
            
            /// \brief 创建讨论组结果
            /// \params[in] errorCode 错误码
            /// \params[in] sendSeq 发送seq
            /// \params[in] pszGroupId 讨论组id
            virtual void OnCreateGroupChat(int errorCode, int sendSeq, const char *pszGroupId) = 0;
            
            /// \brief 获取讨论组信息
            /// \params[in] errorCode 错误码
            /// \params[in] pszGroupId 讨论组id
            /// \params[in] pGroupInfo 讨论组信息（名称，创建时间，创建人...)
            /// \params[in] pMemberList 讨论组成员
            /// \params[in] memberCount 讨论组成员个数
            virtual void OnGetGropuChatInfo(int errorCode, const char *pszGroupId, ROOM::ZegoConverInfo *pGroupInfo, ROOM::ZegoUser *pMemberList, unsigned int memberCount) = 0;
            
            /// \brief 讨论组消息发送结果
            /// \params[in] errorCode 错误码
            /// \params[in] pszGroupId 讨论组id
            /// \params[in] sendSeq 发送seq
            /// \params[in] messageId 消息id
            virtual void OnGroupMessageResult(int errorCode, const char *pszGroupId, int sendSeq, unsigned long long messageId) = 0;
            
            /// \brief 收到讨论组消息
            /// \params[in] pMessage 讨论组消息信息
            /// \params[in] pszGroupId 讨论组id
            virtual void OnRecvGroupMessage(ROOM::ZegoConversationMessage *pMessage, const char *pszGroupId) = 0;
            
            /// \brief 发送RequestVideoTalk结果
            /// \params[in] errorCode 错误码
            /// \params[in] requestSeq 请求seq
            virtual void OnRequestVideoTalkResult(int errorCode, int requestSeq) = 0;
            
            /// \brief 收到RequestVideoTalk消息
            /// \params[in] respondSeq 消息回应seq
            /// \params[in] pszUserId 发送者UserId
            /// \params[in] pszUserName 发送者UserName
            /// \params[in] pszContent 收到的附加消息内容
            virtual void OnRecvRequestVideoTalk(int respondSeq, const char *pszUserId, const char *pszUserName, const char *pszContent) = 0;
            
            /// \brief 发送CancelVideoTalk结果
            /// \params[in] errorCode 错误码
            /// \params[in] requestSeq 请求seq
            virtual void OnCancelVideoTalkResult(int errorCode, int requestSeq) = 0;
            
            /// \brief 收到CancelVideoTalk消息
            /// \params[in] respondSeq 消息回应seq
            /// \params[in] pszUserId 发送者UserId
            /// \params[in] pszUserName 发送者UserName
            virtual void OnRecvCancelVideoTalk(int respondSeq, const char *pszUserId, const char *pszUserName) = 0;
            
            /// \brief 发送RespondVideoTalk结果
            /// \params[in] errorCode 错误码
            /// \params[in] requestSeq 请求seq
            virtual void OnRespondVideoTalkResult(int errorCode, int requestSeq) = 0;
            
            /// \brief 收到RespondVideoTalk消息
            /// \params[in] respondSeq 消息回应seq
            /// \params[in] pszUserId 发送者UserId
            /// \params[in] pszUserName 发送者UserName
            /// \params[in] result 是否同意
            virtual void OnRecvRespondVideoTalk(int respondSeq, const char *pszUserId, const char *pszUserName, bool result) = 0;
            
            /// \brief 收到当前在线用户变化的通知
            /// \params[in] pUserInfo 用户信息
            /// \params[in] userCount 用户个数
            /// \params[in] type 更新类型
            virtual void OnChatRoomUserUpdate(const ZegoUserInfo *pUserInfo, unsigned int userCount, ZegoUserUpdateType type) = 0;
        };
    }
}

#endif /* ChatRoomCallback_h */
