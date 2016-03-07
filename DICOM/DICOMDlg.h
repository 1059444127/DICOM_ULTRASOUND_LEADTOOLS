
// DICOMDlg.h : 헤더 파일
//

#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "DicomCreat.h"
#include	<vector>
// CDICOMDlg 대화 상자
__declspec(dllimport)	bool WINAPI	FindDir();
__declspec(dllimport)	CString WINAPI	AcqDir();
__declspec(dllimport)	bool WINAPI	SearchDir(CString &path, CString &dir);
__declspec(dllimport)	bool SDir(CString &dir, CTreeCtrl &m_Tree);

class CDICOMDlg : public CDialogEx
{
// 생성입니다.
public:
	CDICOMDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DICOM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	// TreeControl 변수
	CTreeCtrl m_TreeC;
	// ListControl 변수
	CListCtrl m_ListC;
	afx_msg void TreeDClk(NMHDR *pNMHDR, LRESULT *pResult);
	// cFindFolder 첫 실행 지점
	bool FindFolder();
	afx_msg void BitmapDCLK(NMHDR *pNMHDR, LRESULT *pResult);
	CString PathName;

	HTREEITEM BitmapPath; // List Control 에서 파일 주소를 얻기 위해서 사용.
	CString sBitmapPath;
	//Class 등록
	DicomCreat cDicomCreat;

	bool MousClip();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	CString m_LAbel;
	CString strFolderItem; // 최종적으로 bit 파일 경로를 얻어서 dicom create 만들시 사용
protected:
	/* 썸네일 사용하기 위한 변수*/
	CImageList				m_ImageListThumb;		// image list holding the thumbnails
	std::vector<CString>	m_VectorImageNames;		// vector holding the image names
	int						m_nSelectedItem;

public:
	void st_gplus();
	void ed_gplus();
	ULONG_PTR           gdiplusToken;

public:
	// List에 있는 파일을 불러오기
	bool ListCall(CString path);
	void ThumbnailF(CString path, int Cnt);
};
