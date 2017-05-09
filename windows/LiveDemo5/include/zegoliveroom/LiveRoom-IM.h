//
//  LiveRoom-IM.h
//  zegoliveroom
//
//  Copyright © 2017年 Zego. All rights reserved.
//
//

#ifndef LiveRoom_IM_h
#define LiveRoom_IM_h

#include "./LiveRoomDefines-IM.h"
#include "./LiveRoomCallback-IM.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        /// \brief 设置即时信息通知的回调
        /// \param[in] pCB 回调对象指针
        /// \return true 成功，false 失败
        ZEGO_API bool SetIMCallback(IIMCallback* pCB);
    
        //聊天室消息
        ZEGO_API int SendRoomMessage(ROOM::ZegoMessageType type, ROOM::ZegoMessageCategory category, ROOM::ZegoMessagePriority priority, const char *messageContent);
        //IM 私聊
        ZEGO_API int CreateConversation(const char *conversationName, ROOM::ZegoUser *memberList, unsigned int memberCount);
        ZEGO_API bool GetConversationInfo(const char *conversationId);
        ZEGO_API int SendConversationMessage(const char *conversationId, ROOM::ZegoMessageType type, const char *messageContent);
    }
}

#endif /* LiveRoom_IM_h */
