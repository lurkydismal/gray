// EngineAPI.h: interface for the CEngineAPI class.
//
//****************************************************************************
// Support for extension DLLs
//****************************************************************************
#pragma once
#include <array>

enum class APILevel;

// Abstract 'Pure' class for DLL interface
class ENGINE_API DLL_Pure {
public:
    CLASS_ID                CLS_ID;

    DLL_Pure(void *params)    {CLS_ID=0; };
    DLL_Pure()                {CLS_ID=0; };
    virtual    DLL_Pure*        _construct        ()    { return this;     }
    virtual ~DLL_Pure()        {};
};

// Class creation/destroying interface
extern "C" 
{
typedef  DLL_Pure*      __cdecl Factory_Create    (CLASS_ID    CLS_ID);
typedef  void          __cdecl Factory_Destroy    (DLL_Pure*    O);
};

enum class EditorUI : u8
{
    Shaders,
    Weather,
    DebugDraw,
    CmdVars,
    CmdConsole,
    LuaDebug,
    LuaCodespace,
    ActorInfos,
    HudAdjust,
    ScenesViewer,
    CameraEffectors,
    Game_TimeManager,
    Game_SpawnManager,
    Game_WeaponManager,
    Game_SearchManager,
    Count
};

class ENGINE_API CEngineAPI
{
private:
    HMODULE hGame;
    HMODULE hRender;

public:
    HMODULE hGameSpy;

    Factory_Create* pCreate;
    Factory_Destroy* pDestroy;

    std::array<bool, static_cast<u8>(EditorUI::Count)> EditorStates = {};

public:
    CEngineAPI();
    ~CEngineAPI();

public:
    void                Initialize    ();
    void                InitializeNotDedicated();
    void                Destroy        ();

    void                CreateRendererList();
    APILevel            GetAPI();
    int                    GetSkinningMode() const;
    void                SetSkinningMode(int Mode = -1);
};

#define NEW_INSTANCE(a)        Engine.External.pCreate(a)
#define DEL_INSTANCE(a)        { Engine.External.pDestroy(a); a=NULL; }
