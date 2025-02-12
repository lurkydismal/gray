#include "stdafx.h"
#include "GameSpy_Full.h"
#include "GameSpy_SAKE.h"
#include "../MainMenu.h"
#include "../profile_store.h"


shared_str const CGameSpy_SAKE::TryToTranslate(SAKERequestResult const & request_result)
{
    string16 digit_dest;
    string256 tmp_string = {};
    _itoa_s(request_result, digit_dest, 10);
    xr_strconcat(
        tmp_string,
        "mp_sake_database_request_error_",
        digit_dest
    );
    return tmp_string;
}
shared_str const CGameSpy_SAKE::TryToTranslate(SAKEStartRequestResult const & request_result)
{
    string16 digit_dest;
    string256 tmp_string = {};
    _itoa_s(request_result, digit_dest, 10);
    xr_strconcat(
        tmp_string,
        "mp_sake_database_start_request_error_",
        digit_dest
    );
    return tmp_string;
}

void CGameSpy_SAKE::LoadGameSpySAKE()
{
    GAMESPY_LOAD_FN(xrGS_sakeStartup);
    GAMESPY_LOAD_FN(xrGS_sakeShutdown);
    GAMESPY_LOAD_FN(xrGS_sakeSetProfile);
    GAMESPY_LOAD_FN(xrGS_sakeGetStartRequestResult);
    GAMESPY_LOAD_FN(xrGS_sakeGetMyRecords);
    GAMESPY_LOAD_FN(xrGS_sakeCreateRecord);
    GAMESPY_LOAD_FN(xrGS_sakeUpdateRecord);
}

void CGameSpy_SAKE::Init()
{
    SAKEStartupResult startup_result = xrGS_sakeStartup(&m_sake_inst);
    VERIFY(startup_result == SAKEStartupResult_SUCCESS);
    if (startup_result != SAKEStartupResult_SUCCESS)
    {
        Msg("! GameSpy SAKE: failed to initialize, error code: %d", startup_result);
    }
}

CGameSpy_SAKE::CGameSpy_SAKE()
{
    m_sake_inst        = nullptr;
    LoadGameSpySAKE    ();
    Init            ();
}

CGameSpy_SAKE::~CGameSpy_SAKE()
{
    if (Engine.External.hGameSpy != 0 && m_sake_inst)
    {
        xrGS_sakeShutdown(m_sake_inst);
    }
}

void CGameSpy_SAKE::SetProfile(int profileId,
                              const char *loginTicket)
{
    xrGS_sakeSetProfile(m_sake_inst, profileId, loginTicket);
}

SAKEStartRequestResult CGameSpy_SAKE::GetRequestResult()
{
    return xrGS_sakeGetStartRequestResult(m_sake_inst);
}

SAKERequest CGameSpy_SAKE::GetMyRecords(SAKEGetMyRecordsInput * input,
                                       SAKERequestCallback callback,
                                       void * userData)
{
    return xrGS_sakeGetMyRecords(
        m_sake_inst,
        input,
        callback,
        userData
    );
}
SAKERequest CGameSpy_SAKE::CreateRecord(SAKECreateRecordInput * input,
                                       SAKERequestCallback callback,
                                       void * userdata)
{
    return xrGS_sakeCreateRecord(
        m_sake_inst,
        input,
        callback,
        userdata
    );
}
SAKERequest CGameSpy_SAKE::UpdateRecord(SAKEUpdateRecordInput * input,
                                       SAKERequestCallback callback,
                                       void * userdata)
{
    return xrGS_sakeUpdateRecord(
        m_sake_inst,
        input,
        callback,
        userdata
    );
}

SAKERequest CGameSpy_SAKE::SearchForRecords(SAKESearchForRecordsInput * input,
                                            SAKERequestCallback callback,
                                            void * userData)
{
    return xrGS_sakeSearchForRecords(
        m_sake_inst,
        input,
        callback,
        userData
    );
}