
// DICOMDlg.h : ��� ����
//

#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "DicomCreat.h"
#include	<vector>
// CDICOMDlg ��ȭ ����
__declspec(dllimport)	bool WINAPI	FindDir();
__declspec(dllimport)	CString WINAPI	AcqDir();
__declspec(dllimport)	bool WINAPI	SearchDir(CString &path, CString &dir);
__declspec(dllimport)	bool SDir(CString &dir, CTreeCtrl &m_Tree);

class CDICOMDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CDICOMDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DICOM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	// TreeControl ����
	CTreeCtrl m_TreeC;
	// ListControl ����
	CListCtrl m_ListC;
	afx_msg void TreeDClk(NMHDR *pNMHDR, LRESULT *pResult);
	// cFindFolder ù ���� ����
	bool FindFolder();
	afx_msg void BitmapDCLK(NMHDR *pNMHDR, LRESULT *pResult);
	CString PathName;

	HTREEITEM BitmapPath; // List Control ���� ���� �ּҸ� ��� ���ؼ� ���.
	CString sBitmapPath;
	//Class ���
	DicomCreat cDicomCreat;

	bool MousClip();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	CString m_LAbel;
	CString strFolderItem; // ���������� bit ���� ��θ� �� dicom create ����� ���
protected:
	/* ����� ����ϱ� ���� ����*/
	CImageList				m_ImageListThumb;		// image list holding the thumbnails
	std::vector<CString>	m_VectorImageNames;		// vector holding the image names
	int						m_nSelectedItem;

public:
	void st_gplus();
	void ed_gplus();
	ULONG_PTR           gdiplusToken;

public:
	// List�� �ִ� ������ �ҷ�����
	bool ListCall(CString path);
	void ThumbnailF(CString path, int Cnt);
};
