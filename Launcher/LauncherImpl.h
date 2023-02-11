#pragma once

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <wrl.h>

#include <functional>
#include <map>
#include <string>
#include <wrl\client.h>

#include "Launcher_h.h"

class LauncherImpl : public Microsoft::WRL::RuntimeClass<
	Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
	ILauncher, IDispatch> {
public:
	typedef std::function<void(void)> Callback;
	typedef std::function<void(Callback)> RunCallbackAsync;

	LauncherImpl(RunCallbackAsync runCallbackAsync, HWND hWnd);

	// IHostObjectSample implementation
	STDMETHODIMP Connect(BSTR stringParameter) override;

	STDMETHODIMP Quit(void) override;

	STDMETHODIMP Minimize(void) override;

	STDMETHODIMP MouseDownDrag(void) override;

	// IDispatch implementation
	STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override;

	STDMETHODIMP GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo) override;

	STDMETHODIMP GetIDsOfNames(
		REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgDispId) override;

	STDMETHODIMP Invoke(
		DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams,
		VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr) override;


private:
	wil::com_ptr<IDispatch> m_callback;
	RunCallbackAsync m_runCallbackAsync;
	wil::com_ptr<ITypeLib> m_typeLib;
	HWND m_hWnd;
};