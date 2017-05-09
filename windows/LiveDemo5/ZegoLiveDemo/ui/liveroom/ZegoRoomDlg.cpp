// TaklDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "resource.h"
#include "ZegoRoomDlg.h"
#include "ZegoLiveDemo.h"
#include <string>
#include <tuple>
#include "ZegoUtility.h"
#include "ZegoLiveDemo.h"

#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include "error/en.h"

using namespace ZEGO;


#define VOLUME_TIMER_ID   1002

static int g_AVViews[] =
{
	IDC_AVVIEW_PRIMARY,
	IDC_AVVIEW_STUDENT1,
	IDC_AVVIEW_STUDENT2,
	IDC_AVVIEW_STUDENT3,
	IDC_AVVIEW_STUDENT4,
	IDC_AVVIEW_STUDENT5,
	IDC_AVVIEW_STUDENT6,
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CZegoRoomDlg 对话框

IMPLEMENT_DYNAMIC(CZegoRoomDlg, CDialogEx)

CZegoRoomDlg::CZegoRoomDlg(SettingsPtr curSettings, RoomPtr room, std::string curUserID, std::string curUserName, bool isAnchor, CWnd* pParent) :
    CDialogEx(IDD_DIALOG_ROOM, pParent),
    m_pAVSettings(curSettings),
    m_pChatRoom(room),
    m_strCurUserID(curUserID),
    m_strCurUserName(curUserName),
    m_bIsAnchor(isAnchor),
	m_bCKEnableMic(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    GetAVSignal().OnLoginRoom().connect(this, &CZegoRoomDlg::OnLoginRoom);
    GetAVSignal().OnStreamUpdated().connect(this, &CZegoRoomDlg::OnStreamUpdated);
    GetAVSignal().OnPublishStateUpdate().connect(this, &CZegoRoomDlg::OnPublishStateUpdate);
    GetAVSignal().OnPlayStateUpdate().connect(this, &CZegoRoomDlg::OnPlayStateUpdate);
    GetAVSignal().OnDisconnect().connect(this, &CZegoRoomDlg::OnDisconnect);
    GetAVSignal().OnKickOut().connect(this, &CZegoRoomDlg::OnKickOut);
    GetAVSignal().OnPublishQulityUpdate().connect(this, &CZegoRoomDlg::OnPublishQulityUpdate);
    GetAVSignal().OnPlayQualityUpdate().connect(this, &CZegoRoomDlg::OnPlayQualityUpdate);
    GetAVSignal().OnAuxInput().connect(this, &CZegoRoomDlg::OnAVAuxInput);
    GetAVSignal().OnSendRoomMessage().connect(this, &CZegoRoomDlg::OnSendRoomMessage);
    GetAVSignal().OnRecvRoomMesge().connect(this, &CZegoRoomDlg::OnRecvRoomMessage);
    GetAVSignal().OnJoinLiveRequest().connect(this, &CZegoRoomDlg::OnJoinLiveRequest);
    GetAVSignal().OnJoinLiveResponse().connect(this, &CZegoRoomDlg::OnJoinLiveResponse);

	m_pAuxData = NULL;
	m_nAuxDataLen = 0;
	m_nAuxDataPos = 0;
}

CZegoRoomDlg::~CZegoRoomDlg()
{
}

void CZegoRoomDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_AVVIEW_PRIMARY, m_primaryAVView);
    DDX_Control(pDX, IDC_AVVIEW_STUDENT1, m_student1AVView);
    DDX_Control(pDX, IDC_AVVIEW_STUDENT2, m_student2AVView);
    DDX_Control(pDX, IDC_AVVIEW_STUDENT3, m_student3AVView);
    DDX_Control(pDX, IDC_AVVIEW_STUDENT4, m_studeng4AVView);
    DDX_Control(pDX, IDC_AVVIEW_STUDENT5, m_student5AVView);
    DDX_Control(pDX, IDC_AVVIEW_STUDENT6, m_student6AVView);
    DDX_Control(pDX, IDC_EDIT_MESSAGE, m_edMessage);
    DDX_Control(pDX, IDC_LISTBOX_CHAT, m_lbChatContent);
    DDX_Check(pDX, IDC_CHECK_MICROPHONE, m_bCKEnableMic);
    DDX_Control(pDX, IDC_EDIT_STREAMURL, m_edStreamUrl);
    DDX_Control(pDX, IDC_BUTTON_AUX, m_btnAux);
    DDX_Control(pDX, IDC_COMBO_CURAUDIO, m_cbCurrentAudio);
    DDX_Control(pDX, IDC_COMBO_CURVIDEO, m_cbCurrentVideo);
    DDX_Control(pDX, IDC_BUTTON_CAPTURE, m_btnCapture);
    DDX_Control(pDX, IDC_CAPTURE_VOLUME_PROG, m_captureVolumeProg);
    DDX_Control(pDX, IDC_BUTTON_JOIN, m_btnStartPublish);
}

