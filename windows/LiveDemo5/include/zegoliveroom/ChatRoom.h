//
//  ChatRoom.h
//  zegoliveroom
//
//  Created by Strong on 2017/2/21.
//
//

#ifndef ChatRoom_h
#define ChatRoom_h

#include "./ChatRoomCallback.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        /// \brief 设置聊天室相关信息通知的回调
        /// \param[in] pCB 回调对象指针
        /// \return true 成功，false失败
        ZEGO_API bool SetChatRoomCallback(IChatRoomCallback *pCB);
        
        /// \brief 登录聊天室
        /// \return true 成功，等待IChatRoomCallback::OnLoginChatRoom 回调，false 失败
        ZEGO_API bool LoginChatRoom();
        
        /// \brief 退出房间
        /// \return true成功，等待 IChatRoomCallback::OnLogoutRoom 回调，false 失败
        ZEGO_API bool LogoutChatRoom();
        
        /// \brief 发送广播消息
        /// \param[in] type 消息类型
        /// \param[in] category 消息分类
        /// \param[in] priority 消息优先级
        /// \param[in] messageContent 消息内容
        /// \return 消息发送seq
        ZEGO_API int SendBroadMessage(ROOM::ZegoMessageType type, ROOM::ZegoMessageCategory category, ROOM::ZegoMessagePriority priority, const char *messageContent);
        
        /// \brief 创建讨论组
        /// \param[in] groupName 讨论组名称
        /// \param[in] memberList 讨论组用户
        /// \return 消息发送seq
        ZEGO_API int CreateGroupChat(const char *groupName, ROOM::ZegoUser *memberList, unsigned int memberCount);
        
        /// \brief 获取讨论组信息
        /// \param[in] pszGroupId 讨论组id
        /// \return true 获取成功
        ZEGO_API bool GetGroupChatInfo(const char *pszGroupId);
        
        /// \brief 发送讨论组消息
        /// \param[in] pszGroupId 讨论组id
        /// \param[in] type 消息类型
        /// \param[in] messageContent 消息内容
        /// \return 消息发送seq
        ZEGO_API int SendGroupChatMessage(const char *pszGroupId, ROOM::ZegoMessageType type, const char *messageContent);
        
        /// \brief 邀请视频通话
        /// \param[in] memberList 请求视频通话成员
        /// \param[in] memberCount 视频通话成员个数
        /// \param[in] pszPrivateRoomId 视频通话房间Id
        /// \return 请求seq
        ZEGO_API int RequestVideoTalk(ROOM::ZegoUser *memberList, unsigned int memberCount, const char *pszVideoRoomId);
        
        /// \brief 取消视频通话
        /// \param[in] requestSeq 请求视频通话的seq
        /// return true 取消成功 false 失败
        ZEGO_API bool CancelVideoTalk(int requestSeq);
        
        /// \brief 回应视频通话
        /// \param[in] respondSeq 收到视频通话请求的seq
        /// \param[in] respondResult 是否同意视频通话
        /// \return true 成功 false 失败
        ZEGO_API bool RespondVideoTalk(int respondSeq, bool respondResult);
    }
}
#endif /* ChatRoom_h */
