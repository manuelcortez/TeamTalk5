/*
 * Copyright (c) 2005-2018, BearWare.dk
 * 
 * Contact Information:
 *
 * Bjoern D. Rasmussen
 * Kirketoften 5
 * DK-8260 Viby J
 * Denmark
 * Email: contact@bearware.dk
 * Phone: +45 20 20 54 59
 * Web: http://www.bearware.dk
 *
 * This source code is part of the TeamTalk SDK owned by
 * BearWare.dk. Use of this file, or its compiled unit, requires a
 * TeamTalk SDK License Key issued by BearWare.dk.
 *
 * The TeamTalk SDK License Agreement along with its Terms and
 * Conditions are outlined in the file License.txt included with the
 * TeamTalk SDK distribution.
 *
 */

// StreamMediaDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "StreamMediaDlg.h"

extern TTInstance* ttInst;

#define MAX_FILENAMES 10

// CStreamMediaDlg dialog

IMPLEMENT_DYNAMIC(CStreamMediaDlg, CDialog)

CStreamMediaDlg::CStreamMediaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStreamMediaDlg::IDD, pParent)
    , m_nVidCodecBitrate(DEFAULT_WEBM_VP8_BITRATE)
{

}

CStreamMediaDlg::~CStreamMediaDlg()
{
}

void CStreamMediaDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_VIDCODEC, m_wndVidCodec);
    DDX_Text(pDX, IDC_EDIT_VIDBITRATE, m_nVidCodecBitrate);
    DDX_Control(pDX, IDC_SPIN_VIDBITRATE, m_wndVidBitrateSpinCtrl);
    DDX_Control(pDX, IDC_STATIC_AUDIOFORMAT, m_wndAudioFormat);
    DDX_Control(pDX, IDC_STATIC_VIDEOFORMAT, m_wndVideoFormat);
    DDX_Control(pDX, IDC_EDIT_VIDBITRATE, m_wndVideoBitrate);
    DDX_Control(pDX, IDC_COMBO_FILENAME, m_wndFilename);
}


BEGIN_MESSAGE_MAP(CStreamMediaDlg, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_AUDIOBROWSE, &CStreamMediaDlg::OnBnClickedButtonBrowse)
END_MESSAGE_MAP()


// CStreamMediaDlg message handlers

BOOL CStreamMediaDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    TRANSLATE(*this, IDD);

    for(POSITION pos = m_fileList.GetHeadPosition(); pos != nullptr;)
        m_wndFilename.AddString(m_fileList.GetNext(pos));
    m_wndFilename.SetCurSel(0);

    AddString(m_wndVidCodec, _T("WebM VP8"), WEBM_VP8_CODEC);
    SetCurSelItemData(m_wndVidCodec, WEBM_VP8_CODEC);
    m_wndVidBitrateSpinCtrl.SetRange(0, 1000);

    UpdateMediaFile();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CStreamMediaDlg::UpdateMediaFile()
{
    CString szFileName;
    m_wndFilename.GetWindowText(szFileName);

    MediaFileInfo mediaFile;
    ZERO_STRUCT(mediaFile);
    TT_GetMediaFileInfo(szFileName, &mediaFile);
    BOOL audio = mediaFile.audioFmt.nAudioFmt != AFF_NONE;
    BOOL video = mediaFile.videoFmt.picFourCC != FOURCC_NONE;

    CString szAudioFormat = _T("Unknown format");
    CString szVideoFormat = _T("Unknown format");
    if(audio)
        szAudioFormat.Format(_T("%d Hz, %s"),
                             mediaFile.audioFmt.nSampleRate,
                             (mediaFile.audioFmt.nChannels == 2?
                             _T("Stereo") : _T("Mono")));
    if(video)
    {
        double fps = (double)mediaFile.videoFmt.nFPS_Numerator / (double)mediaFile.videoFmt.nFPS_Denominator;
        szVideoFormat.Format(_T("%dx%d %d FPS"),
                             mediaFile.videoFmt.nWidth,
                             mediaFile.videoFmt.nHeight,
                             (int)fps);
    }

    m_wndAudioFormat.SetWindowText(szAudioFormat);
    m_wndVideoFormat.SetWindowText(szVideoFormat);

    m_wndVidBitrateSpinCtrl.EnableWindow(video);
    m_wndVideoBitrate.EnableWindow(video);
}

void CStreamMediaDlg::OnBnClickedButtonBrowse()
{
    CString filetypes = _T("Media files (*.*)|*.*|");
    CFileDialog dlg(TRUE, 0, 0, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,filetypes, this);
    TCHAR s[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, s);

    if(dlg.DoModal() == IDOK)
    {
        m_wndFilename.SetWindowText(dlg.GetPathName());
        int nIndex = m_wndFilename.FindString(-1, dlg.GetPathName());
        if (nIndex != CB_ERR)
            m_wndFilename.DeleteString(nIndex);
        m_wndFilename.InsertString(0, dlg.GetPathName());
        if (m_wndFilename.GetCount() > MAX_FILENAMES)
            m_wndFilename.DeleteString(MAX_FILENAMES);
        UpdateMediaFile();
    }

    SetCurrentDirectory(s);
}


void CStreamMediaDlg::OnOK()
{
    CDialog::OnOK();

    m_fileList.RemoveAll();
    CString szFilename;

    m_wndFilename.GetWindowText(szFilename);
    m_fileList.AddTail(szFilename);

    for (int i=0;i<m_wndFilename.GetCount();++i)
    {
        CString s;
        m_wndFilename.GetLBText(i, s);
        if (s.CompareNoCase(szFilename) != 0)
            m_fileList.AddTail(s);
    }
}