BOOL CZegoRoomDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);	// 设置大图标
	SetIcon(m_hIcon, FALSE);	// 设置小图标

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if ( pSysMenu != NULL )
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	ModifyStyleEx(0, WS_EX_APPWINDOW);

	UpdateData(FALSE);

    m_btnCapture.EnableWindow(FALSE);

	CString strTile;
	strTile.Format(_T("直播间(%s)"), UTF8ToWString(m_pChatRoom->GetRoomName().c_str()).c_str());
	SetWindowText(strTile);

	m_edMessage.SetCueBanner(_T("聊会儿天..."));

	m_btnAux.EnableWindow(FALSE);
    m_btnCapture.EnableWindow(FALSE);

    m_avaliableView.push(6);
    m_avaliableView.push(5);
    m_avaliableView.push(4);
    m_avaliableView.push(3);
    m_avaliableView.push(2);
    m_avaliableView.push(1);
    m_avaliableView.push(0);

	LIVEROOM::EnableMic((bool)m_bCKEnableMic);

    EnumVideoAndAudioDevice();

    m_captureVolumeProg.SetRange(0, 100);
    m_captureVolumeProg.SetPos(0);

    m_btnStartPublish.EnableWindow(FALSE);
    SetPublishText();

    EnableWindow(FALSE);
    int role = m_bIsAnchor ? LIVEROOM::ZegoRoomRole::Anchor : LIVEROOM::ZegoRoomRole::Audience;
    if (!LIVEROOM::LoginRoom(m_pChatRoom->GetRoomID().c_str(), role, m_pChatRoom->GetRoomName().c_str()))
    {
        MessageBox(L"进入房间失败", L"提示");
        EnableWindow(TRUE);
    }

	return TRUE;
}

void::CZegoRoomDlg::SetPublishText()
{
   
    if (m_bIsAnchor)
    {
        if (m_bIsPublishing)
        {
            m_btnStartPublish.SetWindowText(_T("停止直播"));
        }
        else
        {
            m_btnStartPublish.SetWindowText(_T("开始直播"));
        }
    }
    else
    {
        if (m_bIsPublishing)
        {
            m_btnStartPublish.SetWindowText(_T("停止连麦"));
        }
        else
        {
            m_btnStartPublish.SetWindowText(_T("开始连麦"));
        }
    }
   
}

void CZegoRoomDlg::OnStartPublish()
{
    if (m_bIsAnchor)
    {
        if (m_bIsPublishing)
        {
            m_bIsPublishing = false;
            StopPublishStream(m_strPublishStreamID);
            m_edStreamUrl.SetWindowText(NULL);
            SetPublishText();
        }
        else
        {
            StartPublishStream();
        }
    }
    else
    {
        if (m_bIsPublishing)
        {
            m_bIsPublishing = false;
            StopPublishStream(m_strPublishStreamID);
            m_edStreamUrl.SetWindowText(NULL);
            SetPublishText();
        }
        else
        {
            // 请求连麦
            m_iRequestJoinLiveSeq = LIVEROOM::RequestJoinLive();
        }
    }
}

