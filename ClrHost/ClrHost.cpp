// AccClientLaunch.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ClrHost.h"

void ShowError(LPCTSTR context, DWORD errorCode)
{
	CAtlString msg;
	msg.Format(L"Error context: %s\nError code: %d\nError: %s",
		context, errorCode, _com_error(errorCode).ErrorMessage()
	);
	MessageBox(nullptr, msg, _T("Error"), MB_OK | MB_ICONHAND);
}

HRESULT HostInClr(LPCTSTR executable)
{
	CComPtr<ICLRMetaHost> pMetaHost;
	HRESULT hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID*)&pMetaHost);
	if (FAILED(hr))
	{
		ShowError(_T("Create meta-host"), GetLastError());
		return hr;
	}

	CComPtr<ICLRRuntimeInfo> pRuntimeInfo;
	hr = pMetaHost->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, reinterpret_cast<LPVOID*>(&pRuntimeInfo));
	if (FAILED(hr))
	{
		ShowError(_T("Create runtime-info"), GetLastError());
		return hr;
	}

	CComPtr<ICorRuntimeHost> pRuntimeHost;
	hr = pRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, reinterpret_cast<LPVOID*>(&pRuntimeHost));
	if (FAILED(hr))
	{
		ShowError(_T("Create runtime-host"), GetLastError());
		return hr;
	}

	hr = pRuntimeHost->Start();
	if (FAILED(hr))
	{
		ShowError(_T("Start runtime-host"), GetLastError());
		return hr;
	}

	CComPtr<IUnknown> pUnknown;
	hr = pRuntimeHost->GetDefaultDomain(&pUnknown);
	if (FAILED(hr))
	{
		ShowError(_T("Get default-app-domain"), GetLastError());
		return hr;
	}

	CComPtr<mscorlib::_AppDomain> appDomainPtr;
	hr = pUnknown->QueryInterface(&appDomainPtr.p);
	if (FAILED(hr))
	{
		ShowError(_T("Get app-domain-ptr"), GetLastError());
		return hr;
	}

	LONG dwRetVal;
	CComBSTR bsExe(executable);
	hr = appDomainPtr->ExecuteAssembly(bsExe, nullptr, &dwRetVal);
	if (FAILED(hr))
	{
		ShowError(L"Execute", GetLastError());
		return hr;
	}

	hr = pRuntimeHost->Stop();
	if (FAILED(hr))
	{
		ShowError(_T("Stop runtime-host"), GetLastError());
		return hr;
	}

	return S_OK;
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (lpCmdLine == nullptr)
	{
		MessageBox(0, L"Syntex: AccClientLaunch executable.exe", L"Usage", MB_OK);
		return 1;
	}
	
	HostInClr(lpCmdLine);
	CoUninitialize();

	return ERROR_SUCCESS;
}