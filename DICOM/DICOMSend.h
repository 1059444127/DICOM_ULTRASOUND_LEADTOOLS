#pragma once
#include "DicomCreat.h"

#define STR_DONT_RECOMPRESS              0    // Leave image compression (Don't re-compress)
#define STR_J2K_LOSSY                    1    // J2K Lossy
#define STR_J2KLOSSLESS                  2    // J2K Lossless
#define STR_JPEG_LOSSY                   3    // JPEG Lossy
#define STR_JPEG_LOSSLESS                4    // JPEG Lossless

// Status
#define STR_CONNECT_FAILED                  1    // Connect operation failed
#define STR_CONNECT_SUCCEEDED               2    // Connect operation succeeded
#define STR_SEND_ASSOCIATION_REQUEST        3    // Sending assocaition request
#define STR_RECEIVE_ASSOCIATE_ACCEPT        4    // Receiving association accept
#define STR_RECEIVE_ASSOCIATE_REJECT        5    // Receiving association reject
#define STR_ABSTRACT_SYNTAX_NOT_SUPPORTED   6    // Abstract Syntax not supported
#define STR_SEND_CSTORE_REQUEST             7    // Sending C-STORE request
#define STR_RECEIVE_CSTORE_RESPONSE         8    // Receiving C-STORE response
#define STR_CONNECTION_CLOSED               9    // Closing connection
#define STR_PROCESS_TERMINATED              10   // Storage has been terminated   
#define STR_SEND_RELEASE_REQUEST            11   // Sending release request
#define STR_RECEIVE_RELEASE_RESPONSE        12   // Receiving release response
#define STR_DICOM_ERROR                     13   // General DICOM Error


#define MAX_STR_LEN 1024
#define SIZEINWORD(p) sizeof(p)/sizeof(L_TCHAR)
#define IMPLEMENTATION_VERSION_NAME   TEXT("1")
#define IMPLEMENTATION_CLASS_UID      TEXT("1.2.840.114257.1123456")

typedef struct CURFILE
{
	L_TCHAR szFilename[MAX_PATH];
	CURFILE* pNextFile;

} CURFILE, *pCURFILE;


class DICOMSend :
	public DicomCreat, LDicomNet
{
protected:
	pCURFILE m_pPosFile;

private:
	pCURFILE m_pFiles;
public:
	DICOMSend(L_TCHAR* pszPath = NULL, L_UINT32 uMode = DICOM_SECURE_NONE);
	~DICOMSend();
	// 보낼 DICOM PATH 작성
	bool DCMFilePath(CString *FilePath, CString *infoPath);
	virtual BOOL OnInitDialog();
	bool StartSending(CString Filepath);

	CString DCMPath;


	// Client
	L_VOID OnConnect();
	L_VOID OnStatus(L_UINT uStatus, L_INT nErrorCode);
	//L_VOID SendAssociateR();
	L_VOID OnReceiveAssociateAccept(LDicomAssociate *pPDU);
	L_VOID TerminateStorage();
	L_VOID CloseConnection();
	L_VOID BuildAssociation();
	//
	L_TCHAR   m_szStorageClass[128];
	L_TCHAR   m_szStorageInstance[128];
	LDicomDS m_DicomDS;
	L_TCHAR*  m_pszClientName;
	L_TCHAR*  m_pszServerName;
	L_UINT    m_uCompression;
	L_UINT m_uPresentationContextType;
	L_INT nError;


	CStdioFile DCMSendFile; // PACS에 보낼 파일정보를 저장하기 위하여 생성된 변수.
	CString sServerip;
	CString sServerport;
	CString sServerAE;
	CString sUserAE;


};