void CZegoRoomDlg::EnumVideoAndAudioDevice()
{
    int nCBCurSel(0);

    int nDeviceCount = 0;
    AV::DeviceInfo* pDeviceList(NULL);

    nCBCurSel = 0;
    pDeviceList = LIVEROOM::GetAudioDeviceList(AV::AudioDevice_Input, nDeviceCount);
    for (int i = 0; i < nDeviceCount; ++i)
    {
        m_cbCurrentAudio.AddString(UTF8ToWString(pDeviceList[i].szDeviceName).c_str());
        m_vecAudioDeviceIDs.push_back(pDeviceList[i].szDeviceId);
        if (m_pAVSettings->GetMircophoneId() == std::string(pDeviceList[i].szDeviceId))
            nCBCurSel = i;
    }
    m_cbCurrentAudio.SetCurSel(nCBCurSel);
    LIVEROOM::FreeDeviceList(pDeviceList);
    pDeviceList = NULL;

    nCBCurSel = 0;
    pDeviceList = LIVEROOM::GetVideoDeviceList(nDeviceCount);
    for (int i = 0; i < nDeviceCount; ++i)
    {
        m_cbCurrentVideo.AddString(UTF8ToWString(pDeviceList[i].szDeviceName).c_str());
        m_vecVideoDeviceIDs.push_back(pDeviceList[i].szDeviceId);
        if (m_pAVSettings->GetCameraId() == std::string(pDeviceList[i].szDeviceId))
            nCBCurSel = i;
    }
    m_cbCurrentVideo.SetCurSel(nCBCurSel);
    LIVEROOM::FreeDeviceList(pDeviceList);
    pDeviceList = NULL;
}


BEGIN_MESSAGE_MAP(CZegoRoomDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
    ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CZegoRoomDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_BACK, &CZegoRoomDlg::OnBnClickedButtonBack)
	ON_COMMAND(IDOK, &CZegoRoomDlg::OnIdok)
	ON_BN_CLICKED(IDC_CHECK_MICROPHONE, &CZegoRoomDlg::OnBnClickedCheckMicrophone)
	ON_BN_CLICKED(IDC_BUTTON_AUX, &CZegoRoomDlg::OnBnClickedButtonAux)
    ON_CBN_SELCHANGE(IDC_COMBO_CURVIDEO, &CZegoRoomDlg::OnCbnSelchangeComboCurvideo)
    ON_CBN_SELCHANGE(IDC_COMBO_CURAUDIO, &CZegoRoomDlg::OnCbnSelchangeComboCuraudio)
    ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &CZegoRoomDlg::OnBnClickedButtonCapture)
    ON_LBN_SELCHANGE(IDC_LISTBOX_CHAT, &CZegoRoomDlg::OnLbnSelchangeListboxChat)
    ON_BN_CLICKED(IDC_BUTTON_JOIN, &CZegoRoomDlg::OnStartPublish)
END_MESSAGE_MAP()


// CZegoRoomDlg 消息处理程序
void CZegoRoomDlg::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	CPoint pt(1024, 720); 
	lpMMI->ptMinTrackSize = pt;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CZegoRoomDlg::OnClose()
{
    GetOut();
	__super::OnClose();
}

void CZegoRoomDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
}

void CZegoRoomDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CZegoRoomDlg::OnTimer(UINT_PTR id)
{
    CDialogEx::OnTimer(id);

    if (id == VOLUME_TIMER_ID)
    {
        float fCaptureLevel = LIVEROOM::GetCaptureSoundLevel();
        m_captureVolumeProg.SetPos(fCaptureLevel);
    }
}

void CZegoRoomDlg::GetOut()
{
    EndAux();
    for (auto& stream : m_pChatRoom->GetStreamList())
    {
        if (stream->IsCurUserCreated())
        {
            StopPublishStream(stream->GetStreamID());
        }
        else
        {
            StopPlayStream(stream->GetStreamID());
        }
    }
    LIVEROOM::LogoutRoom();
}

void CZegoRoomDlg::OnBnClickedButtonBack()
{
    GetOut();
	EndDialog(100001);
}

