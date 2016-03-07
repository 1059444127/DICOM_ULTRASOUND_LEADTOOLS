
// DICOMDlg.cpp : ���� ����
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


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// CDICOMDlg ��ȭ ����



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


// CDICOMDlg �޽��� ó����

BOOL CDICOMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.


	m_ImageListThumb.Create(200, 150, ILC_COLOR24, 0, 1);
	m_ListC.SetImageList(&m_ImageListThumb, LVSIL_NORMAL);
	m_nSelectedItem = 0;



	FindFolder();

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CDICOMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		MousClip();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CDICOMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



// Double Click�� cFindFolder ���� (List�� Bitmap �� ǥ�� �� ����)
void CDICOMDlg::TreeDClk(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	HTREEITEM hItem;
	CString strName, path;


	hItem = m_TreeC.GetNextItem(NULL, TVGN_CARET); // ���� ���õ� �������� �ڵ��� �����´�.
	strName = m_TreeC.GetItemText(hItem); // �� �������� �̸��� ���´�.
	path = strName;
	while (!strName.IsEmpty())
	{
		hItem = m_TreeC.GetNextItem(hItem, TVGN_PARENT); // ���� ���õǾ��� �������� ���� �������� �����´�.
		strName = m_TreeC.GetItemText(hItem); // �� �������� �̸��� ���´�.
		path = strName + _T("\\") + path;
	}
	
	CString rstr;
	rstr = path.Mid(11);
	rstr = PathName + rstr;

	/*GDI+ �ʱ�ȭ*/
	st_gplus();

	/*THUMBNAIL Ȯ��*/
	//1. File �ҷ�����. (.bmp)
	ListCall(rstr);



	*pResult = 0;
}


/*cFindFolder ù ���� ����*/
bool CDICOMDlg::FindFolder()
{
	CString path = _T("D:\\METABIOMED\\1_System\\Ultrasound_MS09\\SCUS\\SYSDATA\\SystemSet.xml");
	//CString path = _T("C:\\Program Files\\SCUS\\SYSDATA\\SystemSet.xml"); // MS09

	/* System�ȿ� �ִ� dir ������ ��� ���� dll */
	SearchDir(path, PathName); // input : path , Output : PathName

	/*TreeControl�� �Է��ϱ� ���� dll*/
	SDir(PathName, m_TreeC);
	return true;
}





void CDICOMDlg::BitmapDCLK(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	

	// TreeControl �ּҰ� ������.

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

	// â ũ��
	CRect c;
	GetClientRect(c);

	// ���콺 ��ġ
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	MousClip(); // Creat�� ������ �� �ٽ� ��� �ö� Clip�� ���� �Ͽ� �� ������ �Ѵ�.
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


// List�� �ִ� ������ �ҷ�����
bool CDICOMDlg::ListCall(CString path)
{
	CFileFind file; // ������ ã�� ���� �޼ҵ�
	BOOL b = file.FindFile(path + _T("\\*.bmp"));			// ��� Ȯ���ڸ� �� ���.
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

		strFolderItem = file.GetFilePath(); // bitmap ��� ��.

		int Ppath_len = path.GetLength() + 1;			// ���� �̸��� ���ϱ� ���Ͽ� ��� °���� �ִ� Ȯ�� �ϱ� ����
		strTempString = strFolderItem.Mid(Ppath_len);	// ���������� �ִ� bmp file �̸�

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

	m_ImageListThumb.SetImageCount(i+1); // �̹��� ��������.

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
	// �ݴ�� ���.
	int A = path.ReverseFind(_T('\\')) + 1;
	CString aa;
	aa = path.Mid(A);

	m_ListC.InsertItem(i, aa, i);


	// get current item position	 
	m_ListC.GetItemPosition(i, &pt);

	// shift the thumbnail to desired position

	int num = 3; // ���ٿ� ��� ��� ���� ����.
	int CntN = Cnt / num;
	pt.x = (i - CntN * num)*(T_Width)+(i - CntN * num) * T_Width * 0.02;
	pt.y = (T_Height*CntN) + (T_Height*CntN) *0.15;
	m_ListC.SetItemPosition(i, pt);
	

	delete pImage;
	delete pThumbnail;

	m_ListC.SetRedraw();
}
