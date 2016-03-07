#include "stdafx.h"
#include "DICOMSend.h"


DICOMSend::DICOMSend(L_TCHAR* pszPath, L_UINT32 nMode) : LDicomNet(pszPath, nMode)
{
	lstrcpy(m_szStorageClass, TEXT(""));

}


DICOMSend::~DICOMSend()
{
}


// 보낼 DICOM PATH 작성
bool DICOMSend::DCMFilePath(CString *FilePath, CString *infoPath)
{
	DCMPath = *FilePath;
	if (!DCMSendFile.Open(*infoPath, CFile::modeReadWrite))
	{
		AfxMessageBox(_T("PACS 정보가 없습니다."));
	}
	else
	{
		CString strn;
		DCMSendFile.ReadString(strn);
		
		int num2 = strn.Find(_T("_2_"));
		int num3 = strn.Find(_T("_3_"));
		int num4 = strn.Find(_T("_4_"));
		int len = strn.GetLength();


		sServerip = strn.Mid(3, num2 - 3);
		sServerport = strn.Mid(num2 + 3, num3 - num2 - 3);
		sServerAE = strn.Mid(num3 + 3, num4 - num3 - 3);
		sUserAE = strn.Mid(num4 + 3, len - num4 + 3 + 1);

	}
	StartSending(DCMPath);
	return TRUE;
}

bool DICOMSend::StartSending(CString Filepath)
{
	L_INT ret;
	// Do the connection to the server   
	ret = LDicomNet::StartUp();
	if (ret = !DICOM_SUCCESS)
	{
		if (ret == 1)
		{
			AfxMessageBox(_T("Leadtools 지원팀 연락"));
		}
		else
		{
			AfxMessageBox(_T("연결에 실패 하였습니다.(StartUp)"));
		}
	}

	m_pPosFile = m_pFiles;


	//ret = Connect(NULL, 0, _T("10.10.103.55"), 104);
	
	ret = Connect(NULL, 0, (TCHAR*)(LPCTSTR)sServerip, _ttoi(sServerport));

	if (ret = !DICOM_SUCCESS)
	{
		if (ret == 1)
		{
			AfxMessageBox(_T("Leadtools 지원팀 연락"));
		}
		else
		{
			AfxMessageBox(_T("연결에 실패 하였습니다.(CONNECT)"));
		}
	}
	//LDICOMNET:OnConnect

	//L_INT nError = 0; // Onconnect에서 1로 둔 이유는 초기화할때 부터 성공적이라고 하면 분명히 오작동이 일어나는 것을 방지.
	//OnConnect(nError);
	OnConnect();

	return TRUE;
}

L_VOID DICOMSend::OnConnect()
{
	//if (nError != DICOM_SUCCESS)
	//{
	//	// Failed connection
	//	OnStatus(STR_CONNECT_FAILED, nError);
	//	TerminateStorage();
	//	return;
	//}

	// Succeeded connection
	OnStatus(STR_CONNECT_SUCCEEDED, 0);

	// Build the association for the first selected file   
	BuildAssociation();
}

L_VOID DICOMSend::OnReceiveAssociateAccept(LDicomAssociate *pPDU)
{
	L_UCHAR uPresID;
	L_TCHAR pszReturnTrans[MAX_STR_LEN];


	// Server accept the association
	OnStatus(STR_RECEIVE_ASSOCIATE_ACCEPT, 0);

	// Check if the sent abstract syntax is accepted
	uPresID = pPDU->FindAbstract(m_szStorageClass);
	while ((uPresID != 0) && (pPDU->GetResult(uPresID) != PDU_ACCEPT_RESULT_SUCCESS))
	{
		uPresID = pPDU->FindNextAbstract(uPresID, m_szStorageClass);
	}

	if (uPresID == 0)
	{
		// The abstract syntax is NOT supported
		OnStatus(STR_ABSTRACT_SYNTAX_NOT_SUPPORTED, 0);
		TerminateStorage();
		return;
	}

	// Get the accepted transfer syntax
	pPDU->GetTransfer(uPresID, 0, pszReturnTrans, SIZEINWORD(pszReturnTrans));

	L_INT nRet;
	// Change the transfer syntax to the accepted one
	nRet = m_DicomDS.ChangeTransferSyntax(pszReturnTrans, 2, 0);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	// Sending the C-Store request
	OnStatus(STR_SEND_CSTORE_REQUEST, 0);

	nRet = SendCStoreRequest(uPresID, 1, m_szStorageClass, m_szStorageInstance,
		COMMAND_PRIORITY_MEDIUM, TEXT(""), 0, &m_DicomDS);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		AfxMessageBox(_T("실패"));

	}
}

// Terminate the storage process
L_VOID DICOMSend::TerminateStorage()
{
	if (IsConnected())
	{
		if (IsAssociated())
		{
			SendAbort(PDU_ABORT_SOURCE_USER, 0);
		}

		// Close the connection
		CloseConnection();
	}

	OnStatus(STR_PROCESS_TERMINATED, 0);
}