void CZegoRoomDlg::OnBnClickedCheckMicrophone()
{
	UpdateData();
    LIVEROOM::EnableMic((bool)m_bCKEnableMic);
}



void CZegoRoomDlg::OnBnClickedButtonAux()
{
	LONG lAuxStatus = GetWindowLong(m_btnAux.GetSafeHwnd(), GWL_USERDATA);
	lAuxStatus == 0 ? BeginAux() : EndAux();
}

void CZegoRoomDlg::BeginAux(void)
{
	if (GetWindowLong(m_btnAux.GetSafeHwnd(), GWL_USERDATA) == 1)
	{
		return;
	}

	char szAppPath[MAX_PATH] = { 0 };
	::GetModuleFileNameA(NULL, szAppPath, MAX_PATH);
	CStringA strAppPath = szAppPath;
	CStringA strAppDir = strAppPath.Left(strAppPath.ReverseFind('\\') + 1);

	OPENFILENAMEA ofn = { 0 };
	char strAuxFilePath[MAX_PATH] = { 0 };
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "pcm文件\0*.pcm;\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = strAuxFilePath;
	ofn.nMaxFile = sizeof(strAuxFilePath);
	ofn.lpstrInitialDir = strAppDir;
	ofn.lpstrTitle = "请选择一个混音文件";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileNameA(&ofn))
	{
		FILE* fAux;
		fopen_s(&fAux, strAuxFilePath, "rb");
		if (fAux == NULL) { return; }

		fseek(fAux, 0, SEEK_END);

		m_nAuxDataLen = ftell(fAux);
		if (m_nAuxDataLen > 0)
		{
			m_pAuxData = new unsigned char[m_nAuxDataLen];
			memset(m_pAuxData, 0, m_nAuxDataLen);
		}

		fseek(fAux, 0, 0);

		int nReadDataLen = fread(m_pAuxData, sizeof(unsigned char), m_nAuxDataLen, fAux);

		fclose(fAux);

        LIVEROOM::EnableAux(TRUE);

		m_btnAux.SetWindowText(_T("关闭混音"));
		SetWindowLong(m_btnAux.GetSafeHwnd(), GWL_USERDATA, 1);
	}
}

void CZegoRoomDlg::EndAux(void)
{
	if (GetWindowLong(m_btnAux.GetSafeHwnd(), GWL_USERDATA) == 0)
	{
		return;
	}

    LIVEROOM::EnableAux(FALSE);

	m_btnAux.SetWindowText(_T("开启混音"));
	SetWindowLong(m_btnAux.GetSafeHwnd(), GWL_USERDATA, 0);

	delete[] m_pAuxData;
	m_nAuxDataLen = 0;
	m_nAuxDataPos = 0;
}

void CZegoRoomDlg::StartPublishStream()
{
    SYSTEMTIME st;
    GetLocalTime(&st);
    CString cstrStreamID;
    cstrStreamID.Format(_T("s-windows-%s-%d"), UTF8ToWString(m_strCurUserID.c_str()).c_str(), (int)st.wMilliseconds);
    m_strPublishStreamID = WStringToUTF8(cstrStreamID.GetString());

    StreamPtr pPublishStream(new CZegoStreamModel(m_strPublishStreamID, m_strCurUserID, m_strCurUserName, "", true));

    m_pChatRoom->AddStream(pPublishStream);
    if (m_avaliableView.size() > 0)
    {
        int nIndex = m_avaliableView.top();
        m_avaliableView.pop();
        pPublishStream->SetPlayView(nIndex);

        CZegoAVView* pAVView = (CZegoAVView*)GetDlgItem(g_AVViews[nIndex]);
        if (pAVView == nullptr || !IsWindow(pAVView->GetDisplaySafeWnd()))
        {
            return;
        }

       /* if (m_pAVSettings == nullptr || m_pAVSettings->GetCameraId().empty())
        {
            MessageBox(L"检测不到摄像头", L"提示");
            return;
        }*/

        LIVEROOM::SetPreviewView(pAVView->GetDisplaySafeWnd());
        LIVEROOM::SetPreviewViewMode(LIVEROOM::ZegoVideoViewModeScaleToFill);
        LIVEROOM::StartPreview();

        static int i = 0;
        std::string cdnFlag;

        if (i++ % 2)
        {
            cdnFlag = "?zmcdn=ws1";
        }
        else
        {
            cdnFlag = "?zmcdn=aliyun1";
        }

        std::string streamID = m_strPublishStreamID + cdnFlag;

        LIVEROOM::StartPublishing(m_pChatRoom->GetRoomName().c_str(), streamID.c_str(), 0, "");
    }
}

