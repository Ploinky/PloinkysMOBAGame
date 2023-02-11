

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0626 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for src\Launcher.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0626 
    protocol : all , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __Launcher_h_h__
#define __Launcher_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if _CONTROL_FLOW_GUARD_XFG
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
#endif

/* Forward Declarations */ 

#ifndef __ILauncher_FWD_DEFINED__
#define __ILauncher_FWD_DEFINED__
typedef interface ILauncher ILauncher;

#endif 	/* __ILauncher_FWD_DEFINED__ */


#ifndef __LauncherImpl_FWD_DEFINED__
#define __LauncherImpl_FWD_DEFINED__

#ifdef __cplusplus
typedef class LauncherImpl LauncherImpl;
#else
typedef struct LauncherImpl LauncherImpl;
#endif /* __cplusplus */

#endif 	/* __LauncherImpl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __LauncherLibrary_LIBRARY_DEFINED__
#define __LauncherLibrary_LIBRARY_DEFINED__

/* library LauncherLibrary */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_LauncherLibrary;

#ifndef __ILauncher_INTERFACE_DEFINED__
#define __ILauncher_INTERFACE_DEFINED__

/* interface ILauncher */
/* [local][object][uuid] */ 


EXTERN_C const IID IID_ILauncher;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3a14c9c0-bc3e-453f-a314-4ce4a0ec81d8")
    ILauncher : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ BSTR stringParameter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Quit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Minimize( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MouseDownDrag( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct ILauncherVtbl
    {
        BEGIN_INTERFACE
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILauncher * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILauncher * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILauncher * This);
        
        DECLSPEC_XFGVIRT(ILauncher, Connect)
        HRESULT ( STDMETHODCALLTYPE *Connect )( 
            ILauncher * This,
            /* [in] */ BSTR stringParameter);
        
        DECLSPEC_XFGVIRT(ILauncher, Quit)
        HRESULT ( STDMETHODCALLTYPE *Quit )( 
            ILauncher * This);
        
        DECLSPEC_XFGVIRT(ILauncher, Minimize)
        HRESULT ( STDMETHODCALLTYPE *Minimize )( 
            ILauncher * This);
        
        DECLSPEC_XFGVIRT(ILauncher, MouseDownDrag)
        HRESULT ( STDMETHODCALLTYPE *MouseDownDrag )( 
            ILauncher * This);
        
        END_INTERFACE
    } ILauncherVtbl;

    interface ILauncher
    {
        CONST_VTBL struct ILauncherVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILauncher_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ILauncher_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ILauncher_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ILauncher_Connect(This,stringParameter)	\
    ( (This)->lpVtbl -> Connect(This,stringParameter) ) 

#define ILauncher_Quit(This)	\
    ( (This)->lpVtbl -> Quit(This) ) 

#define ILauncher_Minimize(This)	\
    ( (This)->lpVtbl -> Minimize(This) ) 

#define ILauncher_MouseDownDrag(This)	\
    ( (This)->lpVtbl -> MouseDownDrag(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ILauncher_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_LauncherImpl;

#ifdef __cplusplus

class DECLSPEC_UUID("637abc45-11f7-4dde-84b4-317d62a638d3")
LauncherImpl;
#endif
#endif /* __LauncherLibrary_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


