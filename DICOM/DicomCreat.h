#pragma once
#include "afxwin.h"
#include "ltdic.h"
#include "resource.h"
#include "afxcmn.h"
//#include "DICOMSend.h"
// DicomCreat 대화 상자입니다.

class DicomCreat : public CDialog
{
	DECLARE_DYNAMIC(DicomCreat)

public:
	DicomCreat(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DicomCreat();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DICOM_CREAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	bool MousClip();
	SYSTEMTIME write_system_time, write_local_time;

public:
	// DicomCreat에서 초기화 및 Bitmap 파일 위치 저장
	bool Initialize(CString* BitmapPath, CString* FolderName);
	CString sBitmapPath;
	// Picture Control 변수
	CStatic m_Bitmap;
	HBITMAP hBitmap = NULL;
	bool SetBitmap();
	afx_msg void OnBnClickedButton1();
	bool InitDCM(CString sFileName);
	BITMAPHANDLE	loadbitmap;

	bool PatientDCM(HDICOMDS &hDS);
	// Study Value 입력
	bool StudyDCM(HDICOMDS & hDS);
	// Series Value 추가
	bool SeriesDCM(HDICOMDS & hDS);
	// Image Value 추가
	bool ImageDCM(HDICOMDS & hDS);
	bool EquipmentDCM(HDICOMDS & hDS);
	bool TagDCM(HDICOMDS & hDS);
	bool FactoryDCM(HDICOMDS & hDS);


	
	// 환자 이름 변수
	CString m_eName;
	// 환자 ID 변수
	CString m_eID;
	// 환자 생일 변수
	CString m_eBirthDay;
	// SEX 변수 (콤보박스)
	CComboBox m_cSEX;
	afx_msg void OnBnClickedButtonInput();
	// 선택 된 폴더 이름
	CString sFolderName; // bitmap
	CString FilePath_Save; // dcm
	
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButtonSend();
	// 서버 아이피 주소 획득
	CIPAddressCtrl m_SeverIP;
	// 서버 포트 (CSTring)
	CString m_ServerPort;
	// SeverAE Title 변수
	CString m_SeverAE;
	// 사용자 AE 타이틀 변수
	CString m_UserAE;
	afx_msg void OnBnClickedButtonDcmsave();
	// 환자 정보가 있는지 확인. 없으면 생성, 있으면 불러오기.
	bool PatientInfo(CString BitmapPath);

	CStdioFile PatientFile; // 환자 정보를 파일에 저장하기 위하여 생성된 변수.
	CStdioFile DCMSendFile; // PACS에 보낼 파일정보를 저장하기 위하여 생성된 변수.
	CString sPatientInfo; // 환자 dat path
	CString sDCMSendInfo; // PACS dat path

	afx_msg void OnBnClickedButtonPchange();
	// PACS에 저장될 Information 함수
	bool PACSInfo();
	afx_msg void OnBnClickedButtonDcmchange();
};
