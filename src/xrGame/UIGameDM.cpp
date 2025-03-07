#include "StdAfx.h"

#include "UIGameDM.h"

#include "ui/UISkinSelector.h"
#include "ui/UIPdaWnd.h"
#include "ui/UIMapDesc.h"
#include "ui/KillMessageStruct.h"
#include "Level.h"
#include "game_cl_base.h"
#include "Spectator.h"
#include "Inventory.h"
#include "InventoryOwner.h"
#include "xrServer_Objects_ALife_Items.h"
#include "../../xrUI/xrUIXmlParser.h"
#include "game_cl_deathmatch.h"
#include "ui/UIMoneyIndicator.h"
#include "ui/UIRankIndicator.h"
#include "ui/UIVoteStatusWnd.h"
#include "ui/UIActorMenu.h"
#include "UIHelperGame.h"
#include "UITeamPanels.h"
#include "object_broker.h"

#define MSGS_OFFS 510

#define TIME_MSG_COLOR            0xffff0000
#define SPECTRMODE_MSG_COLOR    0xffff0000
#define NORMAL_MSG_COLOR        0xffffffff
#define ROUND_RESULT_COLOR        0xfff0fff0
#define VOTE0_MSG_COLOR            0xffff0000
#define VOTE1_MSG_COLOR            0xff00ff00
#define DEMOPLAY_COLOR            0xff00ff00
#define WARM_UP_COLOR            0xff00ff00


#define DI2PX(x) float(iFloor((x+1)*float(UI_BASE_WIDTH)*0.5f))
#define DI2PY(y) float(iFloor((y+1)*float(UI_BASE_HEIGHT)*0.5f))
#define SZ(x) x*UI_BASE_WIDTH

//--------------------------------------------------------------------
#define TEAM_PANELS_DM_XML_NAME "ui_team_panels_dm.xml"

//--------------------------------------------------------------------
CUIGameDM::CUIGameDM()
{
    m_game                                = nullptr; 
    m_voteStatusWnd                        = nullptr;
}

//--------------------------------------------------------------------
void CUIGameDM::SetClGame (game_cl_GameState* g)
{
    inherited::SetClGame(g);
    m_game = smart_cast<game_cl_Deathmatch*>(g);
    R_ASSERT(m_game);

    UpdateTeamPanels    ();
}

void    CUIGameDM::Init(int stage)
{
    if(stage==0)
    { // shared
        m_pTeamPanels                    = new UITeamPanels();
        m_pMoneyIndicator                = new CUIMoneyIndicator();
        m_pMoneyIndicator->SetAutoDelete(true);
        m_pRankIndicator                = new CUIRankIndicator();
        m_pRankIndicator->SetAutoDelete    (true);
        m_pFragLimitIndicator            = new CUITextWnd();
        m_pFragLimitIndicator->SetAutoDelete(true);
        
        inherited::Init                    (stage);
        m_time_caption                    = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_timelimit", m_window);
        m_spectrmode_caption            = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_spetatormode", m_window);
        m_spectator_caption                = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_spectator", m_window);
        m_pressjump_caption                = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_pressjump", m_window);
        m_pressbuy_caption                = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_pressbuy", m_window);
        m_round_result_caption            = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_round_result", m_window);
        m_force_respawn_time_caption    = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_force_respawn_time", m_window);
        m_demo_play_caption                = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_demo_play", m_window);
        m_warm_up_caption                = UIHelper::CreateTextWnd(*m_msgs_xml, "mp_warm_up", m_window);
    }
    if(stage==1)
    { //unique
        m_pTeamPanels->Init                (TEAM_PANELS_DM_XML_NAME, "team_panels_wnd");
        CUIXml                            uiXml;
        uiXml.Load                        (CONFIG_PATH, UI_PATH, "ui_game_dm.xml");
        CUIXmlInit::InitWindow            (uiXml,"global", 0, m_window);
        m_pMoneyIndicator->InitFromXML    (uiXml);
        m_pRankIndicator->InitFromXml    (uiXml);
        CUIXmlInit::InitTextWnd            (uiXml,"fraglimit",0, m_pFragLimitIndicator);
    }
    if(stage==2)
    { //after
        inherited::Init                    (stage);
        m_window->AttachChild            (m_pMoneyIndicator);
        m_window->AttachChild            (m_pRankIndicator);
        m_window->AttachChild            (m_pFragLimitIndicator);

    }
};

