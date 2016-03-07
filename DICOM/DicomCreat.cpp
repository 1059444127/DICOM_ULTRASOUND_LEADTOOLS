// DicomCreat.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "DICOM.h"
#include "DicomCreat.h"
#include "afxdialogex.h"
#include "DICOMDlg.h"
#include "L_Bitmap.h"
#include "DICOMSend.h"
// DicomCreat 대화 상자입니다.
IMPLEMENT_DYNAMIC(DicomCreat, CDialog)

DicomCreat::DicomCreat(CWnd* pParent /*=NULL*/)
	: CDialog(DicomCreat::IDD, pParent)
	, sBitmapPath(_T(""))
	, m_eName(_T("Name"))
	, m_eID(_T("Patient ID"))
	, m_eBirthDay(_T("YYYYMMDD"))
	, sFolderName(_T(""))
	, m_ServerPort(_T(""))
	, m_SeverAE(_T(""))
	, m_UserAE(_T(""))
{

}

DicomCreat::~DicomCreat()
{

}

void DicomCreat::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_bitmap, m_Bitmap);
	DDX_Text(pDX, IDC_EDIT1, m_eName);
	DDX_Text(pDX, IDC_EDIT2, m_eID);
	DDX_Text(pDX, IDC_EDIT3, m_eBirthDay);
	DDX_Control(pDX, IDC_COMBO1, m_cSEX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_SeverIP);
	DDX_Text(pDX, IDC_EDIT5, m_ServerPort);
	DDX_Text(pDX, IDC_EDIT6, m_SeverAE);
	DDX_Text(pDX, IDC_EDIT7, m_UserAE);
}


BEGIN_MESSAGE_MAP(DicomCreat, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCANCEL, &DicomCreat::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &DicomCreat::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_Input, &DicomCreat::OnBnClickedButtonInput)
	ON_BN_CLICKED(IDC_BUTTON_Send, &DicomCreat::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_DCMSAVE, &DicomCreat::OnBnClickedButtonDcmsave)
	ON_BN_CLICKED(IDC_BUTTON_PChange, &DicomCreat::OnBnClickedButtonPchange)
	ON_BN_CLICKED(IDC_BUTTON_DCMChange, &DicomCreat::OnBnClickedButtonDcmchange)
END_MESSAGE_MAP()


// DicomCreat 메시지 처리기입니다.


void DicomCreat::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	MousClip();
	// 그리기 메시지에 대해서는 CDialog::OnPaint()을(를) 호출하지 마십시오.
}


void DicomCreat::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	CDialog::OnCancel();
}


BOOL DicomCreat::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//L_DicomStartUp(); // 꼭 필요하다.
	L_INT nRet = L_SetLicenseFile(_T("D:\\METABIOMED\\1_System\\Ultrasound_MS09\\dicom_meta\\meta.lic"),
		_T("3Mx+XodTlJbjbUvDFtFmk/mRWcBZLi0IkN2k3qZptzOWoLXYPWOU1xiYtXqhCnFE"));
	m_cSEX.InsertString(0, _T("Male"));
	m_cSEX.InsertString(1, _T("Female"));
	SetBitmap();
	PatientInfo(sBitmapPath);
	PACSInfo();
	GetDlgItem(IDC_BUTTON_Send)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


bool DicomCreat::MousClip()
{
	CRect Rect;
	CPoint ptChild;
	GetWindowRect(&Rect);
	ptChild.x = Rect.left;
	ptChild.y = Rect.top;

	// 창 크기
	CRect c;
	GetClientRect(c);

	// 마우스 위치
	CPoint PosM, SPosM;
	GetCursorPos(&PosM);
	SPosM = PosM;

	ClipCursor(Rect);
	ScreenToClient(&SPosM);
	//CDialogEx::OnPaint();

	return TRUE;
}


