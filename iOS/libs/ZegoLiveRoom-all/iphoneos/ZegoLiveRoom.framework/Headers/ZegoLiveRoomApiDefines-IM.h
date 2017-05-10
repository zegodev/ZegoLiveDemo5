//
//  ZegoLiveRoomApiDefines-IM.h
//  ZegoLiveRoom
//
//  Copyright © 2017年 zego. All rights reserved.
//

#ifndef ZegoLiveRoomApiDefines_IM_h
#define ZegoLiveRoomApiDefines_IM_h

typedef enum
{
    ZEGO_UPDATE_TOTAL = 1,
    ZEGO_UPDATE_INCREASE,
} ZegoUserUpdateType;

typedef enum
{
    ZEGO_USER_ADD = 1,
    ZEGO_USER_DELETE,
} ZegoUserUpateFlag;

typedef enum
{
    ZEGO_TEXT = 1,
    ZEGO_PICTURE,
    ZEGO_FILE,
    ZEGO_OTHER_TYPE = 100,
} ZegoMessageType;

typedef enum
{
    ZEGO_DEFAULT = 2,
    ZEGO_HIGH = 3,
} ZegoMessagePriority;

typedef enum
{
    ZEGO_CHAT = 1,
    ZEGO_SYSTEM,
    ZEGO_LIKE,
    ZEGO_GIFT,
    ZEGO_OTHER_CATEGORY = 100,
} ZegoMessageCategory;

@interface ZegoUserState : NSObject

@property (nonatomic, copy) NSString *userID;
@property (nonatomic, copy) NSString *userName;
@property (nonatomic, assign) ZegoUserUpateFlag updateFlag;
@property (nonatomic, assign) int role;

@end

@interface ZegoRoomMessage : NSObject

@property (nonatomic, copy) NSString *fromUserId;
@property (nonatomic, copy) NSString *fromUserName;
@property (nonatomic, assign) unsigned long long messageId;
@property (nonatomic, copy) NSString *content;
@property (nonatomic, assign) ZegoMessageType type;
@property (nonatomic, assign) ZegoMessagePriority priority;
@property (nonatomic, assign) ZegoMessageCategory category;

@end

@interface ZegoConversationMessage : NSObject

@property (nonatomic, copy) NSString *fromUserId;
@property (nonatomic, copy) NSString *fromUserName;
@property (nonatomic, assign) unsigned long long messageId;
@property (nonatomic, copy) NSString *content;
@property (nonatomic, assign) ZegoMessageType type;
@property (nonatomic, assign) unsigned int sendTime;

@end

@interface ZegoUser : NSObject 

@property (nonatomic, copy) NSString *userId;
@property (nonatomic, copy) NSString *userName;

@end

@interface ZegoConversationInfo : NSObject 

@property (nonatomic, copy) NSString *conversationName;
@property (nonatomic, copy) NSString *creatorId;
@property (nonatomic, assign) unsigned int createTime;
@property (nonatomic, strong) NSArray<ZegoUser*>* members;

@end

#endif /* ZegoLiveRoomApiDefines_h */
