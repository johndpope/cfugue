// dllmain.h : Declaration of module class.



/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __CFugueDLLLib_LIBRARY_DEFINED__
#define __CFugueDLLLib_LIBRARY_DEFINED__

/* library CFugueDLLLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MusicNoteDLLLib;
#endif /* __CFugueDLLLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

class CMusicNoteDLLModule : public CAtlDllModuleT< CMusicNoteDLLModule >
{
public :
	DECLARE_LIBID(LIBID_MusicNoteDLLLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MUSICNOTEDLL, "{F736F29E-2A42-4DA2-AFCF-223EABEBC23C}")
};

extern class CMusicNoteDLLModule _AtlModule;