// DicomCreat에서 초기화 및 Bitmap 파일 위치 저장
bool DicomCreat::Initialize(CString* BitmapPath, CString* FolderName)
{
	sBitmapPath = *BitmapPath;
	sFolderName = *FolderName;
	
	CString rstring;
	int a = sBitmapPath.Find(sFolderName);
	rstring = sBitmapPath.Mid(0, a - 1);
	a = rstring.ReverseFind(_T('\\'));
	rstring = rstring.Mid(a + 1);

	// 환자 ID를 폴더에서 정보 가져오기.
	int A = rstring.Find('-');
	m_eID = rstring.Left(A);

	// 환자 이름을 폴더에서 정보 가져오기.
	CString PaName = rstring.Mid(A + 1);
	int Aa = PaName.Find('-');
	m_eName = PaName.Left(Aa);


	HANDLE h_File = CreateFile(sBitmapPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	FILETIME create_time, access_time, write_time;
	GetFileTime(h_File, &create_time, &access_time, &write_time);
	FileTimeToSystemTime(&write_time, &write_system_time);


	return TRUE;
}


bool DicomCreat::SetBitmap()
{
	hBitmap = (HBITMAP)LoadImage(0, sBitmapPath, IMAGE_BITMAP, 512, 384, LR_LOADFROMFILE);
	m_Bitmap.SetBitmap(hBitmap);
	m_Bitmap.MoveWindow(20, 50, 512, 384);

	if (!hBitmap)::CloseHandle(hBitmap);

	return TRUE;
}


void DicomCreat::OnBnClickedButton1() // 불러온 Bitmap을 DCM으로 변환.
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	InitDCM(sBitmapPath);
}

bool DicomCreat::InitDCM(CString sFileName)
{
	GetDlgItem(IDC_BUTTON_Send)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Create)->EnableWindow(FALSE);

	LDicomDS		CurDS;
	HDICOMDS		hDS;
	L_INT			nRet;

	hDS = L_DicomCreateDS(NULL);
	::L_DicomInitDS(hDS, CLASS_SC_IMAGE_STORAGE, 0);
	AfxMessageBox(sFileName.GetBuffer(sFileName.GetLength()));

	nRet = L_LoadBitmap((TCHAR*)(LPCTSTR)sFileName, &loadbitmap, sizeof(BITMAPHANDLE), 8, ORDER_BGR, NULL, NULL);

	if (nRet != TRUE)
	{
		CString a;
		AfxMessageBox(_T("Bitmap file Load: Fail"));
		return FALSE;
	}

	nRet = CurDS.LoadDS(sFileName.GetBuffer(sFileName.GetLength()), 0);


	if (nRet != DICOM_SUCCESS)
	{
		MessageBox(TEXT("Not able to open the file"), TEXT("DICOM ERROR"), MB_ICONSTOP);
		return FALSE;
	}
	TagDCM(hDS);
	int fd = FactoryDCM(hDS);
	if (fd == 0)
	{
		AfxMessageBox(_T("오류 발생 (1)"));
	}
	int pd = PatientDCM(hDS);
	if (pd == 0)
	{
		AfxMessageBox(_T("오류 발생 (2)"));
	}
	int sd = StudyDCM(hDS);
	if (sd == 0)
	{
		AfxMessageBox(_T("오류 발생 (3)"));
	}
	int rd = SeriesDCM(hDS);
	if (rd == 0)
	{
		AfxMessageBox(_T("오류 발생 (4)"));
	}
	int id = ImageDCM(hDS);
	if (id == 0)
	{
		AfxMessageBox(_T("오류 발생 (5)"));
	}
	int ed = EquipmentDCM(hDS);
	if (ed == 0)
	{
		AfxMessageBox(_T("오류 발생 (6)"));
	}

	int FileN = sBitmapPath.Find('.');
	CString FileName;
	FileName = sBitmapPath.Mid(0, FileN);

	FilePath_Save = FileName + _T(".dcm");

	// File Path
	nRet = L_DicomSaveDS(hDS, (TCHAR*)(LPCTSTR)FilePath_Save, DS_LITTLE_ENDIAN | DS_IMPLICIT_VR);
	if (nRet != DICOM_SUCCESS)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	L_DicomFreeDS(hDS);

	L_FreeBitmap(&loadbitmap);
	return true;
}



