#pragma once
#include "ZegoSettingsModel.h"
#include "ZegoAVView.h"
#include "ZegoRoomModel.h"
#include "LiveRoom.h"
#include "LiveRoom-IM.h"
#include "LiveRoom-Publisher.h"
#include "LiveRoom-Player.h"
#include "LiveRoomDefines.h"
#include "LiveRoomDefines-IM.h"
#include "ZegoRoomMsgModel.h"

using namespace ZEGO;

#include <string>
#include <stack>
#include "CGridListCtrlX/CGridListCtrlEx.h"
#include "afxwin.h"
#include "afxcmn.h"

class CAboutDlg : public CDialogEx, public sigslot::has_slots<>
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

														// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

// CZegoRoomDlg 对话框

class CZegoRoomDlg : public CDialogEx, public sigslot::has_slots<>
{
	DECLARE_DYNAMIC(CZegoRoomDlg)

public:
	CZegoRoomDlg(SettingsPtr curSettings, RoomPtr room, std::string curUserID, std::string curUserName, bool isAnchor, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CZegoRoomDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ROOM };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	void OnGetMinMaxInfo(MINMAXINFO *lpMMI);
	void OnClose();
	void OnSize(UINT nType, int cx, int cy);
	void OnSysCommand(UINT nID, LPARAM lParam);
    void OnTimer(UINT_PTR id);
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButtonBack();
	afx_msg void OnBnClickedCheckMicrophone();
	afx_msg void OnBnClickedButtonAux();
	afx_msg void OnIdok();
    afx_msg void OnStartPublish();
    afx_msg void OnBnClickedButtonCapture();
    afx_msg void OnLbnSelchangeListboxChat();
    afx_msg void OnCbnSelchangeComboCurvideo();
    afx_msg void OnCbnSelchangeComboCuraudio();

    void OnLoginRoom(int errorCode, std::string strRoomID, std::vector<StreamPtr> vStreamList);
    void OnStreamUpdated(std::string strRoomID, std::vector<StreamPtr> vStreamList, LIVEROOM::ZegoStreamUpdateType tyep);
    void OnPublishStateUpdate(int stateCode, std::string streamID, StreamPtr stream);
    void OnPlayStateUpdate(int stateCode, std::string streamID);
    void OnDisconnect(int errorCode, std::string roomID);
    void OnKickOut(int reason, std::string roomID);
    void OnPlayQualityUpdate(std::string streamID, int quality, double videoFPS, double videoKBS);
    void OnPublishQulityUpdate(std::string streamID, int quality, double videoFPS, double videoKBS);
    void OnAVAuxInput(unsigned char *pData, int *pDataLen, int *pSampleRate, int *pNumChannels);
    void OnSendRoomMessage(int errorCode, std::string pszRoomID, int sendSeq, unsigned long long messageId);
    void OnRecvRoomMessage(std::string roomID, std::vector<RoomMsgPtr> vRoomMsgList);
    void OnJoinLiveRequest(int seq, std::string fromUserID, std::string fromUserName, std::string roomID);
    void OnJoinLiveResponse(int result, std::string fromUserID, std::string fromUserName, int seq);
    void OnAudioDeviceChanged(AV::AudioDeviceType deviceType, std::string strDeviceId, std::string strDeviceName, AV::DeviceState state);
    void OnVideoDeviceChanged(std::string strDeviceId, std::string strDeviceName, AV::DeviceState state);

private:
	void BeginAux(void);
	void EndAux(void);
	void FreeAVView(StreamPtr stream);
    void StartPublishStream();
    void StopPublishStream(const std::string& streamID);
    void StartPlayStream(StreamPtr stream);
    void StopPlayStream(const std::string& streamID);
    void SetPublishText();
    void EnumVideoAndAudioDevice();
    void GetOut();

private:
	HICON m_hIcon;

	CEdit m_edMessage;
	CListBox m_lbChatContent;
	std::string m_strLastSendMsg;
	CEdit m_edStreamUrl;
	BOOL m_bCKEnableMic;
	CButton m_btnAux;
    CComboBox m_cbCurrentAudio;
    CComboBox m_cbCurrentVideo;
    CButton m_btnCapture;
    CButton m_btnStartPublish;

	CZegoAVView m_primaryAVView;
	std::stack<unsigned int> m_avaliableView;
	CZegoAVView m_student1AVView;
	CZegoAVView m_student2AVView;
	CZegoAVView m_student3AVView;
	CZegoAVView m_studeng4AVView;
	CZegoAVView m_student5AVView;
	CZegoAVView m_student6AVView;	
    CProgressCtrl m_captureVolumeProg;
  
private:
    SettingsPtr m_pAVSettings;
    RoomPtr m_pChatRoom;
    unsigned char* m_pAuxData;
    int m_nAuxDataLen;
    int m_nAuxDataPos;
    bool m_bSystemCapture = false;
    bool m_bIsAnchor = false;
    bool m_bIsPublishing = false;
    int m_iRequestJoinLiveSeq = -1;
    std::string m_strPublishStreamID;
    std::string m_strCurUserID;
    std::string m_strCurUserName;
    std::vector<std::string> m_vecAudioDeviceIDs;
    std::vector<std::string> m_vecVideoDeviceIDs;
};
