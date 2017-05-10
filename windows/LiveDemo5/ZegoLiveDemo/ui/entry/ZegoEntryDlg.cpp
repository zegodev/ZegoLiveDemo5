// StartingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ZegoEntryDlg.h"
#include "ZegoRoomDlg.h"
#include "ZegoSettingsDlg.h"
#include "CGridListCtrlX/CGridColumnTraitButton.h"
#include "ZegoLiveDemo.h"
#include "ZegoUtility.h"
#include "ZegoUserModel.h"
#include "LiveRoom.h"
#include "LiveRoomDefines.h"

#include "rapidjson.h"
#include "document.h"
#include "stringbuffer.h"
#include "writer.h"
#include "error/en.h"
#include <time.h>

#define AUTO_GETROOMLIST_TIMMERID	10086
#define ROOMLIST_MAX_NUM			30

// CZegoEntryDlg 对话框

IMPLEMENT_DYNAMIC(CZegoEntryDlg, CDialogEx)

CZegoEntryDlg::CZegoEntryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_ENTRY, pParent)
	, m_bCKAutoRefresh(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pRoomDlg = nullptr;
}

CZegoEntryDlg::~CZegoEntryDlg()
{
	m_roomList.clear();

    theApp.GetBase().UninitAVSdk();

	delete m_pRoomDlg;
	m_pRoomDlg = nullptr;
}

void CZegoEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_USERID, m_strEdUserId);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_strEdUserName);
	DDX_Control(pDX, IDC_COMBO_QUALITY, m_cbQuality);
	DDX_Control(pDX, IDC_LIST_ROOM, m_lsRooms);
	DDX_Control(pDX, IDC_EDIT_ROOMTITLE, m_edNewRoomTitle);
	DDX_Check(pDX, IDC_CHECK_AUTOREFRESH, m_bCKAutoRefresh);
}

BOOL CZegoEntryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);	// 设置大图标
	SetIcon(m_hIcon, FALSE);	// 设置小图标

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	EnableToolTips();

	m_userConfig.LoadConfig();

	m_strEdUserId = m_userConfig.GetUserId().c_str();
	m_strEdUserName = m_userConfig.GetUserName().c_str();

	VideoQuality quality = m_userConfig.GetVideoQuality();
	m_cbQuality.AddString(_T("超低质量"));
	m_cbQuality.AddString(_T("低质量"));
	m_cbQuality.AddString(_T("标准质量"));
	m_cbQuality.AddString(_T("高质量"));
	m_cbQuality.AddString(_T("超高质量"));
	m_cbQuality.AddString(_T("自定义"));
	m_cbQuality.SetCurSel(quality);

	m_lsRooms.SetCellMargin(1.2);
	m_lsRooms.EnableVisualStyles(true);
	m_lsRooms.InsertHiddenLabelColumn();
	m_lsRooms.SetEmptyMarkupText(_T("当前没有房间正在直播..."));
	m_lsRooms.OnTraitSignal().connect(this, &CZegoEntryDlg::OnBnRoomListEntry);

	m_lsRooms.InsertColumnTrait(1, L"No.", LVCFMT_CENTER, 60, 1, NULL);
	CRect rtList;
	m_lsRooms.GetWindowRect(&rtList);
	m_lsRooms.InsertColumnTrait(2, L"标题", LVCFMT_LEFT, rtList.Width() - 60 - 200, 2, NULL);

	CGridColumnTraitButton* pTraitButton = new CGridColumnTraitButton;
	m_lsRooms.InsertColumnTrait(3, L"详情", LVCFMT_CENTER, 200, 3, pTraitButton);

	m_edNewRoomTitle.SetCueBanner(_T("给直播起个标题吧[可选]"));

	UpdateData(FALSE);

    SettingsPtr pCurSettings = m_userConfig.GetVideoSettings();
    if (pCurSettings != nullptr)
    {
        std::string userID = WStringToUTF8(m_userConfig.GetUserId().c_str());
        std::string userName = WStringToUTF8(m_userConfig.GetUserName().c_str());
        theApp.GetBase().InitAVSdk(pCurSettings, userID, userName);
    }

    // pull房间列表
    PullRoomList();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CZegoEntryDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CLOSE()
	ON_WM_NCLBUTTONDOWN()
	ON_BN_CLICKED(IDOK, &CZegoEntryDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CZegoEntryDlg::OnBnClickedButtonSetting)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CZegoEntryDlg::OnBnClickedButtonRefresh)
	ON_CBN_SELCHANGE(IDC_COMBO_QUALITY, &CZegoEntryDlg::OnCbnSelchangeComboQuality)
	ON_BN_CLICKED(IDC_CHECK_AUTOREFRESH, &CZegoEntryDlg::OnBnClickedCheckAutorefresh)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, SetTipText)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ROOM, &CZegoEntryDlg::OnNMDblclkListRoom)