void CZegoRoomDlg::StartPlayStream(StreamPtr stream)
{
    if (stream == nullptr) { return; }

    m_pChatRoom->AddStream(stream);
    if (m_avaliableView.size() > 0)
    {
        int nIndex = m_avaliableView.top();
        m_avaliableView.pop();
        stream->SetPlayView(nIndex);

        CZegoAVView* pAVView = (CZegoAVView*)GetDlgItem(g_AVViews[nIndex]);
        if (pAVView == nullptr || !IsWindow(pAVView->GetDisplaySafeWnd()))
        {
            return;
        }

        LIVEROOM::SetViewMode(LIVEROOM::ZegoVideoViewModeScaleToFill, stream->GetStreamID().c_str());
        LIVEROOM::StartPlayingStream(stream->GetStreamID().c_str(), pAVView->GetDisplaySafeWnd());
    }
}

void CZegoRoomDlg::StopPublishStream(const std::string& streamID)
{
    if (streamID.size() == 0) { return; }

    LIVEROOM::StopPublishing();
    LIVEROOM::StopPreview();
    LIVEROOM::SetPreviewView(nullptr);

    StreamPtr pStream = m_pChatRoom->RemoveStream(streamID);
    FreeAVView(pStream);
}

void CZegoRoomDlg::StopPlayStream(const std::string& streamID)
{
    if (streamID.size() == 0) { return; }

    LIVEROOM::StopPlayingStream(streamID.c_str());

    StreamPtr pStream = m_pChatRoom->RemoveStream(streamID);
    FreeAVView(pStream);
}

void CZegoRoomDlg::FreeAVView(StreamPtr stream)
{
    if (stream == nullptr)
    {
        return;
    }

    int nIndex = stream->GetPlayView();
    m_avaliableView.push(nIndex);

    CZegoAVView* pAVView = (CZegoAVView*)GetDlgItem(g_AVViews[nIndex]);
    if (pAVView != nullptr)
    {
        pAVView->ShowQuailtyTips(-1);
    }
}


void CZegoRoomDlg::OnLoginRoom(int errorCode, std::string strRoomID, std::vector<StreamPtr> vStreamList)
{

    EnableWindow(TRUE);

    if (errorCode != 0)
    {
        MessageBox(L"登录房间失败", L"提示");
        return;
    }

    if (m_bIsAnchor)
    {
        StartPublishStream();
    }
    else
    {
        // 观众，激活连麦按钮
        m_btnStartPublish.EnableWindow(TRUE);
    }

    for (auto& stream : vStreamList)
    {
        StartPlayStream(stream);
    }
}

void CZegoRoomDlg::OnDisconnect(int errorCode, std::string roomID)
{
    if (m_pChatRoom->GetRoomID() == roomID)
    {
        MessageBox(L"已掉线", L"提示");

        EndDialog(100001);
    }
}

void CZegoRoomDlg::OnKickOut(int reason, std::string roomID)
{
    if (m_pChatRoom->GetRoomID() ==  roomID)
    {
        MessageBox(L"被踢出房间", L"提示");

        EndDialog(100001);
    }
}

void CZegoRoomDlg::OnStreamUpdated(std::string strRoomID, std::vector<StreamPtr> vStreamList, LIVEROOM::ZegoStreamUpdateType type)
{
    for (auto& stream : vStreamList)
    {
        if (type == LIVEROOM::ZegoStreamUpdateType::StreamAdded)
        {
            StartPlayStream(stream);
        }
        else if (type == LIVEROOM::ZegoStreamUpdateType::StreamDeleted)
        {
            StopPlayStream(stream->GetStreamID());
        }
    }
}