L_VOID DICOMSend::BuildAssociation()
{
	// Build the association for the C-Store
	LDicomAssociate StoreAssociate(TRUE);

	// Send the name of the current file
	L_INT nRet;

	// Set the version
	nRet = StoreAssociate.SetVersion(1);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	// Set called application title
	m_pszServerName = (TCHAR*)(LPCTSTR)sServerAE;
	//m_pszServerName = _T("LEAD_SERVER");
	nRet = StoreAssociate.SetCalled(m_pszServerName ? m_pszServerName : TEXT(""));
	if (nRet != 0)
	{
		AfxMessageBox(_T("Sever AE Disable"));
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	// Set calling application title
	//m_pszClientName = _T("HYUNWOOO86");
	m_pszClientName = (TCHAR*)(LPCTSTR)sUserAE;
	nRet = StoreAssociate.SetCalling(m_pszClientName ? m_pszClientName : TEXT(""));
	if (nRet != 0)
	{
		AfxMessageBox(_T("USER AE Disable"));
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	// Set maximum length
	nRet = StoreAssociate.SetMaxLength(TRUE, 46726);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	pDICOMELEMENT pDicomElement = NULL;

	nRet = m_DicomDS.LoadDS((TCHAR*)(LPCTSTR)DCMPath, 0);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	L_TCHAR* pszSOPClassUID = NULL;
	pDicomElement = m_DicomDS.FindFirstElement(NULL, TAG_MEDIA_STORAGE_SOP_CLASS_UID, FALSE);
	if (pDicomElement)
		pszSOPClassUID = m_DicomDS.GetStringValue(pDicomElement, 0, 1);

	if (lstrlen(pszSOPClassUID) == 0)
	{
		pDicomElement = m_DicomDS.FindFirstElement(NULL, TAG_SOP_CLASS_UID, FALSE);
		if (pDicomElement)
			pszSOPClassUID = m_DicomDS.GetStringValue(pDicomElement, 0, 1);
	}

	if (lstrlen(pszSOPClassUID) == 0)
		pszSOPClassUID = TEXT("1.1.1.1");

	lstrcpyn(m_szStorageClass, pszSOPClassUID, sizeof(m_szStorageClass));

	L_TCHAR* pszText = NULL;
	pDicomElement = m_DicomDS.FindFirstElement(NULL, TAG_SOP_INSTANCE_UID, FALSE);
	if (pDicomElement != NULL)
		pszText = m_DicomDS.GetStringValue(pDicomElement, 0, 1);
	if (lstrlen(pszText) == 0)
		pszText = TEXT("998.998.1.19950214.94000.1.102");

	lstrcpyn(m_szStorageInstance, pszText, sizeof(m_szStorageInstance));



	L_UCHAR nPresID = 1;
	L_TCHAR* pszTransfer = NULL;
	L_TCHAR* pszDefTransfer = UID_IMPLICIT_VR_LITTLE_ENDIAN; // Default transfer syntax


	pDicomElement = m_DicomDS.FindFirstElement(NULL, TAG_TRANSFER_SYNTAX_UID, FALSE);
	if (pDicomElement != NULL)
	{
		// Check if this transfer syntax is as the default transfere syntax
		pszTransfer = m_DicomDS.GetStringValue(pDicomElement, 0, 1);
	}


	// If not the default, add the transfer syntax for the storage service class
	if (lstrcmp(pszTransfer, pszDefTransfer) != 0)
	{
		nRet = StoreAssociate.AddPresentation(nPresID, 0, m_szStorageClass);
		if (nRet != 0)
		{
			OnStatus(STR_DICOM_ERROR, nRet);
			TerminateStorage();
			return;
		}

		nRet = StoreAssociate.AddTransfer(nPresID, pszTransfer);
		if (nRet != 0)
		{
			OnStatus(STR_DICOM_ERROR, nRet);
			TerminateStorage();
			return;
		}

		if (m_uPresentationContextType == 1)
		{
			nPresID = 3;
		}
	}

	// Create a new presenetation context if necessary
	if ((m_uPresentationContextType == 1) || (StoreAssociate.GetPresentationCount() == 0))
	{
		nRet = StoreAssociate.AddPresentation(nPresID, 0, m_szStorageClass);
		if (nRet != 0)
		{
			OnStatus(STR_DICOM_ERROR, nRet);
			TerminateStorage();
			return;
		}
	}

	// Add the default transfer syntax
	nRet = StoreAssociate.AddTransfer(nPresID, pszDefTransfer);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	// Add the Implementation Version 
	nRet = StoreAssociate.SetImplementVersion(TRUE, IMPLEMENTATION_VERSION_NAME);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	// Add the Implementation Class
	nRet = StoreAssociate.SetImplementClass(TRUE, IMPLEMENTATION_CLASS_UID);
	if (nRet != 0)
	{
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
		return;
	}

	// Sending the associate request
	OnStatus(STR_SEND_ASSOCIATION_REQUEST, 0);

	nRet = SendAssociateRequest(&StoreAssociate);
	if (nRet != 0)
	{
		// Error sending the association request
		OnStatus(STR_DICOM_ERROR, nRet);
		TerminateStorage();
	}
	else
	{
		AfxMessageBox(_T("성공적으로 보냈습니다."));

		CloseConnection();


	}

}


// Close the connection
L_VOID DICOMSend::CloseConnection()
{
	Close();

	::DeleteFile(DCMPath);
	if (DeleteFile(DCMPath) == TRUE)
	{
		AfxMessageBox(_T("파일 삭제 성공"));
	}

	OnStatus(STR_CONNECTION_CLOSED, 0);
}

L_VOID DICOMSend::OnStatus(L_UINT uStatus, L_INT nErrorCode)
{
	UNREFERENCED_PARAMETER(uStatus);
	UNREFERENCED_PARAMETER(nErrorCode);
}


BOOL DICOMSend::OnInitDialog()
{
	DicomCreat::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