bool DicomCreat::PatientDCM(HDICOMDS &hDS)
{
	UpdateData(TRUE);
	pDICOMELEMENT	nElement;
	L_INT			nRet;
	//Patient Name 설정
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_PATIENT_NAME, FALSE);
	CString cName;
	cName = m_eName.GetString();
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)cName, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}

	//Patient ID 설정
	CString cID;
	cID = m_eID.GetString();
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_PATIENT_ID, FALSE);
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)cID, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}

	//Patient Birthday 설정
	CString cBirth, cYear, cMonth, cDay, rBirth;
	cBirth = m_eBirthDay.GetString();
	cYear = cBirth.Left(4);
	cMonth = cBirth.Mid(4, 2);
	cDay = cBirth.Right(2);
	rBirth = cMonth + _T("/") + cDay + _T("/") + cYear;
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_PATIENT_BIRTH_DATE, FALSE);
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)rBirth, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}

	//SEX 설정
	CString cSEX;
	int numS = m_cSEX.GetCurSel();
	m_cSEX.GetLBText(numS, cSEX);
	
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_PATIENT_SEX, FALSE);
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)cSEX, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}

	//Age 설정
	int age = write_system_time.wYear - _ttoi(cYear) + 1;
	CString sAge;
	sAge.Format(_T("%d"), age);
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_PATIENT_AGE, FALSE); 
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)sAge, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}

	return TRUE;
}


// Study Value 입력
bool DicomCreat::StudyDCM(HDICOMDS & hDS)
{
	pDICOMELEMENT	nElement;
	L_INT			nRet;

	CString StudyUID;
	StudyUID.Format(_T("%d"), write_system_time.wMonth);

	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_STUDY_ID, FALSE);
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)StudyUID, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	//Study UID 설정
	CString UID;
	UID.Format(_T("0.0.%d.0.%d.0.%d.0.") + StudyUID, write_system_time.wYear, write_system_time.wMonth, write_system_time.wDay);
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_STUDY_INSTANCE_UID, TRUE);
	//nRet = L_DicomSetConvertValue(hDS, nElement, _T("0.0.0.0.0.5167978576.0.1010030507.1923201825"), 1);
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)UID, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	return TRUE;
}


// Series Value 추가
bool DicomCreat::SeriesDCM(HDICOMDS & hDS)
{
	pDICOMELEMENT	nElement;
	L_INT			nRet;

	//Modality
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_MODALITY, FALSE);
	
	nRet = L_DicomSetConvertValue(hDS, nElement, _T("SC"), 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}

	CString SeriesUID;
	SeriesUID.Format(_T("%d"), write_system_time.wDay);

	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_SERIES_NUMBER, TRUE); 
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)SeriesUID, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	CString UID;
	UID.Format(_T("0.0.%d.0.%d.0.%d.") + SeriesUID, write_system_time.wYear, write_system_time.wMonth, write_system_time.wDay);
	//Series Instance UID 설정
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_SERIES_INSTANCE_UID, TRUE);
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)UID, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	return TRUE;
}


// Image Value 추가
bool DicomCreat::ImageDCM(HDICOMDS & hDS)
{
	pDICOMELEMENT	nElement;
	L_INT			nRet;

	nElement = ::L_DicomFindFirstElement(hDS, NULL, TAG_PIXEL_DATA, FALSE); 


	nRet = L_DicomInsertImage(hDS, NULL, &loadbitmap, 0, IMAGE_COMPRESSION_NONE,
		IMAGE_PHOTOMETRIC_RGB, loadbitmap.BitsPerPixel, 0, 0, NULL, NULL);
	if (nRet != DICOM_SUCCESS)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	L_FreeBitmap(&loadbitmap);



	//TAG_SERIES_DATE 설정
	CString SeriesDate;
	SeriesDate.Format(_T("%d/%d/%d"), write_system_time.wMonth, write_system_time.wDay, write_system_time.wYear);
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_SERIES_DATE, FALSE); 
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)SeriesDate, 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	// Image 번호
	CString FileName = sBitmapPath.Right(8);
	nElement = ::L_DicomFindFirstElement(hDS, NULL, TAG_INSTANCE_NUMBER, FALSE); 
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)FileName.Left(4), 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}	
	//SOP instance UID (☆ 관리 요망) 
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_SOP_INSTANCE_UID, TRUE); 
	nRet = L_DicomSetConvertValue(hDS, nElement, (TCHAR*)(LPCTSTR)FileName.Left(4), 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}

	return true;
}


