#pragma once

class CBlender_accum_point : public IBlender  
{
public:
    virtual        LPCSTR        getComment()    { return "INTERNAL: accumulate point light";    }
    virtual        BOOL        canBeDetailed()    { return FALSE;    }
    virtual        BOOL        canBeLMAPped()    { return FALSE;    }

    virtual        void        Compile            (CBlender_Compile& C);

    CBlender_accum_point();
    virtual ~CBlender_accum_point();
};
