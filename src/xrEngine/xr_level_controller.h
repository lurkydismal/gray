#pragma once

enum EGameActions
{
    kLEFT,
    kRIGHT,
    kUP,
    kDOWN,
    kJUMP,
    kCROUCH,
    kACCEL,
    kSPRINT_TOGGLE,
                        
    kFWD,
    kBACK,
    kL_STRAFE,
    kR_STRAFE,
                        
    kL_LOOKOUT,
    kR_LOOKOUT,
                        
    kCAM_1,
    kCAM_2,
    kCAM_3,
    kCAM_ZOOM_IN,
    kCAM_ZOOM_OUT,
                        
    kTORCH,
    kNIGHT_VISION,
    kDETECTOR,
    kWPN_1,
    kWPN_2,
    kWPN_3,
    kWPN_4,
    kWPN_5,
    kWPN_6,
    kARTEFACT,
    kWPN_NEXT,
    kWPN_FIRE,
    kWPN_ZOOM,
    kWPN_ZOOM_INC,
    kWPN_ZOOM_DEC,
    kWPN_RELOAD,
    kWPN_FUNC,
    kWPN_FIREMODE_PREV,
    kWPN_FIREMODE_NEXT,
                        
    kPAUSE,
    kDROP,
    kUSE,
    kSCORES,
    kCHAT,
    kCHAT_TEAM, 
    kVOICE_CHAT,
    kVOICE_DISTANCE,
    kSCREENSHOT,
    kQUIT,
    kCONSOLE,
    kINVENTORY,
    kBUY,
    kSKIN,
    kTEAM,
    kACTIVE_JOBS,
                        
    kVOTE_BEGIN,
    kSHOW_ADMIN_MENU,
    kVOTE,
    kVOTEYES,
    kVOTENO,
                        
    kNEXT_SLOT,
    kPREV_SLOT,
                        
    kSPEECH_MENU_0,
    kSPEECH_MENU_1,
                        
    kQUICK_USE_1,
    kQUICK_USE_2,        
    kQUICK_USE_3,        
    kQUICK_USE_4,        
    
    kQUICK_SAVE,
    kQUICK_LOAD,
    kALIFE_CMD,

    kCUSTOM1,
    kCUSTOM2,
    kCUSTOM3,
    kCUSTOM4,
    kCUSTOM5,
    kCUSTOM6,
    kCUSTOM7,
    kCUSTOM8,
    kCUSTOM9,
    kCUSTOM10,
    kCUSTOM11,
    kCUSTOM12,
    kCUSTOM13,
    kCUSTOM14,
    kCUSTOM15,

    kCAM_AUTOAIM,

    kLASTACTION,
    kNOTBINDED,
    kFORCEDWORD        = u32(-1)
};

struct _keyboard        
{
    LPCSTR        key_name;
    int            dik;
    xr_string    key_local_name;
};
enum _key_group
{
    _both    =    (1<<0)            ,
    _sp        =    _both | (1<<1)    ,
    _mp        =    _both | (1<<2)    ,
};

extern ENGINE_API _key_group g_current_keygroup;

ENGINE_API bool is_group_not_conflicted(_key_group g1, _key_group g2);

struct _action
{
    LPCSTR            action_name;
    EGameActions    id;
    _key_group        key_group;
};

ENGINE_API LPCSTR            dik_to_keyname            (int _dik);
ENGINE_API int                keyname_to_dik            (LPCSTR _name);
ENGINE_API _keyboard*        keyname_to_ptr            (LPCSTR _name);
ENGINE_API _keyboard*        dik_to_ptr                (int _dik, bool bSafe);

ENGINE_API LPCSTR            id_to_action_name        (EGameActions _id);
ENGINE_API EGameActions    action_name_to_id        (LPCSTR _name);
ENGINE_API _action*        action_name_to_ptr        (LPCSTR _name);

extern ENGINE_API _action        actions        [];
//extern _keyboard    keyboards    [];
//extern xr_vector< _keyboard >    keyboards;

#define bindings_count kLASTACTION
struct _binding
{
    _action*        m_action;
    _keyboard*        m_keyboard[2];
};

extern ENGINE_API _binding g_key_bindings[];

ENGINE_API bool                is_binded            (EGameActions action_id, int dik);
ENGINE_API int                    get_action_dik        (EGameActions action_id, int idx=-1);
ENGINE_API EGameActions        get_binded_action    (int dik);

ENGINE_API void     CCC_RegisterInput();

struct _conCmd    
{
    shared_str    cmd;
};

class ENGINE_API ConsoleBindCmds
{
public:
    xr_map<int,_conCmd>        m_bindConsoleCmds;

    void     bind            (int dik, LPCSTR N);
    void     unbind            (int dik);
    bool     execute            (int dik);
    void     clear            ();
    void     save            (IWriter* F);
};

ENGINE_API void GetActionAllBinding    (LPCSTR action, char* dst_buff, int dst_buff_sz);

extern ENGINE_API ConsoleBindCmds bindConsoleCmds;

// 0xED - max vavue in DIK* enum
#define MOUSE_1        (0xED + 100)
#define MOUSE_2        (0xED + 101)
#define MOUSE_3        (0xED + 102)

#define MOUSE_4        (0xED + 103)
#define MOUSE_5        (0xED + 104)
#define MOUSE_6        (0xED + 105)
#define MOUSE_7        (0xED + 106)
#define MOUSE_8        (0xED + 107)