bool DicomCreat::EquipmentDCM(HDICOMDS & hDS)
{
	pDICOMELEMENT	nElement;
	L_INT			nRet;

	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_CONVERSION_TYPE, TRUE); 
	nRet = L_DicomSetConvertValue(hDS, nElement, _T("SI"), 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	return TRUE;
}


bool DicomCreat::TagDCM(HDICOMDS & hDS)
{
	pDICOMELEMENT	nElement;

	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_FILE_META_INFORMATION_VERSION, TRUE);
	if (nElement == NULL)
	{
		L_DicomInsertElement(hDS, nElement, TRUE, TAG_FILE_META_INFORMATION_VERSION, VR_OB, FALSE, 0);
	}
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_MEDIA_STORAGE_SOP_INSTANCE_UID, TRUE);
	if (nElement == NULL)
	{
		L_DicomInsertElement(hDS, nElement, TRUE, TAG_MEDIA_STORAGE_SOP_INSTANCE_UID, VR_UI, FALSE, 0);
	}
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_IMPLEMENTATION_CLASS_UID, TRUE);
	if (nElement == NULL)
	{
		L_DicomInsertElement(hDS, nElement, TRUE, TAG_IMPLEMENTATION_CLASS_UID, VR_UI, FALSE, 0);
	}
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_FILE_META_INFORMATION_VERSION, FALSE);
	if (nElement == NULL)
	{
		L_DicomSetConvertValue(hDS, nElement, _T("0x01"), 1);
	}
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_MEDIA_STORAGE_SOP_INSTANCE_UID, TRUE);
	if (nElement == NULL)
	{
		L_DicomSetConvertValue(hDS, nElement, _T("1.2.276.0.7230010.3.1.4.914591144.1552.1398584984.1"), 1);
	}
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_IMPLEMENTATION_CLASS_UID, FALSE);
	if (nElement == NULL)
	{
		L_DicomSetConvertValue(hDS, nElement, _T("1.2.276.0.7230010.3.0.3.5.4"), 1);
	}

	return TRUE;
}


bool DicomCreat::FactoryDCM(HDICOMDS & hDS)
{
	pDICOMELEMENT	nElement;
	L_INT			nRet;

	// 장비 명 입력
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_SOURCE_APPLICATION_ENTITY_TITLE, TRUE);
	nRet = L_DicomSetConvertValue(hDS, nElement, _T("SmarSONO MS-09"), 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	// 회사명 입력
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_MANUFACTURER, TRUE);
	nRet = L_DicomSetConvertValue(hDS, nElement, _T("METABIOMED"), 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	// INSTITUTION NAME
	nElement = L_DicomFindFirstElement(hDS, NULL, TAG_INSTITUTION_NAME, TRUE);
	nRet = L_DicomSetConvertValue(hDS, nElement, _T("SmartSONO"), 1);
	if (nRet != TRUE)
	{
		AfxMessageBox(_T("Fail"));
		return FALSE;
	}
	return TRUE;
}


//환자 정보 저장 하는 버튼
void DicomCreat::OnBnClickedButtonInput()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	PatientFile.Open(sPatientInfo, CFile::modeCreate | CFile::modeReadWrite, NULL);

	if (m_eBirthDay.IsEmpty() || m_eID.IsEmpty() || m_eName.IsEmpty() || m_cSEX.GetCurSel() < 0)
	{
		AfxMessageBox(_T("Wrong"));
	}
	else if (m_eBirthDay.GetLength() != 8)
	{
		AfxMessageBox(_T("Wrong"));
	}
	else
	{
		CString sPatinein;
		sPatinein = _T("_1_") + m_eName + _T("_2_") + m_eID + _T("_3_") + m_eBirthDay + _T("_4_");
		sPatinein.Format(sPatinein + _T("%d"), m_cSEX.GetCurSel());
		PatientFile.WriteString(sPatinein);
		PatientFile.Close();

		GetDlgItem(IDC_BUTTON_Input)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_Create)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);


	}
}



void DicomCreat::OnBnClickedButtonSend()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_BUTTON_Send)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Create)->EnableWindow(TRUE);

	DICOMSend a;
	a.DCMFilePath(&FilePath_Save, &sDCMSendInfo);
}





