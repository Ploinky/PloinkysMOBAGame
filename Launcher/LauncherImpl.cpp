#include "LauncherImpl.h"

#include <comutil.h>
#pragma comment(lib,"comsuppw.lib")
LauncherImpl::LauncherImpl(LauncherImpl::RunCallbackAsync runCallbackAsync, HWND hWnd)
	: m_runCallbackAsync(runCallbackAsync), m_hWnd(hWnd)
{
}
std::string bstr_to_str(BSTR source) {
	//source = L"lol2inside";
	_bstr_t wrapped_bstr = _bstr_t(source);
	int length = wrapped_bstr.length();
	char* char_array = new char[length];
	strcpy_s(char_array, length + 1, wrapped_bstr);
	return char_array;
}


STDMETHODIMP LauncherImpl::Connect(BSTR stringParameter) {
	std::string cmd = std::string("start Client.exe").append(" -connect ").append(bstr_to_str(stringParameter));
	system(cmd.c_str());
	return S_OK;
}

STDMETHODIMP LauncherImpl::Minimize() {
	PostMessage(m_hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	return S_OK;
}

STDMETHODIMP LauncherImpl::MouseDownDrag() {
	ReleaseCapture();
	SendMessageW(m_hWnd, WM_NCLBUTTONDOWN, 0x2, 0);
	return S_OK;
};


STDMETHODIMP LauncherImpl::Quit() {
	PostQuitMessage(0);
	return S_OK;
}

STDMETHODIMP LauncherImpl::GetTypeInfoCount(UINT* pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}

STDMETHODIMP LauncherImpl::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
{
	if (0 != iTInfo)
	{
		return TYPE_E_ELEMENTNOTFOUND;
	}
	if (!m_typeLib)
	{
		RETURN_IF_FAILED(LoadTypeLib(L"Launcher.tlb", &m_typeLib));
	}
	return m_typeLib->GetTypeInfoOfGuid(__uuidof(ILauncher), ppTInfo);
}

STDMETHODIMP LauncherImpl::GetIDsOfNames(
	REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId)
{
	wil::com_ptr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->GetIDsOfNames(rgszNames, cNames, rgDispId);
}

STDMETHODIMP LauncherImpl::Invoke(
	DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
	VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
{
	wil::com_ptr<ITypeInfo> typeInfo;
	RETURN_IF_FAILED(GetTypeInfo(0, lcid, &typeInfo));
	return typeInfo->Invoke(
		this, dispIdMember, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr);
}