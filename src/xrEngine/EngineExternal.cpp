#include "stdafx.h"
#include <magic_enum/magic_enum.hpp>

ENGINE_API CEngineExternal* g_pEngineExternal = nullptr;

CEngineExternal::CEngineExternal()
{
    string_path fname;
    FS.update_path(fname, "$game_config$", "engine_external.ltx");
    pOptions = new CInifile(fname);
}

CEngineExternal::~CEngineExternal() {
    xr_delete(pOptions);
}

xr_string CEngineExternal::GetTitle() const {
    return READ_IF_EXISTS(pOptions, r_string_wb, "general", "title", "IX-Ray Platform").c_str();
}

const char* CEngineExternal::GetPlayerHudOmfAdditional() const
{
    return READ_IF_EXISTS(pOptions, r_string_wb, "player_hud", "PlayerHudOmfAdditional", "").c_str();
}

bool CEngineExternal::operator[](const EEngineExternalUI& ID) const
{
    return READ_IF_EXISTS(pOptions, r_bool, "ui", magic_enum::enum_name(ID).data(), false);
}

bool CEngineExternal::operator[](const EEngineExternalPhysical& ID) const
{
    return READ_IF_EXISTS(pOptions, r_bool, "physics", magic_enum::enum_name(ID).data(), false);
}

bool CEngineExternal::operator[](const EEngineExternalGame& ID) const
{
    return READ_IF_EXISTS(pOptions, r_bool, "gameplay", magic_enum::enum_name(ID).data(), false);
}

bool CEngineExternal::operator[](const EEngineExternalRender& ID) const
{
    return READ_IF_EXISTS(pOptions, r_bool, "render", magic_enum::enum_name(ID).data(), false);
}

bool CEngineExternal::operator[](const EEngineExternalEnvironment& ID) const {
    return READ_IF_EXISTS(pOptions, r_bool, "environment", magic_enum::enum_name(ID).data(), false);
}

ENGINE_API CEngineExternal& EngineExternal() {
    if (g_pEngineExternal == nullptr) {
        g_pEngineExternal = new CEngineExternal;
    }
    return *g_pEngineExternal;
}
