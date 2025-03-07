#pragma once

enum KILL_TYPE{
    KT_HIT = 0,
    KT_BLEEDING,
    KT_RADIATION,
    KT_FORCEDWORD = u32(-1)
};

enum SPECIAL_KILL_TYPE
{
    SKT_NONE = 0,
    SKT_HEADSHOT,
    SKT_BACKSTAB,
    
    SKT_KNIFEKILL,
    SKT_PDA,

    SKT_KIR,    //Kill in Row

    SKT_NEWRANK,
    SKT_EYESHOT,

    SKT_FORCEDWORD = u32(-1)
};

enum KILL_RES 
{
    KR_NONE = 0,
    KR_SELF,
    KR_TEAMMATE,
    KR_TEAMMATE_CRITICAL,
    KR_RIVAL,
    KR_RIVAL_CRITICAL,

    KR_FORCEDWORD    = u32 (-1)
};
