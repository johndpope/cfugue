// dllmain.h : Declaration of module class.

class CMusicNoteDLLModule : public CAtlDllModuleT< CMusicNoteDLLModule >
{
public :
	DECLARE_LIBID(LIBID_MusicNoteDLLLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_MUSICNOTEDLL, "{F736F29E-2A42-4DA2-AFCF-223EABEBC23C}")
};

extern class CMusicNoteDLLModule _AtlModule;
