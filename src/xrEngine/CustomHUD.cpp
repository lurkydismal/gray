#include "stdafx.h"
#include "CustomHUD.h"

Flags32 psHUD_Flags = {HUD_CROSSHAIR_RT|HUD_WEAPON_RT|HUD_WEAPON_RT2|HUD_CROSSHAIR_DYNAMIC|HUD_CROSSHAIR_RT2|HUD_DRAW_RT|HUD_DRAW_RT2};

ENGINE_API CCustomHUD* g_hud = nullptr;

CCustomHUD::CCustomHUD()
{
    g_hud = this;
    Device.seqResolutionChanged.Add(this);
}

CCustomHUD::~CCustomHUD()
{
    g_hud = nullptr;
    Device.seqResolutionChanged.Remove(this);
}

