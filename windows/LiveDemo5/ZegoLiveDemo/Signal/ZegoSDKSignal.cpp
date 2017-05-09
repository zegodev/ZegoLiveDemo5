#include "stdafx.h"
#include "ZegoSDKSignal.h"
#include "ZegoUtility.h"

CZegoAVSignal::CZegoAVSignal()
{
	m_hCommWnd = ::FindWindow(ZegoCommWndClassName, ZegoCommWndName);
}

CZegoAVSignal::~CZegoAVSignal()
{
}

void CZegoAVSignal::OnLoginRoom(int errorCode, const char *pszRoomID, const LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount)
{
    std::string strRoomID = pszRoomID ? pszRoomID : "";

    std::vector<StreamPtr> vStreamList;
    for (int i = 0; i < streamCount; i++)
    {
        LIVEROOM::ZegoStreamInfo zegoStreamInfo = pStreamInfo[i];
        StreamPtr pStream(new CZegoStreamModel(zegoStreamInfo.szStreamId, zegoStreamInfo.szUserId, zegoStreamInfo.szUserName, zegoStreamInfo.szExtraInfo));
        vStreamList.push_back(pStream);
    }

    ZEGO_SWITCH_THREAD_PRE
        m_sigLoginRoom(errorCode, strRoomID, vStreamList);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnLogoutRoom(int errorCode, const char *pszRoomID)
{
    std::string strRoomID = pszRoomID ? pszRoomID : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigLogoutRoom(errorCode, strRoomID);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnDisconnect(int errorCode, const char *pszRoomID)
{
    std::string strRoomID = pszRoomID ? pszRoomID : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigDisconnect(errorCode, strRoomID);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnKickOut(int reason, const char *pszRoomID)
{
    std::string strRoomID = pszRoomID ? pszRoomID : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigKickOut(reason, strRoomID);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnSendRoomMessage(int errorCode, const char *pszRoomID, int sendSeq, unsigned long long messageId)
{
    std::string strRoomID = pszRoomID ? pszRoomID : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigSendRoomMessage(errorCode, strRoomID, sendSeq, messageId);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnRecvRoomMessage(ROOM::ZegoRoomMessage *pMessageInfo, unsigned int messageCount, const char *pszRoomID)
{
    if (pMessageInfo == nullptr || messageCount == 0)
    {
        return;
    }

    std::string strRoomID = pszRoomID ? pszRoomID : "";

    std::vector<RoomMsgPtr> vRoomMsgList;
    for (int i = 0; i < messageCount; i++)
    {
        ROOM::ZegoRoomMessage zegoRoomMessage = pMessageInfo[i];
        RoomMsgPtr pRoomMsg(new CZegoRoomMsgModel(zegoRoomMessage.szUserId, zegoRoomMessage.szUserName, 
            zegoRoomMessage.szContent, zegoRoomMessage.messageId, zegoRoomMessage.type, zegoRoomMessage.priority, zegoRoomMessage.category));
        vRoomMsgList.push_back(pRoomMsg);
    }

    ZEGO_SWITCH_THREAD_PRE
        m_sigRecvRoomMesge(strRoomID, vRoomMsgList);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnStreamUpdated(LIVEROOM::ZegoStreamUpdateType type, LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID)
{
    if (pStreamInfo == nullptr || streamCount == 0)
    {
        return;
    }

    std::string strRoomID = pszRoomID ? pszRoomID: "";

    std::vector<StreamPtr> vStreamList;
    for (int i = 0; i < streamCount; i++)
    {
        LIVEROOM::ZegoStreamInfo zegoStreamInfo = pStreamInfo[i];
        StreamPtr pStream(new CZegoStreamModel(zegoStreamInfo.szStreamId, zegoStreamInfo.szUserId, zegoStreamInfo.szUserName, zegoStreamInfo.szExtraInfo));
        vStreamList.push_back(pStream);
    }

    ZEGO_SWITCH_THREAD_PRE
        m_sigStreamUpdated(strRoomID, vStreamList, type);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnPublishStateUpdate(int stateCode, const char* pszStreamID, const LIVEROOM::ZegoPublishingStreamInfo& oStreamInfo)
{
    std::string strStreamID = pszStreamID ? pszStreamID : "";

    StreamPtr pStream(new CZegoStreamModel(strStreamID, "", "", "", true));

    for (unsigned int i = 0; i < oStreamInfo.uiRtmpURLCount; i++)
    {
        pStream->m_vecRtmpUrls.push_back(oStreamInfo.arrRtmpURLs[i]);
    }

    for (unsigned int i = 0; i < oStreamInfo.uiFlvURLCount; i++)
    {
        pStream->m_vecFlvUrls.push_back(oStreamInfo.arrFlvURLs[i]);
    }

    for (unsigned int i = 0; i < oStreamInfo.uiHlsURLCount; i++)
    {
        pStream->m_vecHlsUrls.push_back(oStreamInfo.arrHlsURLs[i]);
    }

    ZEGO_SWITCH_THREAD_PRE
        m_sigPublishStateUpdate(stateCode, strStreamID, pStream);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnPlayStateUpdate(int stateCode, const char* pszStreamID)
{
    std::string strStreamID = pszStreamID ? pszStreamID : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigPlayStateUpdate(stateCode, strStreamID);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnPublishQulityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS)
{
    std::string strStreamID = pszStreamID ? pszStreamID : "";
    
    ZEGO_SWITCH_THREAD_PRE
        m_sigPublishQulityUpdate(strStreamID, quality, videoFPS, videoKBS);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnPlayQualityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS)
{
    std::string strStreamID = pszStreamID ? pszStreamID : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigPlayQualityUpdate(strStreamID, quality, videoFPS, videoKBS);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnAuxCallback(unsigned char *pData, int *pDataLen, int *pSampleRate, int *pNumChannels)
{
    m_sigAuxInput(pData, pDataLen, pSampleRate, pNumChannels);
}

void CZegoAVSignal::OnJoinLiveRequest(int seq, const char *pszFromUserId, const char *pszFromUserName, const char *pszRoomID) 
{
    std::string strFromUserID = pszFromUserId ? pszFromUserId : "";
    std::string strFromUserName = pszFromUserName ? pszFromUserName : "";
    std::string strRoomID = pszRoomID ? pszRoomID : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigJoinLiveRequest(seq, strFromUserID, strFromUserName, strRoomID);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnJoinLiveResponse(int result, const char* pszFromUserId, const char* pszFromUserName, int seq)
{
    std::string strFromUserID = pszFromUserId ? pszFromUserId : "";
    std::string strFromUserName = pszFromUserName ? pszFromUserName : "";

    ZEGO_SWITCH_THREAD_PRE
        m_sigJoinLiveResponse(result, strFromUserID, strFromUserName, seq);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnAudioDeviceStateChanged(AV::AudioDeviceType deviceType, AV::DeviceInfo *deviceInfo, AV::DeviceState state) 
{
    if (deviceInfo == nullptr)
        return;

    std::string strDeviceId = deviceInfo->szDeviceId;
    std::string strDeviceName = deviceInfo->szDeviceName;

    ZEGO_SWITCH_THREAD_PRE
        m_sigAudioDeviceChanged(deviceType, strDeviceId, strDeviceName, state);
    ZEGO_SWITCH_THREAD_ING
}

void CZegoAVSignal::OnVideoDeviceStateChanged(AV::DeviceInfo *deviceInfo, AV::DeviceState state)
{
    if (deviceInfo == nullptr)
        return;

    std::string strDeviceId = deviceInfo->szDeviceId;
    std::string strDeviceName = deviceInfo->szDeviceName;

    ZEGO_SWITCH_THREAD_PRE
        m_sigVideoDeviceChanged(strDeviceId, strDeviceName, state);
    ZEGO_SWITCH_THREAD_ING
}