void CUIGameDM::UnLoad()
{
    inherited::UnLoad        ();
    xr_delete                (m_pTeamPanels);
    xr_delete                (m_voteStatusWnd);
}

CUIGameDM::~CUIGameDM()
{
}


void CUIGameDM::SetTimeMsgCaption(LPCSTR str)
{
    m_time_caption->SetTextST(str);
}

void CUIGameDM::ShowFragList(bool bShow)
{
    if (bShow && m_pTeamPanels)
        AddDialogToRender(m_pTeamPanels);
    else
        RemoveDialogToRender(m_pTeamPanels);
}

void CUIGameDM::ShowPlayersList(bool bShow)
{
    if (bShow && m_pTeamPanels)
        AddDialogToRender(m_pTeamPanels);
    else
        RemoveDialogToRender(m_pTeamPanels);
}


void CUIGameDM::SetSpectrModeMsgCaption(LPCSTR str)
{
    m_spectrmode_caption->SetTextST(str);
}

void CUIGameDM::SetSpectatorMsgCaption(LPCSTR str)
{
    m_spectator_caption->SetTextST(str);
}

void CUIGameDM::SetPressJumpMsgCaption(LPCSTR str)
{
    m_pressjump_caption->SetTextST(str);
}

void CUIGameDM::SetPressBuyMsgCaption(LPCSTR str)
{
    m_pressbuy_caption->SetTextST(str);
}


void CUIGameDM::SetRoundResultCaption(LPCSTR str)
{
    m_round_result_caption->SetTextST(str);
}

void CUIGameDM::SetForceRespawnTimeCaption(LPCSTR str)
{
    m_force_respawn_time_caption->SetTextST(str);
}

void CUIGameDM::SetDemoPlayCaption(LPCSTR str)
{
    m_demo_play_caption->SetTextST(str);
}

void CUIGameDM::SetWarmUpCaption(LPCSTR str)
{
    m_warm_up_caption->SetTextST(str);
}

void CUIGameDM::SetVoteMessage                    (LPCSTR str)
{
    if(!str)
        xr_delete(m_voteStatusWnd);
    else{
        if(!m_voteStatusWnd)
        {
            CUIXml                            uiXml;
            uiXml.Load                        (CONFIG_PATH, UI_PATH, "ui_game_dm.xml");
            m_voteStatusWnd                    = new UIVoteStatusWnd();
            m_voteStatusWnd->InitFromXML    (uiXml);
        }
        m_voteStatusWnd->Show                (true);
        m_voteStatusWnd->SetVoteMsg            (str);
    }
};

void CUIGameDM::SetVoteTimeResultMsg            (LPCSTR str)
{
    if(m_voteStatusWnd)
        m_voteStatusWnd->SetVoteTimeResultMsg(str);
}

void CUIGameDM::OnFrame()
{
    inherited::OnFrame                ();
    if(m_voteStatusWnd && m_voteStatusWnd->IsShown()) 
        m_voteStatusWnd->Update        ();
}

void CUIGameDM::Render()
{
    inherited::Render                ();
    if(m_voteStatusWnd && m_voteStatusWnd->IsShown()) 
        m_voteStatusWnd->Draw        ();
}

void CUIGameDM::DisplayMoneyChange(LPCSTR deltaMoney)
{
    m_pMoneyIndicator->SetMoneyChange(deltaMoney);
}

void CUIGameDM::DisplayMoneyBonus(KillMessageStruct* bonus){
    m_pMoneyIndicator->AddBonusMoney(*bonus);
}

void CUIGameDM::ChangeTotalMoneyIndicator(LPCSTR newMoneyString)
{
    m_pMoneyIndicator->SetMoneyAmount(newMoneyString);
}

void    CUIGameDM::SetRank(s16 team, u8 rank)
{
    m_pRankIndicator->SetRank(u8(m_game->ModifyTeam(team)), rank);
};

void CUIGameDM::SetFraglimit(int local_frags, int fraglimit)
{
    string64 str;
    if(fraglimit)
        xr_sprintf(str,"%d/%d", local_frags, fraglimit);
    else
        xr_sprintf(str,"%d", local_frags);

    m_pFragLimitIndicator->SetText(str);
}

void CUIGameDM::UpdateTeamPanels()
{
    m_pTeamPanels->NeedUpdatePanels();
    m_pTeamPanels->NeedUpdatePlayers();
}
