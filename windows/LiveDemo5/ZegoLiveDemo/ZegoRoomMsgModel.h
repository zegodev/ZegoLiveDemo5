#pragma once

#include <string>
#include <vector>
#include <memory>
#include "LiveRoomDefines-IM.h"

using namespace ZEGO;

class CZegoRoomMsgModel
{
public:
    CZegoRoomMsgModel(const std::string& userID, const std::string& userName, const std::string& content, unsigned long long msgID, ROOM::ZegoMessageType msgType, ROOM::ZegoMessagePriority msgPriority, ROOM::ZegoMessageCategory msgCategory);
    CZegoRoomMsgModel(const CZegoRoomMsgModel& otherRoom);
    ~CZegoRoomMsgModel();

public:
    std::string GetUserID(void);
    std::string GetUserName(void);
    std::string GetContent(void);
    unsigned long long GetMsgID(void);
    ROOM::ZegoMessageType GetMsgType(void);
    ROOM::ZegoMessagePriority GetMsgPriority(void);
    ROOM::ZegoMessageCategory GetMsgCategory(void);

private:
    std::string m_strUserID;
    std::string m_strUserName;
    std::string m_strContent;
    unsigned long long m_ullMsgID;
    ROOM::ZegoMessageType m_nMsgType;
    ROOM::ZegoMessagePriority m_nMsgPriority;
    ROOM::ZegoMessageCategory m_nMsgCategory;
};

using RoomMsgPtr = std::shared_ptr<CZegoRoomMsgModel>;

