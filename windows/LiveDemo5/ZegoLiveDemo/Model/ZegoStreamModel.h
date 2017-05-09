#pragma once

#include <string>
#include <vector>
#include <memory>

class CZegoStreamModel
{
public:
	CZegoStreamModel(const std::string& streamID, const std::string& userID, const std::string& userName, const std::string& extraInfo, bool curUser = false);
	CZegoStreamModel(const CZegoStreamModel& otherStream);

public:
	std::string GetStreamID(void);
	std::string GetUserID(void);
	std::string GetUserName(void);
    std::string GetExtraInfo(void);

	void SetPlayView(int viewId);
	int GetPlayView(void);
	bool IsPlaying(void);

	bool IsCurUserCreated(void);

	std::vector<std::string> m_vecRtmpUrls;
	std::vector<std::string> m_vecFlvUrls;
	std::vector<std::string> m_vecHlsUrls;

private:
	std::string m_strStreamID;
    std::string m_strUserID;
	std::string m_strUserName;
    std::string m_strExtraInfo;

	int m_nPlayViewId;
	bool m_bCurUserCreated;
	bool m_bPrimary;
};

using StreamPtr = std::shared_ptr<CZegoStreamModel>;