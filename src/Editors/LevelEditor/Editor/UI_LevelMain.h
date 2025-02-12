#pragma once

enum ELECommand
{
    COMMAND_EXTFIRST_EXT = COMMAND_MAIN_LAST-1,

    COMMAND_CHANGE_TARGET,
    COMMAND_ENABLE_TARGET,
    COMMAND_READONLY_TARGET,
    COMMAND_SHOW_TARGET,

    COMMAND_SHOW_OBJECTLIST,
    COMMAND_MULTI_RENAME_OBJECTS,

    COMMAND_REFRESH_SOUND_ENVS,
    COMMAND_REFRESH_SOUND_ENV_GEOMETRY,
    
    COMMAND_CLEAN_LIBRARY,
    COMMAND_LIBRARY_EDITOR,
    COMMAND_LANIM_EDITOR,
    COMMAND_FILE_MENU,
    COMMAND_CLEAR_DEBUG_DRAW,
    COMMAND_IMPORT_COMPILER_ERROR,
    COMMAND_IMPORT_AICOMPILER_ERROR,
    COMMAND_EXPORT_COMPILER_ERROR,
    COMMAND_VALIDATE_SCENE,
    COMMAND_RELOAD_OBJECTS,

    COMMAND_CUT,
    COMMAND_COPY,
    COMMAND_PASTE,
    COMMAND_DUPLICATE,
    COMMAND_LOAD_SELECTION,
    COMMAND_SAVE_SELECTION,
    COMMAND_LOAD_LEVEL_PART, 
    COMMAND_UNLOAD_LEVEL_PART,   
    
    COMMAND_CLEAR_SCENE_SUMMARY,
    COMMAND_COLLECT_SCENE_SUMMARY,
    COMMAND_SHOW_SCENE_SUMMARY,            
    COMMAND_EXPORT_SCENE_SUMMARY,
    COMMAND_SCENE_HIGHLIGHT_TEXTURE,

    COMMAND_OPTIONS,
    COMMAND_BUILD,

    COMMAND_MAKE_GAME,
    COMMAND_MAKE_PUDDLES,
    COMMAND_MAKE_DETAILS,
    COMMAND_MAKE_HOM,
    COMMAND_MAKE_SOM,
    COMMAND_MAKE_AIMAP,

    COMMAND_MOVE_GIZMO,
    COMMAND_UPDATE_GIZMO,

    COMMAND_INVERT_SELECTION_ALL,
    COMMAND_SELECT_ALL,
    COMMAND_DESELECT_ALL,
    COMMAND_DELETE_SELECTION,
    COMMAND_HIDE_UNSEL,
    COMMAND_HIDE_SEL,
    COMMAND_HIDE_ALL,
    COMMAND_LOCK_ALL,
    COMMAND_LOCK_SEL,
    COMMAND_LOCK_UNSEL,

    COMMAND_SET_SNAP_OBJECTS,
    COMMAND_ADD_SEL_SNAP_OBJECTS,
    COMMAND_DEL_SEL_SNAP_OBJECTS,
    COMMAND_CLEAR_SNAP_OBJECTS,
    COMMAND_SELECT_SNAP_OBJECTS,
    COMMAND_REFRESH_SNAP_OBJECTS,

    COMMAND_LOAD_FIRSTRECENT,

    COMMAND_SHOWCONTEXTMENU,
    COMMAND_SHOW_CLIP_EDITOR,

    COMMAND_MULTI_REPLACE_OBJECTS,

    COMMAND_LE_END
};

class CLevelMain: public TUI{
    typedef TUI inherited;
    
    virtual void     RealUpdateScene            ();
    virtual void     RealQuit                ();
public:
    C3DCursor*       m_Cursor;
public:
                    CLevelMain                 ();
    virtual         ~CLevelMain                ();


    virtual LPSTR    GetCaption                ();

    virtual void     ResetStatus                ();
    virtual void     SetStatus                (LPCSTR s, bool bOutLog=true);
    virtual void    ProgressDraw            ();

    virtual LPCSTR    EditorName                (){return "level";}
    virtual LPCSTR    EditorDesc                (){return "Level Editor";}

    void             ShowContextMenu            (int cls);
    bool             PickGround                (Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap=1, Fvector* hitnormal=0);
    bool             SelectionFrustum        (CFrustum& frustum);

    virtual bool     ApplyShortCut            (DWORD Key, TShiftState Shift);
    virtual bool     ApplyGlobalShortCut        (DWORD Key, TShiftState Shift);

    // commands
    virtual    void    RegisterCommands        (); 

    virtual void    SaveSettings            (nlohmann::json&);
    virtual void    LoadSettings            (nlohmann::json&);
    virtual Ivector2 GetRenderMousePosition()const;
    virtual void OnDrawUI();
    virtual bool  KeyDown(WORD Key, TShiftState Shift) override;
    virtual void    OnStats(CGameFont* font);

    virtual bool IsPlayInEditor();
private:
};    
extern CLevelMain*    LUI;