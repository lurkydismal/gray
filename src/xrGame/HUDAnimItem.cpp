#include "StdAfx.h"
#include "HUDAnimItem.h"
#include "Actor.h"
#include "Inventory.h"
#include "player_hud.h"
#include "CustomDetector.h"

void CHUDAnimItem::Load(LPCSTR section)
{
    CInventoryItemObject::Load(section);

    hud_sect = pSettings->r_string(section, "hud");
    m_animation_slot = pSettings->r_u32(section, "animation_slot");

    m_nearwall_dist_min = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_min", .2f);
    m_nearwall_dist_max = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_dist_max", 1.f);
    m_nearwall_target_hud_fov = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_target_hud_fov", 0.27f);
    m_nearwall_speed_mod = READ_IF_EXISTS(pSettings, r_float, section, "nearwall_speed_mod", 10.f);

    m_fHudFov = READ_IF_EXISTS(pSettings, r_float, section, "hud_fov", 0.0f);

    m_current_inertion.PitchOffsetR = READ_IF_EXISTS(pSettings, r_float, section, "inertion_pitch_offset_r", PITCH_OFFSET_R);
    m_current_inertion.PitchOffsetD = READ_IF_EXISTS(pSettings, r_float, section, "inertion_pitch_offset_d", PITCH_OFFSET_D);
    m_current_inertion.PitchOffsetN = READ_IF_EXISTS(pSettings, r_float, section, "inertion_pitch_offset_n", PITCH_OFFSET_N);

    m_current_inertion.OriginOffset = READ_IF_EXISTS(pSettings, r_float, section, "inertion_origin_offset", ORIGIN_OFFSET);
    m_current_inertion.TendtoSpeed = READ_IF_EXISTS(pSettings, r_float, section, "inertion_tendto_speed", TENDTO_SPEED);

    m_bDisableBore = true;
}

bool CHUDAnimItem::SendDeactivateItem()
{
    return true;
}

bool CHUDAnimItem::ActivateItem()
{
    if (CurrentMotion.size() != 0)
    {
        SetState(eIdle);
        m_dwMotionStartTm = u32(-1);
    }

    return true;
}

void CHUDAnimItem::DeactivateItem()
{
    g_player_hud->RemoveHudItem(hud_sect);
}

void CHUDAnimItem::UpdateCL()
{
    CHudItemObject::UpdateCL();
    
    if (GetState() == eHidden)
        return;

    if (CurrentMotion.size() == 0)
    {
        if (Device.dwTimeGlobal >= m_dwMotionEndTm)
        {
            SetState(eHidden);
            Actor()->inventory().Activate(NO_ACTIVE_SLOT);
            Actor()->inventory().Activate(OldSlot);

            if (DetectorActive)
            {
                if (CCustomDetector* Detector = (CCustomDetector*)Actor()->inventory().ItemFromSlot(DETECTOR_SLOT))
                {
                    Detector->switch_detector();
                }
            }

            m_dwMotionStartTm = u32(-1);
        }

        return;
    }

    if (g_player_hud != nullptr)
    {
        bool Test1 = g_player_hud->attached_item(0) != nullptr && g_player_hud->attached_item(0)->m_parent_hud_item == this;
        bool Test2 = g_player_hud->attached_item(1) != nullptr && g_player_hud->attached_item(1)->m_parent_hud_item == this;

        if (!Test1 && !Test2)
            return;
    }

    PlayHUDMotion(CurrentMotion, true, this, eIdle);
    CurrentMotion = "";
}

bool CHUDAnimItem::need_renderable()
{
    return (m_dwMotionStartTm != u32(-1)) && m_dwMotionStartTm + 15 < Device.dwTimeGlobal;
}

void CHUDAnimItem::PlayHudAnim(const char* Section, const char* Anim)
{
    auto& Inventory = Actor()->inventory();
    CHUDAnimItem* ThisItem = (CHUDAnimItem*)Inventory.ItemFromSlot(ANIM_SLOT);

    if (ThisItem == nullptr)
        return;

    if (ThisItem == Inventory.ActiveItem())
        return;

    if (CCustomDetector* Detector = (CCustomDetector*)Inventory.ItemFromSlot(DETECTOR_SLOT))
    {
        ThisItem->DetectorActive = Detector->IsActive();
    }

    ThisItem->OldSlot = Inventory.GetActiveSlot();
    ThisItem->hud_sect = Section;
    ThisItem->CurrentMotion = Anim;

    Actor()->inventory().Activate(ANIM_SLOT);
}