void  CZegoRoomDlg::OnPublishStateUpdate(int stateCode, std::string streamID, StreamPtr stream)
{
    if (stateCode == 0)
    {
        m_btnAux.EnableWindow(TRUE);
        m_btnCapture.EnableWindow(TRUE);
        SetTimer(VOLUME_TIMER_ID, 100, NULL);

        if (stream != nullptr)
        {
            CString strUrl;

            CString strRtmpUrl = (stream->m_vecRtmpUrls.size() > 0) ?
                UTF8ToWString(stream->m_vecRtmpUrls[0].c_str()).c_str() : _T("");
            if (!strRtmpUrl.IsEmpty())
            {
                strUrl.Append(_T("1. "));
                strUrl.Append(strRtmpUrl);
                strUrl.Append(_T("\r\n"));
            }

            CString strFlvUrl = (stream->m_vecFlvUrls.size() > 0) ?
                UTF8ToWString(stream->m_vecFlvUrls[0].c_str()).c_str() : _T("");
            if (!strFlvUrl.IsEmpty())
            {
                strUrl.Append(_T("2. "));
                strUrl.Append(strFlvUrl);
                strUrl.Append(_T("\r\n"));
            }

            CString strHlsUrl = (stream->m_vecHlsUrls.size() > 0) ?
                UTF8ToWString(stream->m_vecHlsUrls[0].c_str()).c_str() : _T("");
            if (!strHlsUrl.IsEmpty())
            {
                strUrl.Append(_T("3. "));
                strUrl.Append(strHlsUrl);
                strUrl.Append(_T("\r\n"));
            }

            m_edStreamUrl.SetWindowText(strUrl);
        }

        // 设置状态
        m_btnStartPublish.EnableWindow(TRUE);
        m_bIsPublishing = true;
        SetPublishText();
    }
    else
    {
        EndAux();
        m_btnAux.EnableWindow(FALSE);
        m_btnCapture.EnableWindow(FALSE);
        KillTimer(VOLUME_TIMER_ID);
        m_edStreamUrl.SetWindowText(_T(""));

        // 停止预览, 回收view
        LIVEROOM::StopPreview();
        LIVEROOM::SetPreviewView(nullptr);
        StreamPtr pStream = m_pChatRoom->RemoveStream(streamID);
        FreeAVView(pStream);

        // 设置状态
        m_bIsPublishing = false;
        m_btnStartPublish.EnableWindow(TRUE);
        SetPublishText();
    }
}

void CZegoRoomDlg::OnPlayStateUpdate(int stateCode, std::string streamID)
{
    if (stateCode != 0)
    {
        // 回收view
        StreamPtr pStream = m_pChatRoom->RemoveStream(streamID);
        FreeAVView(pStream);
    }
}

void CZegoRoomDlg::OnPlayQualityUpdate(std::string streamID, int quality, double videoFPS, double videoKBS)
{
    StreamPtr pStream = m_pChatRoom->GetStreamByID(streamID);
    if (pStream == nullptr) { return; }

    int nIndex = pStream->GetPlayView();
    if (nIndex < 0 || nIndex > 6)
    {
        return;
    }

    CZegoAVView* pAVView = (CZegoAVView*)GetDlgItem(g_AVViews[nIndex]);
    if (pAVView != nullptr)
    {
        pAVView->ShowQuailtyTips(quality);
    }
}

void CZegoRoomDlg::OnPublishQulityUpdate(std::string streamID, int quality, double videoFPS, double videoKBS)
{
    StreamPtr pStream = m_pChatRoom->GetStreamByID(streamID);
    if (pStream == nullptr)
    {
        return;
    }

    int nIndex = pStream->GetPlayView();
    if (nIndex < 0 || nIndex > 6)
    {
        return;
    }

    CZegoAVView* pAVView = (CZegoAVView*)GetDlgItem(g_AVViews[nIndex]);
    if (pAVView != nullptr)
    {
        pAVView->ShowQuailtyTips(quality);
    }
}

