#include "stdafx.h"
#include "ZegoStreamModel.h"

CZegoStreamModel::CZegoStreamModel(const std::string& streamID, const std::string& userID, const std::string& userName, const std::string& extraInfo, bool curUser):
	m_strStreamID(streamID), m_strUserID(userID), m_strUserName(userName), m_strExtraInfo(extraInfo), m_bCurUserCreated(curUser)
{
	m_nPlayViewId = -1;
}

CZegoStreamModel::CZegoStreamModel(const CZegoStreamModel& otherStream)
{
	m_strStreamID = otherStream.m_strStreamID;
	m_strUserID = otherStream.m_strUserID;
	m_strUserName = otherStream.m_strUserName;
    m_strExtraInfo = otherStream.m_strExtraInfo;
	m_nPlayViewId = otherStream.m_nPlayViewId;
	m_bCurUserCreated = otherStream.m_bCurUserCreated;
	m_bPrimary = otherStream.m_bPrimary;
}

std::string CZegoStreamModel::GetStreamID(void)
{
	return m_strStreamID;
}

std::string CZegoStreamModel::GetUserID(void)
{
	return m_strUserID;
}

std::string CZegoStreamModel::GetUserName(void)
{
	return m_strUserName;
}

std::string CZegoStreamModel::GetExtraInfo(void)
{
    return m_strExtraInfo;
}

void CZegoStreamModel::SetPlayView(int viewId)
{
	m_nPlayViewId = viewId;
}

int CZegoStreamModel::GetPlayView(void)
{
	return m_nPlayViewId;
}

bool CZegoStreamModel::IsPlaying(void)
{
	return m_nPlayViewId >= 0;
}

bool CZegoStreamModel::IsCurUserCreated(void)
{
	return m_bCurUserCreated;
}