// 환자 정보가 있는지 확인. 없으면 생성, 있으면 불러오기.
bool DicomCreat::PatientInfo(CString BitmapPath)
{
	int flag = 0;

	int num = BitmapPath.Find(_T("SINO"));
	sPatientInfo = BitmapPath.Mid(0, num);
	sPatientInfo = sPatientInfo + _T("Paitent Infomation.dat");


	if (!PatientFile.Open(sPatientInfo, CFile::modeReadWrite))
	{
 		flag = 1; // 파일이 없다.
	}
	else
	{
		flag = 2; // 파일이 있다.
	}

	switch (flag)
	{
	case 1: // 파일이 없을 경우.
		break;
	case 2: // 파일이 있을 경우.

		CString strn;
		PatientFile.ReadString(strn);
		PatientFile.Close(); // 파일을 불러 왔으면 닫아 줘야 함. 메모리에 남으면 안됨!

		int num2 = strn.Find(_T("_2_"));
		int num3 = strn.Find(_T("_3_"));
		int num4 = strn.Find(_T("_4_"));
		int len = strn.GetLength();
		int cursel = 0;


		m_eName = strn.Mid(3, num2 - 3);
		m_eID = strn.Mid(num2 + 3, num3 - num2 - 3);
		m_eBirthDay = strn.Mid(num3 + 3, num4 - num3 - 3);
		m_cSEX.SetCurSel(_ttoi(strn.Right(1)));

		UpdateData(FALSE);

		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT3)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO1)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_Input)->EnableWindow(FALSE);
		break;
	}

	return TRUE;
}


void DicomCreat::OnBnClickedButtonPchange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT2)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT3)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Input)->EnableWindow(TRUE);
}


// PACS에 저장될 Information 함수
bool DicomCreat::PACSInfo()
{
	int flag = 0;
	sDCMSendInfo = _T("C:\\PACS information.dat");

	if (!DCMSendFile.Open(sDCMSendInfo, CFile::modeReadWrite))
	{
		flag = 1; // 파일이 없다.
	}
	else
	{
		flag = 2; // 파일이 있다.
	}

	switch (flag)
	{
	case 1: // 파일이 없을 경우.
		break;
	case 2: // 파일이 있을 경우.

		CString strn;
		DCMSendFile.ReadString(strn);

		int num2 = strn.Find(_T("_2_"));
		int num3 = strn.Find(_T("_3_"));
		int num4 = strn.Find(_T("_4_"));
		int len = strn.GetLength();


		m_SeverIP.SetWindowText(strn.Mid(3, num2 - 3));
		m_ServerPort = strn.Mid(num2 + 3, num3 - num2 - 3);
		m_SeverAE = strn.Mid(num3 + 3, num4 - num3 - 3);
		m_UserAE = strn.Mid(num4 + 3, len - num4 + 3 + 1);


		UpdateData(FALSE);

		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DCMSAVE)->EnableWindow(FALSE);

		DCMSendFile.Close(); // 파일을 불러 왔으면 닫아 줘야 함. 메모리에 남으면 안됨!

		
		break;
	}
	return TRUE;
}

void DicomCreat::OnBnClickedButtonDcmsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DCMSendFile.Open(sDCMSendInfo, CFile::modeReadWrite | CFile::modeCreate);

	UpdateData(TRUE);


	CString sSeverip, sSeverport, sSverAE, sUserAE;
	m_SeverIP.GetWindowText(sSeverip);
	sSeverport = m_ServerPort;
	sSverAE = m_SeverAE;
	sUserAE = m_UserAE;
	

	if (m_ServerPort.IsEmpty() || m_SeverAE.IsEmpty() || m_UserAE.IsEmpty() || sSeverip.IsEmpty())
	{
		AfxMessageBox(_T("Wrong"));
	}
	else
	{
		CString sDCMinfo;
		sDCMinfo = _T("_1_") + sSeverip + _T("_2_") + m_ServerPort + _T("_3_") + m_SeverAE + _T("_4_") + m_UserAE;
		DCMSendFile.WriteString(sDCMinfo);
		DCMSendFile.Close();

		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT6)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT7)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DCMSAVE)->EnableWindow(FALSE);
	}

}

void DicomCreat::OnBnClickedButtonDcmchange()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT6)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DCMSAVE)->EnableWindow(TRUE);
}
