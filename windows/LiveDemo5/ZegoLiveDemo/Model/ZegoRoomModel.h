#pragma once

#include <vector>
#include <deque>
#include <memory>
#include "ZegoUserModel.h"
#include "ZegoStreamModel.h"

class CZegoRoomModel
{
public:
	CZegoRoomModel() { m_strRoomID = nullptr; m_strRoomName = nullptr; }
	CZegoRoomModel(std::string& roomID, std::string& roomName, std::string& anchorID, std::string& anchorName);
	~CZegoRoomModel();

public:
    void SetRoomID(const std::string& roomID);
    std::string GetRoomID(void);
	void SetRoomName(const std::string& roomName);
    std::string GetRoomName(void);

    void SetAnchorID(const std::string& anchorID);
    std::string GetAnchorID(void);
    void SetAnchorName(const std::string& anchorName);
    std::string GetAnchorName(void);

	void SetCreatedTime(unsigned int time);
	unsigned int GetCreatedTime(void);
	void SetLivesCount(unsigned int count);
	unsigned int GetLivesCount(void);

    void AddStream(StreamPtr stream);
    StreamPtr RemoveStream(const std::string& streamID);
    int GetStreamCount(void);
    StreamPtr GetStreamByID(const std::string& streamID);
    std::vector<StreamPtr> GetStreamList(void);
private:
    std::string m_strRoomID;
	std::string m_strRoomName;
    std::string m_strAnchorID;
    std::string m_strAnchorName;

	std::vector<StreamPtr> m_vStreamList;

	unsigned int m_uCreatedTime;
	unsigned int m_uLivesCount;
};

using RoomPtr = std::shared_ptr<CZegoRoomModel>;