END_MESSAGE_MAP()

// CStartingDlg 消息处理程序

void CZegoEntryDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CZegoEntryDlg::OnClose()
{
	UpdateData();

	m_userConfig.SetUserId(m_strEdUserId.GetBuffer());
	m_userConfig.SetUserName(m_strEdUserName.GetBuffer());
	m_userConfig.SaveConfig();

	KillTimer(AUTO_GETROOMLIST_TIMMERID);

	m_lsRooms.OnTraitSignal().disconnect(this);

	CDialogEx::OnClose();
}

void CZegoEntryDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	if (nHitTest == HTHELP)
	{
		ShellExecute(GetSafeHwnd(), _T("open"), _T("www.zego.im"), NULL, NULL, SW_SHOWNORMAL);
		return;
	}

	CDialogEx::OnNcLButtonDown(nHitTest, point);
}

BOOL CZegoEntryDlg::SetTipText(UINT id, NMHDR *pTTTStruct, LRESULT *pResult)
{
	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pTTTStruct;
	UINT nID = pTTTStruct->idFrom;   

	if (pTTT->uFlags & TTF_IDISHWND)
	{
		nID = ::GetDlgCtrlID((HWND)nID);
		if (nID == 0)
		{
			return FALSE;
		}

		// strcpy(pTTT->lpszText, your_string1);

		return TRUE;
	}
	return FALSE;
}

void CZegoEntryDlg::OnBnClickedButtonRefresh()
{
    PullRoomList();
}

void CZegoEntryDlg::OnBnClickedCheckAutorefresh()
{
    UpdateData();

    if (m_bCKAutoRefresh)
    {
        SetTimer(AUTO_GETROOMLIST_TIMMERID, 5000, &CZegoEntryDlg::AutoGetRoomInfoList);
    }
    else
    {
        KillTimer(AUTO_GETROOMLIST_TIMMERID);
    }
}

void CZegoEntryDlg::AutoGetRoomInfoList(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
    if (nIDEvent == AUTO_GETROOMLIST_TIMMERID)
    {
    }
}

void CZegoEntryDlg::PullRoomList()
{
    CString cstrBaseUrl;
    cstrBaseUrl.Format(_T("https://liveroom%u-api.zego.im/demo/roomlist?appid=%u"), theApp.GetBase().GetAppID(), theApp.GetBase().GetAppID());

    CInternetSession session;
    CHttpFile *file = nullptr;
    std::string strJson;
  
    try
    {
        InternetSetOption(session, INTERNET_OPTION_RESET_URLCACHE_SESSION, NULL, 0);
        file = (CHttpFile *)session.OpenURL(cstrBaseUrl);
    }
    catch (CInternetException *e)
    {
        file = nullptr;
        e->Delete();
        session.Close();
    }
    
    if (file != nullptr)
    {
        char szBuf[1024];
        while (file->ReadString((LPTSTR)szBuf, 1024))
        {
            strJson += szBuf;
        }

        file->Close();
        session.Close();
        delete file;
        file = nullptr;

        // 解析房间列表
        ParseRoomList(strJson);
    }
}

