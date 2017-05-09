//
//  LiveRoomDefines-IM.h
//  zegoliveroom
//
//  Copyright © 2017年 Zego. All rights reserved.
//
//

#ifndef LiveRoomDefines_IM_h
#define LiveRoomDefines_IM_h

#include "./LiveRoomDefines.h"

namespace ZEGO
{
    namespace LIVEROOM
    {
        enum ZegoUserUpdateType
        {
            UPDATE_TOTAL = 1,
            UPDATE_INCREASE,
        };
        
        enum ZegoUserUpdateFlag
        {
            USER_ADDED = 1,
            USER_DELETED,
        };
        
        struct ZegoUserInfo
        {
            ZegoUserInfo()
            {
                szUserId[0] = '\0';
                szUserName[0] = '\0';
            }
            
            char szUserId[ZEGO_MAX_COMMON_LEN];
            char szUserName[ZEGO_MAX_COMMON_LEN];
            ZegoUserUpdateFlag udapteFlag;
            ZegoRoomRole role;
        };
        
    }
    
#ifndef ZegoRoomDefines_h
    namespace ROOM
    {
        struct ZegoUser
        {
            ZegoUser()
            {
                szUserId[0] = '\0';
                szUserName[0] = '\0';
            }
            
            char szUserId[ZEGO_MAX_COMMON_LEN];
            char szUserName[ZEGO_MAX_COMMON_LEN];
        };
        
        enum ZegoMessageType
        {
            Text = 1,
            Picture,
            File,
            OtherType = 100,
        };
        
        enum ZegoMessagePriority
        {
            Default = 2,
            High = 3,
        };
        
        enum ZegoMessageCategory
        {
            Chat = 1,
            System,
            Like,
            Gift,
            OtherCategory = 100,
        };
        
        struct ZegoRoomMessage
        {
            ZegoRoomMessage()
            {
                szUserId[0] = '\0';
                szUserName[0] = '\0';
                szContent[0] = '\0';
                type = Text;
                priority = Default;
                category = Chat;
            }
            
            char szUserId[ZEGO_MAX_COMMON_LEN];
            char szUserName[ZEGO_MAX_COMMON_LEN];
            char szContent[ZEGO_MAX_COMMON_LEN];
            unsigned long long messageId;
            ZegoMessageType type;
            ZegoMessagePriority priority;
            ZegoMessageCategory category;
        };
        
        struct ZegoConversationMessage
        {
            ZegoConversationMessage()
            {
                szUserId[0] = '\0';
                szUserName[0] = '\0';
                szContent[0] = '\0';
                type = Text;
                sendTime = 0;
            }
            
            char szUserId[ZEGO_MAX_COMMON_LEN];
            char szUserName[ZEGO_MAX_COMMON_LEN];
            char szContent[ZEGO_MAX_COMMON_LEN];
            unsigned long long messageId;
            ZegoMessageType type;
            int sendTime;
        };
        
        struct ZegoConverInfo
        {
            char szConverName[ZEGO_MAX_COMMON_LEN];
            char szCreatorId[ZEGO_MAX_COMMON_LEN];
            int createTime;
        };
    }
#endif
}

#endif /* LiveRoomDefines_IM_h */
