#pragma once
#include "afxwin.h"
#include "ltdic.h"
#include "resource.h"
#include "afxcmn.h"
//#include "DICOMSend.h"
// DicomCreat ��ȭ �����Դϴ�.

class DicomCreat : public CDialog
{
	DECLARE_DYNAMIC(DicomCreat)

public:
	DicomCreat(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DicomCreat();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DICOM_CREAT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	bool MousClip();
	SYSTEMTIME write_system_time, write_local_time;

public:
	// DicomCreat���� �ʱ�ȭ �� Bitmap ���� ��ġ ����
	bool Initialize(CString* BitmapPath, CString* FolderName);
	CString sBitmapPath;
	// Picture Control ����
	CStatic m_Bitmap;
	HBITMAP hBitmap = NULL;
	bool SetBitmap();
	afx_msg void OnBnClickedButton1();
	bool InitDCM(CString sFileName);
	BITMAPHANDLE	loadbitmap;

	bool PatientDCM(HDICOMDS &hDS);
	// Study Value �Է�
	bool StudyDCM(HDICOMDS & hDS);
	// Series Value �߰�
	bool SeriesDCM(HDICOMDS & hDS);
	// Image Value �߰�
	bool ImageDCM(HDICOMDS & hDS);
	bool EquipmentDCM(HDICOMDS & hDS);
	bool TagDCM(HDICOMDS & hDS);
	bool FactoryDCM(HDICOMDS & hDS);


	
	// ȯ�� �̸� ����
	CString m_eName;
	// ȯ�� ID ����
	CString m_eID;
	// ȯ�� ���� ����
	CString m_eBirthDay;
	// SEX ���� (�޺��ڽ�)
	CComboBox m_cSEX;
	afx_msg void OnBnClickedButtonInput();
	// ���� �� ���� �̸�
	CString sFolderName; // bitmap
	CString FilePath_Save; // dcm
	
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButtonSend();
	// ���� ������ �ּ� ȹ��
	CIPAddressCtrl m_SeverIP;
	// ���� ��Ʈ (CSTring)
	CString m_ServerPort;
	// SeverAE Title ����
	CString m_SeverAE;
	// ����� AE Ÿ��Ʋ ����
	CString m_UserAE;
	afx_msg void OnBnClickedButtonDcmsave();
	// ȯ�� ������ �ִ��� Ȯ��. ������ ����, ������ �ҷ�����.
	bool PatientInfo(CString BitmapPath);

	CStdioFile PatientFile; // ȯ�� ������ ���Ͽ� �����ϱ� ���Ͽ� ������ ����.
	CStdioFile DCMSendFile; // PACS�� ���� ���������� �����ϱ� ���Ͽ� ������ ����.
	CString sPatientInfo; // ȯ�� dat path
	CString sDCMSendInfo; // PACS dat path

	afx_msg void OnBnClickedButtonPchange();
	// PACS�� ����� Information �Լ�
	bool PACSInfo();
	afx_msg void OnBnClickedButtonDcmchange();
};
