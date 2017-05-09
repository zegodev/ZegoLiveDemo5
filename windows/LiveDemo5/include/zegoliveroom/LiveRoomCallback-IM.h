//
//  LiveRoomCallback-IM.h
//  zegoliveroom
//
//  Copyright © 2017年 Zego. All rights reserved.
//
//

#ifndef LiveRoomCallback_IM_h
#define LiveRoomCallback_IM_h

#include "./LiveRoomDefines-IM.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        class IIMCallback
        {
        public:
            virtual void OnUserUpdate(const ZegoUserInfo *pUserInfo, unsigned int userCount, ZegoUserUpdateType type) = 0;
            
            virtual void OnSendRoomMessage(int errorCode, const char *pszRoomID, int sendSeq, unsigned long long messageId) = 0;
            
            virtual void OnCreateConversation(int errorCode, const char *pszRoomID, int sendSeq, const char *pszConversationID) = 0;
            virtual void OnGetConversationInfo(int errorCode, const char *pszRoomID, const char *pszConverID, ROOM::ZegoConverInfo *pConverInfo, ROOM::ZegoUser *pMemberList, unsigned int memberCount) = 0;
            virtual void OnSendConversationMessage(int errorCode, const char *pszRoomID, const char *pszConverID, int sendSeq, unsigned long long messageId) = 0;
            
            //收到IM消息回调
            virtual void OnRecvRoomMessage(ROOM::ZegoRoomMessage *pMessageInfo, unsigned int messageCount, const char *pszRoomID) = 0;
            virtual void OnRecvConversationMessage(ROOM::ZegoConversationMessage *pMessageInfo, const char *pszConversationID, const char *pszRoomID) = 0;
            
            virtual ~IIMCallback() {}
        };
    }
}
#endif /* LiveRoomCallback_IM_h */