void CZegoRoomDlg::OnAVAuxInput(unsigned char *pData, int *pDataLen, int *pSampleRate, int *pNumChannels)
{
    if (m_pAuxData != nullptr && (*pDataLen < m_nAuxDataLen))
    {
        *pSampleRate = 44100;
        *pNumChannels = 2;

        if (m_nAuxDataPos + *pDataLen > m_nAuxDataLen)
        {
            m_nAuxDataPos = 0;
        }

        int nCopyLen = *pDataLen;
        memcpy(pData, m_pAuxData + m_nAuxDataPos, nCopyLen);

        m_nAuxDataPos += nCopyLen;

        *pDataLen = nCopyLen;
    }
    else
    {
        *pDataLen = 0;
    }
}

void CZegoRoomDlg::OnAudioDeviceChanged(AV::AudioDeviceType deviceType, std::string strDeviceId, std::string strDeviceName, AV::DeviceState state)
{
    if (deviceType == AV::AudioDeviceType::AudioDevice_Output)
        return;

    if (state == AV::DeviceState::Device_Added)
    {
        m_cbCurrentAudio.AddString(UTF8ToWString(strDeviceName.c_str()).c_str());
        m_vecAudioDeviceIDs.push_back(strDeviceId);
    }
    else if (state == AV::DeviceState::Device_Deleted)
    {
        for (int i = 0; i < m_vecAudioDeviceIDs.size(); i++)
        {
            if (m_vecAudioDeviceIDs[i] != strDeviceId)
                continue;

            m_vecAudioDeviceIDs.erase(m_vecAudioDeviceIDs.begin() + i);

            int currentCurl = m_cbCurrentAudio.GetCurSel();
            if (currentCurl == i)
            {
                //MessageBox(L"当前音频采集设备被移除", L"提示", MB_OK);

                //默认采集第一个音频设备
                if (m_vecAudioDeviceIDs.size() > 0)
                {
                    LIVEROOM::SetAudioDevice(AV::AudioDevice_Input, m_vecAudioDeviceIDs[0].c_str());
                    m_pAVSettings->SetMicrophoneId(m_vecAudioDeviceIDs[0]);
                }
            }

            m_cbCurrentAudio.DeleteString(i);
            m_cbCurrentAudio.SetCurSel(0);
            break;
        }
    }
}

