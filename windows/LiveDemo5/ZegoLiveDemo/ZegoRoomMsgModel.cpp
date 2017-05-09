#include "stdafx.h"
#include "ZegoRoomMsgModel.h"


CZegoRoomMsgModel::CZegoRoomMsgModel(const std::string& userID, const std::string& userName, const std::string& content, unsigned long long msgID, ROOM::ZegoMessageType type, ROOM::ZegoMessagePriority priority, ROOM::ZegoMessageCategory category)
{
    m_strUserID = userID;
    m_strUserName = userName;
    m_strContent = content;
    m_ullMsgID = msgID;
    m_nMsgType = type;
    m_nMsgPriority = priority;
    m_nMsgCategory = category;
}

CZegoRoomMsgModel::CZegoRoomMsgModel(const CZegoRoomMsgModel& otherRoom)
{
    m_strUserID = otherRoom.m_strUserID;
    m_strUserName = otherRoom.m_strUserName;
    m_strContent = otherRoom.m_strContent;
    m_ullMsgID = otherRoom.m_ullMsgID;
    m_nMsgType = otherRoom.m_nMsgType;
    m_nMsgPriority = otherRoom.m_nMsgPriority;
    m_nMsgCategory = otherRoom.m_nMsgCategory;
}

std::string CZegoRoomMsgModel::GetUserID(void)
{
    return m_strUserID;
}

std::string CZegoRoomMsgModel::GetUserName(void)
{
    return m_strUserName;
}

std::string CZegoRoomMsgModel::GetContent(void)
{
    return m_strContent;
}

unsigned long long CZegoRoomMsgModel::GetMsgID(void)
{
    return m_ullMsgID;
}

ROOM::ZegoMessageType CZegoRoomMsgModel::GetMsgType(void)
{
    return m_nMsgType;
}

ROOM::ZegoMessagePriority CZegoRoomMsgModel::GetMsgPriority(void)
{
    return m_nMsgPriority;
}

ROOM::ZegoMessageCategory CZegoRoomMsgModel::GetMsgCategory(void)
{
    return m_nMsgCategory;
}

CZegoRoomMsgModel::~CZegoRoomMsgModel()
{
}
