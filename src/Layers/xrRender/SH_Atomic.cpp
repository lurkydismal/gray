#include "stdafx.h"
#pragma hdrstop

#include "SH_Atomic.h"
#include "ResourceManager.h"

#include "dxRenderDeviceRender.h"

// Atomic
//SVS::~SVS                                ()            {    _RELEASE(vs);        dxRenderDeviceRender::Instance().Resources->_DeleteVS            (this);    }
//SPS::~SPS                                ()            {    _RELEASE(ps);        dxRenderDeviceRender::Instance().Resources->_DeletePS            (this);    }
//SState::~SState                            ()            {    _RELEASE(state);    dxRenderDeviceRender::Instance().Resources->_DeleteState        (this);    }
//SDeclaration::~SDeclaration                ()            {    _RELEASE(dcl);        dxRenderDeviceRender::Instance().Resources->_DeleteDecl        (this);    }

///////////////////////////////////////////////////////////////////////
//    SVS
SVS::SVS() :
    vs(0)
#ifdef USE_DX11
//    ,signature(0)
#endif //USE_DX11
{
    ;
}


SVS::~SVS()
{
    DEV->_DeleteVS(this);
#ifdef USE_DX11
    //_RELEASE(signature);
    //    Now it is release automatically
#endif //USE_DX11
    _RELEASE(vs);
}


///////////////////////////////////////////////////////////////////////
//    SPS
SPS::~SPS                                ()            {    _RELEASE(ps);        DEV->_DeletePS            (this);    }

#ifdef USE_DX11
///////////////////////////////////////////////////////////////////////
//    SGS
SGS::~SGS                                ()            {    _RELEASE(gs);        DEV->_DeleteGS            (this);    }
SHS::~SHS                                ()            {    _RELEASE(sh);        DEV->_DeleteHS            (this);    }
SDS::~SDS                                ()            {    _RELEASE(sh);        DEV->_DeleteDS            (this);    }
SCS::~SCS                                ()            {    _RELEASE(sh);        DEV->_DeleteCS            (this);    }

///////////////////////////////////////////////////////////////////////
//    SInputSignature
SInputSignature::SInputSignature(ID3DBlob* pBlob)    { VERIFY(pBlob); signature=pBlob; signature->AddRef();};
SInputSignature::~SInputSignature        ()            {    _RELEASE(signature); DEV->_DeleteInputSignature(this); }
#endif //USE_DX11

///////////////////////////////////////////////////////////////////////
//    SState
SState::~SState                            ()            {    _RELEASE(state);    DEV->_DeleteState        (this);    }

///////////////////////////////////////////////////////////////////////
//    SDeclaration
SDeclaration::~SDeclaration()
{    
    DEV->_DeleteDecl(this);    
#ifdef USE_DX11
    xr_map<ID3DBlob*, ID3DInputLayout*>::iterator iLayout;
    iLayout = vs_to_layout.begin();
    for( ; iLayout != vs_to_layout.end(); ++iLayout)
    {
        //    Release vertex layout
        _RELEASE(iLayout->second);
    }
#else //USE_DX11
    //    Release vertex layout
    _RELEASE(dcl);
#endif
}
