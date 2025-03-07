#pragma once

//new
enum EGameIDs {
        eGameIDNoGame                                   = u32(0),
        eGameIDSingle                                   = u32(1) << 0,
        eGameIDDeathmatch                               = u32(1) << 1,
        eGameIDTeamDeathmatch                           = u32(1) << 2,
        eGameIDArtefactHunt                             = u32(1) << 3,
        eGameIDCaptureTheArtefact                       = u32(1) << 4,
        eGameIDDominationZone                           = u32(1) << 5,
        eGameIDTeamDominationZone                       = u32(1) << 6,
        eGameIDFreeMP                                   = u32(1) << 7,
};

class PropValue;
class PropItem;

using PropItemVec = xr_vector<PropItem*>;
using PropItemIt = PropItemVec::iterator;

struct GameTypeChooser
{
    Flags16    m_GameType;
#ifndef XRGAME_EXPORTS
        void    FillProp        (LPCSTR pref, PropItemVec& items);
#endif // #ifndef XRGAME_EXPORTS

    bool     LoadStream        (IReader&F);
    bool     LoadLTX            (CInifile& ini, LPCSTR sect_name, bool bOldFormat);
    void     SaveStream        (IWriter&);
    void     SaveLTX            (CInifile& ini, LPCSTR sect_name);
    void    SetDefaults        ()                {m_GameType.one();}
    bool    MatchType        (const u16 t) const        {return (t==eGameIDNoGame) || !!m_GameType.test(t);};
};