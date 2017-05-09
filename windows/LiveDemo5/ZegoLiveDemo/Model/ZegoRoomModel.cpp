#include "stdafx.h"
#include <algorithm>
#include "ZegoRoomModel.h"
#include "ZegoUtility.h"

CZegoRoomModel::CZegoRoomModel(std::string& roomID, std::string& roomName, std::string& anchorID, std::string& anchorName) 
    : m_strRoomID(roomID), m_strRoomName(roomName), m_strAnchorID(anchorID), m_strAnchorName(anchorName)
{ 
}

CZegoRoomModel::~CZegoRoomModel(void) 
{
    m_vStreamList.clear();
}


std::string CZegoRoomModel::GetRoomID(void)
{
	return m_strRoomID;
}

std::string CZegoRoomModel::GetRoomName(void)
{
	return m_strRoomName;
}

void CZegoRoomModel::SetRoomID(const std::string& roomID)
{
	m_strRoomID = roomID;
}

void CZegoRoomModel::SetRoomName(const std::string& roomName)
{
    m_strRoomName = roomName;
}

void CZegoRoomModel::SetAnchorID(const std::string& anchorID)
{
    m_strAnchorID = anchorID;
}

std::string CZegoRoomModel::GetAnchorID(void)
{
    return m_strAnchorID;
}

void CZegoRoomModel::SetAnchorName(const std::string& anchorName)
{
    m_strAnchorName = anchorName;
}

std::string CZegoRoomModel::GetAnchorName(void)
{
    return m_strAnchorName;
}

void CZegoRoomModel::SetCreatedTime(unsigned int time)
{
	m_uCreatedTime = time;
}

unsigned int CZegoRoomModel::GetCreatedTime(void)
{
	return m_uCreatedTime;
}

void CZegoRoomModel::SetLivesCount(unsigned int count)
{
	m_uLivesCount = count;
}

unsigned int CZegoRoomModel::GetLivesCount(void)
{
	return m_uLivesCount;
}

void CZegoRoomModel::AddStream(StreamPtr stream)
{
    if (stream == nullptr) { return; }

    std::string strStreamID = stream->GetStreamID();

    auto iter = std::find_if(m_vStreamList.begin(), m_vStreamList.end(),
        [&strStreamID](const StreamPtr& elem) { return elem->GetStreamID() == strStreamID; });

    if (iter == m_vStreamList.end())
    {
        m_vStreamList.push_back(stream);
    }
}

StreamPtr CZegoRoomModel::RemoveStream(const std::string& strStreamID)
{
    StreamPtr pStream(nullptr);

    auto iter = std::find_if(m_vStreamList.begin(), m_vStreamList.end(),
        [&strStreamID](const StreamPtr& elem) { return elem->GetStreamID() == strStreamID; });

    if (iter != m_vStreamList.end())
    {
        pStream = *iter;
        m_vStreamList.erase(iter);
    }

    return pStream;
}

int CZegoRoomModel::GetStreamCount(void)
{
    return m_vStreamList.size();
}

StreamPtr CZegoRoomModel::GetStreamByID(const std::string& streamID)
{

    auto iter = std::find_if(m_vStreamList.begin(), m_vStreamList.end(),
        [&streamID](const StreamPtr& elem) {return elem->GetStreamID() == streamID; });

    if (iter != m_vStreamList.end())
    {
        return *iter;
    }

    return nullptr;
}

std::vector<StreamPtr> CZegoRoomModel::GetStreamList(void)
{
    return m_vStreamList;
}