
// DICOMDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "DICOM.h"
#include "DICOMDlg.h"
#include "afxdialogex.h"
#include "DicomCreat.h"
#include <GdiPlus.h>
using namespace Gdiplus;

#define T_Height  150
#define T_Width  200

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDICOMDlg 대화 상자



CDICOMDlg::CDICOMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDICOMDlg::IDD, pParent)
	, PathName(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDICOMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_TreeC);
	DDX_Control(pDX, IDC_LIST1, m_ListC);
}

BEGIN_MESSAGE_MAP(CDICOMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE1, &CDICOMDlg::TreeDClk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CDICOMDlg::BitmapDCLK)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CDICOMDlg 메시지 처리기

BOOL CDICOMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.


	m_ImageListThumb.Create(200, 150, ILC_COLOR24, 0, 1);
	m_ListC.SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
	m_nSelectedItem = 0;



	FindFolder();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CDICOMDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CDICOMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		MousClip();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CDICOMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// Double Click시 cFindFolder 실행 (List에 Bitmap 을 표시 할 것임)
void CDICOMDlg::TreeDClk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	HTREEITEM hItem;
	CString strName, path;


	hItem = m_TreeC.GetNextItem(NULL, TVGN_CARET); // 현재 선택된 아이템의 핸들을 가져온다.
	strName = m_TreeC.GetItemText(hItem); // 그 아이템의 이름을 얻어온다.
	path = strName;
	while (!strName.IsEmpty())
	{
		hItem = m_TreeC.GetNextItem(hItem, TVGN_PARENT); // 현재 선택되어진 아이템의 상위 아이템을 가져온다.
		strName = m_TreeC.GetItemText(hItem); // 그 아이템의 이름을 얻어온다.
		path = strName + _T("\\") + path;
	}
	
	CString rstr;
	rstr = path.Mid(11);
	rstr = PathName + rstr;

	/*GDI+ 초기화*/
	st_gplus();

	/*THUMBNAIL 확인*/
	//1. File 불러오기. (.bmp)
	ListCall(rstr);



	*pResult = 0;
}


/*cFindFolder 첫 실행 지점*/
bool CDICOMDlg::FindFolder()
{
	CString path = _T("D:\\METABIOMED\\1_System\\Ultrasound_MS09\\SCUS\\SYSDATA\\SystemSet.xml");
	//CString path = _T("C:\\Program Files\\SCUS\\SYSDATA\\SystemSet.xml"); // MS09

	/* System안에 있는 dir 정보를 얻기 위한 dll */
	SearchDir(path, PathName); // input : path , Output : PathName

	/*TreeControl에 입력하기 위한 dll*/
	SDir(PathName, m_TreeC);
	return true;
}





void CDICOMDlg::BitmapDCLK(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	

	// TreeControl 주소값 얻어오기.

	POSITION pos = m_ListC.GetFirstSelectedItemPosition();
	int i = m_ListC.GetNextSelectedItem(pos);
	CString ListName = m_ListC.GetItemText(i, 0);
	CString Total_Path;
	int a = strFolderItem.ReverseFind(_T('\\'));
	Total_Path = strFolderItem.Mid(0 , a + 1) + ListName;
	a = ListName.Find(_T(".bmp"));
	CString TreePath = ListName.Mid(0, a);

	if (i < 0)
	{
		AfxMessageBox(_T("Click on the outside."));
	}
	else if (i >= 0)
	{
		cDicomCreat.Initialize(&Total_Path, &TreePath);
		cDicomCreat.DoModal();
	}



	*pResult = 0;
}


bool CDICOMDlg::MousClip()
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
	CDialogEx::OnPaint();

	return TRUE;
}


void CDICOMDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	MousClip(); // Creat로 움직인 후 다시 들어 올때 Clip을 수행 하여 재 설정을 한다.
	__super::OnMouseMove(nFlags, point);
}



void CDICOMDlg::st_gplus()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}


void CDICOMDlg::ed_gplus()
{
	GdiplusShutdown(gdiplusToken);

}


// List에 있는 파일을 불러오기
bool CDICOMDlg::ListCall(CString path)
{
	CFileFind file; // 파일을 찾기 위한 메소드
	BOOL b = file.FindFile(path + _T("\\*.bmp"));			// 모든 확장자를 다 사용.
	CString strTempString;

	// reset our image list
	for (int i = 0; i<m_ImageListThumb.GetImageCount(); i++)
		m_ImageListThumb.Remove(i);

	// remove all items from list view
	if (m_ListC.GetItemCount() != 0)
		m_ListC.DeleteAllItems();

	int cnt = 0;

	while (b)
	{
		b = file.FindNextFile();

		strFolderItem = file.GetFilePath(); // bitmap 경로 명.

		int Ppath_len = path.GetLength() + 1;			// 폴더 이름을 구하기 위하여 몇번 째부터 있는 확인 하기 위함
		strTempString = strFolderItem.Mid(Ppath_len);	// 실지적으로 있는 bmp file 이름

		//m_ListC->InsertItem(cnt, strTempString, GetIconIndexFromFile(strFolderItem));
		ThumbnailF(strFolderItem, cnt);

		cnt++;
	}

	return TRUE;
}


void CDICOMDlg::ThumbnailF(CString path, int Cnt)
{
	// hold the window update to avoid flicking

	CBitmap*    pImage = NULL;
	HBITMAP		hBmp;
	POINT		pt;
	int i = Cnt;

	int nGap = 16;

	m_ImageListThumb.SetImageCount(i+1); // 이미지 가져오기.

	USES_CONVERSION;
	Bitmap img((path));
	Bitmap* pThumbnail = static_cast<Bitmap*>(img.GetThumbnailImage(T_Width, T_Height, NULL, NULL));
	
	// attach the thumbnail bitmap handle to an CBitmap object
	pThumbnail->GetHBITMAP(NULL, &hBmp);
	pImage = new CBitmap();
	pImage->Attach(hBmp);

	// attach the thumbnail bitmap handle to an CBitmap object
	pThumbnail->GetHBITMAP(NULL, &hBmp);
	pImage = new CBitmap();
	pImage->Attach(hBmp);

	// add bitmap to our image list
	m_ImageListThumb.Replace(i, pImage, NULL);

	// put item to display
	// set the image file name as item text
	// 반대로 계산.
	int A = path.ReverseFind(_T('\\')) + 1;
	CString aa;
	aa = path.Mid(A);

	m_ListC.InsertItem(i, aa, i);


	// get current item position	 
	m_ListC.GetItemPosition(i, &pt);

	// shift the thumbnail to desired position

	int num = 3; // 한줄에 몇개씩 사용 할지 지정.
	int CntN = Cnt / num;
	pt.x = (i - CntN * num)*(T_Width)+(i - CntN * num) * T_Width * 0.02;
	pt.y = (T_Height*CntN) + (T_Height*CntN) *0.15;
	m_ListC.SetItemPosition(i, pt);
	

	delete pImage;
	delete pThumbnail;

	m_ListC.SetRedraw();
}