void CZegoRoomDlg::OnVideoDeviceChanged(std::string strDeviceId, std::string strDeviceName, AV::DeviceState state)
{
    if (state == AV::DeviceState::Device_Added)
    {
        m_cbCurrentVideo.AddString(UTF8ToWString(strDeviceName.c_str()).c_str());
        m_vecVideoDeviceIDs.push_back(strDeviceId);
    }
    else if (state == AV::DeviceState::Device_Deleted)
    {
        for (int i = 0; i < m_vecVideoDeviceIDs.size(); i++)
        {
            if (m_vecVideoDeviceIDs[i] != strDeviceId)
                continue;

            m_vecVideoDeviceIDs.erase(m_vecVideoDeviceIDs.begin() + i);

            int currentCurl = m_cbCurrentVideo.GetCurSel();
            if (currentCurl == i)
            {
                //MessageBox(L"当前视频设备被移除", L"提示", MB_OK);
                //默认采集第一个视频设备
                if (m_vecVideoDeviceIDs.size() > 0)
                {
                    LIVEROOM::SetVideoDevice(m_vecVideoDeviceIDs[0].c_str());
                    m_pAVSettings->SetCameraId(m_vecVideoDeviceIDs[0]);
                }
            }

            m_cbCurrentVideo.DeleteString(i);
            m_cbCurrentVideo.SetCurSel(0);
            break;
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// 消息相关处理
void CZegoRoomDlg::OnIdok()
{
	if (GetFocus()->GetSafeHwnd() == m_edMessage.GetSafeHwnd())
	{
		OnBnClickedButtonSend();
	}
}

void CZegoRoomDlg::OnBnClickedButtonSend()
{
	CString strChat;
	m_edMessage.GetWindowText(strChat);
	if (strChat.IsEmpty())
	{
		return;
	}

	m_strLastSendMsg =  WStringToUTF8(strChat);

    LIVEROOM::SendRoomMessage(ROOM::ZegoMessageType::Text, ROOM::ZegoMessageCategory::Chat, ROOM::ZegoMessagePriority::Default, m_strLastSendMsg.c_str());

	m_edMessage.SetWindowText(L"");
}

void CZegoRoomDlg::OnSendRoomMessage(int errorCode, std::string pszRoomID, int sendSeq, unsigned long long messageId)
{
    if (errorCode != 0) { return; }

    if(m_strLastSendMsg.empty()) {return ;}

    CString cstrTmpContent;
    cstrTmpContent.Format(_T("我: %s"), UTF8ToWString(m_strLastSendMsg.c_str()).c_str());
    m_lbChatContent.InsertString(-1, cstrTmpContent);

    m_strLastSendMsg.clear();
}

void CZegoRoomDlg::OnRecvRoomMessage(std::string roomID, std::vector<RoomMsgPtr> vRoomMsgList)
{
    for (auto& roomMsg : vRoomMsgList)
    {
        CString cstrTmpContent;
        cstrTmpContent.Format(_T("%s: %s"), UTF8ToWString(roomMsg->GetUserID().c_str()).c_str(), UTF8ToWString(roomMsg->GetContent().c_str()).c_str());
        m_lbChatContent.InsertString(-1, cstrTmpContent);
    }
}

void CZegoRoomDlg::OnJoinLiveRequest(int seq, std::string fromUserID, std::string fromUserName, std::string roomID)
{
    CString cstrMsg;
    cstrMsg.Format(_T("%s %s"), UTF8ToWString(fromUserID.c_str()).c_str(), _T("正在请求连麦"));
    if (IDOK == MessageBox(cstrMsg, L"提示", MB_OKCANCEL))
    {
        LIVEROOM::RespondJoinLiveReq(seq, 0);
    }
    else
    {
        LIVEROOM::RespondJoinLiveReq(seq, -1);
    }
  
}

void CZegoRoomDlg::OnJoinLiveResponse(int result, std::string fromUserID, std::string fromUserName, int seq)
{
    if (seq == m_iRequestJoinLiveSeq)
    {
        if (result == 0)
        {
            StartPublishStream();
        }
        else
        {
            MessageBox(L"连麦请求被拒绝", L"提示");
        }
    }
}


void CZegoRoomDlg::OnCbnSelchangeComboCurvideo()
{
    // TODO: Add your control notification handler code here
    int currentCurl = m_cbCurrentVideo.GetCurSel();
    if (currentCurl < m_vecVideoDeviceIDs.size())
    {
        LIVEROOM::SetVideoDevice(m_vecVideoDeviceIDs[currentCurl].c_str());
        m_pAVSettings->SetCameraId(m_vecVideoDeviceIDs[currentCurl]);
    }
}


void CZegoRoomDlg::OnCbnSelchangeComboCuraudio()
{
    // TODO: Add your control notification handler code here
    int currentCurl = m_cbCurrentAudio.GetCurSel();
    if (currentCurl < m_vecAudioDeviceIDs.size())
    {
        LIVEROOM::SetAudioDevice(AV::AudioDevice_Input, m_vecAudioDeviceIDs[currentCurl].c_str());
        m_pAVSettings->SetMicrophoneId(m_vecAudioDeviceIDs[currentCurl]);
    }
}


void CZegoRoomDlg::OnBnClickedButtonCapture()
{
    // TODO: Add your control notification handler code here
    if (m_bSystemCapture)
    {
        m_bSystemCapture = false;
        m_btnCapture.SetWindowText(_T("声卡采集"));
    }
    else
    {
        m_bSystemCapture = true;
        m_btnCapture.SetWindowText(_T("关闭采集"));
    }

    LIVEROOM::EnableMixSystemPlayout(m_bSystemCapture);
}


void CZegoRoomDlg::OnLbnSelchangeListboxChat()
{
    // TODO: Add your control notification handler code here
}