void CZegoEntryDlg::ParseRoomList(std::string json)
{
    if (json.size() == 0) { return; }

    std::vector<RoomPtr> vRoomList;

    rapidjson::Document doc;
    doc.Parse<rapidjson::kParseStopWhenDoneFlag>(json.c_str());
    if (doc.HasParseError())
    {
        rapidjson::ParseErrorCode errorCode = doc.GetParseError();
        return;
    }

    if (!doc.HasMember("code"))
    { 
        return; 
    }
    rapidjson::Value& code = doc["code"];

    if (code.GetInt() != 0)
    {
        MessageBox(L"获取房间列表错误", L"提示");
        return;
    }

    if (!doc.HasMember("data"))
    {
        return;
    }
    rapidjson::Value& data = doc["data"];

    if (!data.HasMember("room_list"))
    {
        return;
    }
    rapidjson::Value& valArray = data["room_list"];
    if (valArray.IsArray())
    {
        for (int i = 0; i < valArray.Size(); i++)
        {
            rapidjson::Value& room = valArray[i];
            std::string strRoomID = room["room_id"].GetString();
            std::string strRoomName = room["room_name"].GetString();
            std::string strAnchorID = room["anchor_id_name"].GetString();
            std::string strAnchorName = room["anchor_nick_name"].GetString();

            int nAnchorSize = 0;
            if (room["stream_info"].IsArray())
            {
                nAnchorSize = room["stream_info"].Size();
            }

            if (nAnchorSize == 0)
                continue;

            RoomPtr pRoom(new CZegoRoomModel(strRoomID, strRoomName, strAnchorID, strAnchorName));
            pRoom->SetLivesCount(nAnchorSize);

            vRoomList.push_back(pRoom);
        }
     }

     RefreshRoomList(vRoomList);
}

void CZegoEntryDlg::OnBnClickedButtonSetting()
{
	UpdateData();

	CZegoSettingsDlg settingsDlg(m_userConfig.GetVideoSettings());
	if (IDOK == settingsDlg.DoModal())
	{
		m_userConfig.SaveConfig();
	}

	VideoQuality quality = m_userConfig.GetVideoQuality();
	m_cbQuality.SetCurSel(quality);
}

void CZegoEntryDlg::OnCbnSelchangeComboQuality()
{
	VideoQuality quality = (VideoQuality)m_cbQuality.GetCurSel();
	if (quality == VQ_SelfDef)
	{
		OnBnClickedButtonSetting();
	}
	else
	{
		m_userConfig.SetVideoQuality(quality);
	}
}

void CZegoEntryDlg::OnNMDblclkListRoom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	if (pNMItemActivate->iItem >= 0)
	{
		OnBnRoomListEntry(pNMItemActivate->iItem);
	}

	*pResult = 0;
}

void CZegoEntryDlg::OnBnRoomListEntry(int nRow)
{
	if (m_pRoomDlg != nullptr) { return; }

	do
	{
		DWORD dwIndex = m_lsRooms.GetItemData(nRow);
		if (dwIndex >= m_roomList.size())
		{
			break;
		}

        RoomPtr pRoom = m_roomList[dwIndex];

		UpdateData();

        if (m_strEdUserId.IsEmpty() || m_strEdUserName.IsEmpty())
        {
            break;
        }

        m_userConfig.SetUserRole(false);
        m_userConfig.SetUserId(m_strEdUserId.GetString());
        m_userConfig.SetUserName(m_strEdUserName.GetString());

        std::string strUserId = WStringToUTF8(m_userConfig.GetUserId().c_str()).c_str();
        std::string strUserName = WStringToUTF8(m_userConfig.GetUserName().c_str()).c_str();
        // 更新用户信息
        LIVEROOM::SetUser(strUserId.c_str(), strUserName.c_str());

		SettingsPtr pCurSettings = m_userConfig.GetVideoSettings();
		if (pCurSettings == nullptr)
		{
			break;
		}

		m_userConfig.SaveConfig();

        ShowWindow(SW_HIDE);

        m_pRoomDlg = new CZegoRoomDlg(pCurSettings, pRoom, strUserId, strUserName, false);
        if (m_pRoomDlg->DoModal() != 100001)
         {
              theApp.m_pMainWnd->PostMessage(WM_CLOSE);
         }
         else
         {
              ShowWindow(SW_SHOW);
         }

        delete m_pRoomDlg;
        m_pRoomDlg = nullptr;
	} while (false);
}

