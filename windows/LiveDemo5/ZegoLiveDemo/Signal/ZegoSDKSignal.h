#pragma once

#include "sigslot.h"
#include "ZegoSigslotDefine.h"
#include "ZegoRoomModel.h"
#include "LiveRoomCallback.h"
#include "ZegoRoomMsgModel.h"
#include "LiveRoomCallback-IM.h"
#include "LiveRoomCallback-Player.h"
#include "LiveRoomCallback-Publisher.h"

using namespace ZEGO;

class CZegoAVSignal : public LIVEROOM::IRoomCallback, public LIVEROOM::ILivePublisherCallback, public LIVEROOM::ILivePlayerCallback, public LIVEROOM::IIMCallback, public AV::IZegoDeviceStateCallback
{
public:
    CZegoAVSignal();
    ~CZegoAVSignal();

    ZEGO_MAKE_SIGNAL(LoginRoom, 3, int, std::string, std::vector<StreamPtr>);
    ZEGO_MAKE_SIGNAL(LogoutRoom, 2, int, std::string);
    ZEGO_MAKE_SIGNAL(Disconnect, 2, int, std::string);
    ZEGO_MAKE_SIGNAL(KickOut, 2, int, std::string);
    ZEGO_MAKE_SIGNAL(SendRoomMessage, 4, int, std::string, int, unsigned long long);
    ZEGO_MAKE_SIGNAL(RecvRoomMesge, 2, std::string, std::vector<RoomMsgPtr>);
    ZEGO_MAKE_SIGNAL(StreamUpdated, 3, std::string, std::vector<StreamPtr>, LIVEROOM::ZegoStreamUpdateType);
    ZEGO_MAKE_SIGNAL(PublishStateUpdate, 3, int, std::string, StreamPtr);
    ZEGO_MAKE_SIGNAL(PlayStateUpdate, 2, int, std::string);
    ZEGO_MAKE_SIGNAL(PublishQulityUpdate, 4, std::string, int, double, double);
    ZEGO_MAKE_SIGNAL(PlayQualityUpdate, 4, std::string, int, double, double);
    ZEGO_MAKE_SIGNAL(AuxInput, 4, unsigned char*, int*, int*, int*);
    ZEGO_MAKE_SIGNAL(JoinLiveRequest, 4, int, std::string, std::string, std::string);
    ZEGO_MAKE_SIGNAL(JoinLiveResponse, 4, int, std::string, std::string, int);
    ZEGO_MAKE_SIGNAL(AudioDeviceChanged, 4, AV::AudioDeviceType, std::string, std::string, AV::DeviceState);
    ZEGO_MAKE_SIGNAL(VideoDeviceChanged, 3, std::string, std::string, AV::DeviceState);
protected:

    void OnLoginRoom(int errorCode, const char *pszRoomID, const LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount);
    void OnLogoutRoom(int errorCode, const char *pszRoomID);
    void OnKickOut(int reason, const char *pszRoomID);
    void OnDisconnect(int errorCode, const char *pszRoomID);
    void OnStreamUpdated(LIVEROOM::ZegoStreamUpdateType type, LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID);
    void OnStreamExtraInfoUpdated(LIVEROOM::ZegoStreamInfo *pStreamInfo, unsigned int streamCount, const char *pszRoomID) {}
    void OnCustomCommand(int errorCode, int requestSeq, const char *pszRoomID) {}
    void OnRecvCustomCommand(const char *pszUserId, const char *pszUserName, const char *pszContent, const char *pszRoomID) {}

    void OnPublishStateUpdate(int stateCode, const char* pszStreamID, const LIVEROOM::ZegoPublishingStreamInfo& oStreamInfo);
    void OnJoinLiveRequest(int seq, const char *pszFromUserId, const char *pszFromUserName, const char *pszRoomID);
    void OnInviteJoinLiveResponse(int result, const char *pszFromUserId, const char *pszFromUserName, int seq) {}
    void OnPublishQulityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS);
    void OnCaptureVideoSizeChanged(int nWidth, int nHeight) {}
    void OnPreviewSnapshot(void *pImage) {}
    void OnAuxCallback(unsigned char *pData, int *pDataLen, int *pSampleRate, int *pNumChannels);
    void OnUpdateMixStreamConfig(unsigned int uiErrorCode, const char* pszMixStreamID, const LIVEROOM::ZegoPublishingStreamInfo& oStreamInfo) {}

    void OnPlayStateUpdate(int stateCode, const char* pszStreamID);
    void OnPlayQualityUpdate(const char* pszStreamID, int quality, double videoFPS, double videoKBS);
    void OnJoinLiveResponse(int result, const char* pszFromUserId, const char* pszFromUserName, int seq);
    void OnInviteJoinLiveRequest(int seq, const char* pszFromUserId, const char* pszFromUserName, const char* pszRoomID) {};
    void OnVideoSizeChanged(const char* pStreamID, int nWidth, int nHeight) {};
    void OnSnapshot(void *pImage, const char* pszStreamID) {};

    void OnUserUpdate(const LIVEROOM::ZegoUserInfo *pUserInfo, unsigned int userCount, LIVEROOM::ZegoUserUpdateType type) {};
    void OnSendRoomMessage(int errorCode, const char *pszRoomID, int sendSeq, unsigned long long messageId);
    void OnCreateConversation(int errorCode, const char *pszRoomID, int sendSeq, const char *pszConversationID) {};
    void OnGetConversationInfo(int errorCode, const char *pszRoomID, const char *pszConverID, ROOM::ZegoConverInfo *pConverInfo, ROOM::ZegoUser *pMemberList, unsigned int memberCount) {};
    void OnSendConversationMessage(int errorCode, const char *pszRoomID, const char *pszConverID, int sendSeq, unsigned long long messageId) {};
    void OnRecvRoomMessage(ROOM::ZegoRoomMessage *pMessageInfo, unsigned int messageCount, const char *pszRoomID);
    void OnRecvConversationMessage(ROOM::ZegoConversationMessage *pMessageInfo, const char *pszConversationID, const char *pszRoomID) {};


    void OnAudioDeviceStateChanged(AV::AudioDeviceType deviceType, AV::DeviceInfo *deviceInfo, AV::DeviceState state);
    void OnVideoDeviceStateChanged(AV::DeviceInfo *deviceInfo, AV::DeviceState state);
    void OnDeviceError(const char* deviceName, int errorCode) {};

private:
	HWND m_hCommWnd;
};