void CZegoEntryDlg::OnBnClickedOk()
{
	if (m_pRoomDlg != nullptr) { return; }

	do
	{
		UpdateData();

		if (m_strEdUserId.IsEmpty() || m_strEdUserName.IsEmpty())
		{
			break;
		}

        m_userConfig.SetUserRole(true);
		m_userConfig.SetUserId(m_strEdUserId.GetString());
		m_userConfig.SetUserName(m_strEdUserName.GetString());

        std::string strUserId = WStringToUTF8(m_userConfig.GetUserId().c_str()).c_str();
        std::string strUserName = WStringToUTF8(m_userConfig.GetUserName().c_str()).c_str();
        // 更新用户信息
        LIVEROOM::SetUser(strUserId.c_str(), strUserName.c_str());

		SettingsPtr pCurSettings = m_userConfig.GetVideoSettings();
		if (pCurSettings == nullptr)
		{
			break;
		}

        if (pCurSettings->GetCameraId().length() == 0)
            pCurSettings->InitDeviceId();

        SYSTEMTIME st;
        GetLocalTime(&st);
        CString cstrRoomID;
        cstrRoomID.Format(_T("#m-%s-%d"), m_userConfig.GetUserId().c_str(), (int)st.wMilliseconds);
        std::string strRoomID = WStringToUTF8(cstrRoomID.GetString());

        std::string strRoomName = "windows-room-" + strUserId;
        CString cstrRoomName;
        m_edNewRoomTitle.GetWindowText(cstrRoomName);
        if (!cstrRoomName.IsEmpty())
        {
            strRoomName = WStringToUTF8(cstrRoomName.GetString()).c_str();
        }

        ShowWindow(SW_HIDE);
      
        RoomPtr pRoom(new CZegoRoomModel(strRoomID, strRoomName, strUserId, strUserName));
        m_pRoomDlg = new CZegoRoomDlg(pCurSettings, pRoom, strUserId, strUserName, true);

        if (m_pRoomDlg->DoModal() != 100001)
        {
            theApp.m_pMainWnd->PostMessage(WM_CLOSE);
        }
        else
        {
            ShowWindow(SW_SHOW);
        }

        delete m_pRoomDlg;
        m_pRoomDlg = nullptr;
    } while (false);
}

void CZegoEntryDlg::RefreshRoomList(std::vector<RoomPtr> roomList)
{
	UpdateData();

	m_roomList.clear();
	m_roomList = roomList;

	m_lsRooms.DeleteAllItems();

	for (size_t index = 0; index < m_roomList.size(); index++)
	{
		CString strIndex;
		strIndex.Format(_T("%u"), index + 1);
		m_lsRooms.InsertItem(index, strIndex);
		m_lsRooms.SetItemText(index, 1, strIndex);

        CString cstrRoomName;
        std::wstring temp = UTF8ToWString(m_roomList[index]->GetRoomName().c_str());
        if (temp.size() == 0)
        {
            temp = UTF8ToWString(m_roomList[index]->GetRoomID().c_str());
        }
        cstrRoomName.Format(_T("%s"), temp.c_str());
		m_lsRooms.SetItemText(index, 2, cstrRoomName);

		CString strDetail(_T("-"));
		unsigned int uLivesCount = m_roomList[index]->GetLivesCount();
		if (uLivesCount != 0)
		{
			strDetail.Format(_T("%u人正在直播"), m_roomList[index]->GetLivesCount());
		}
		m_lsRooms.SetItemText(index, 3, strDetail);

		m_lsRooms.SetItemData(index, index);
	}
}