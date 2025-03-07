// sea 08.01.2008

#include "stdafx.h"
#include "UIDoubleProgressBar.h"
#include "../UIXmlInit.h"

CUIDoubleProgressBar::CUIDoubleProgressBar()
{
    AttachChild( &m_progress_one );
    AttachChild( &m_progress_two );
}

CUIDoubleProgressBar::~CUIDoubleProgressBar()
{
}

void CUIDoubleProgressBar::InitFromXml( CUIXml& xml_doc, LPCSTR path )
{
    CUIXmlInit::InitProgressBar( xml_doc, path, 0, &m_progress_one );
    CUIXmlInit::InitProgressBar( xml_doc, path,    0, &m_progress_two );
    
    string256 buf;
    xr_strconcat(buf, path, ":color_less" );
    m_less_color = CUIXmlInit::GetColor( xml_doc, buf, 0, color_rgba( 255,   0, 0, 255 ) );
    xr_strconcat(buf, path, ":color_more" );
    m_more_color = CUIXmlInit::GetColor( xml_doc, buf, 0, color_rgba(   0, 255, 0, 255 ) );

    xr_strconcat(buf, path, ":first_min_color");

    if (xml_doc.NavigateToNode(buf, 0)) {
        m_progress_two.m_bUseColor = true;

        u32 color = CUIXmlInit::GetColor(xml_doc, buf, 0, 0xff);
        m_progress_two.m_minColor.set(color);

        xr_strconcat(buf, path, ":first_middle_color");
        if (xml_doc.NavigateToNode(buf, 0)) {
            color = CUIXmlInit::GetColor(xml_doc, buf, 0, 0xff);
            m_progress_two.m_bUseMidColor = true;
            m_progress_two.m_middleColor.set(color);
        }
        xr_strconcat(buf, path, ":first_max_color");

        color = CUIXmlInit::GetColor(xml_doc, buf, 0, 0xff);
        m_progress_two.m_maxColor.set(color);
    }

    m_progress_one.SetRange( 0.0f, 100.0f );
    m_progress_two.SetRange( 0.0f, 100.0f );
    
    m_progress_one.ShowBackground( true );
    m_progress_two.ShowBackground( false );
}

void CUIDoubleProgressBar::SetTwoPos( float cur_value, float compare_value )
{
    if ( cur_value < compare_value ) // red
    {
        m_progress_one.m_UIProgressItem.SetTextureColor( m_less_color );
        m_progress_one.SetProgressPos( compare_value );
        m_progress_two.SetProgressPos( cur_value );
    }
    else if ( cur_value > compare_value ) // green
    {
        m_progress_one.m_UIProgressItem.SetTextureColor( m_more_color );
        m_progress_one.SetProgressPos( cur_value );
        m_progress_two.SetProgressPos( compare_value );
    }
    else
    {
        m_progress_one.m_UIProgressItem.SetTextureColor( m_progress_two.m_UIProgressItem.GetTextureColor() );
        m_progress_one.SetProgressPos( cur_value );
        m_progress_two.SetProgressPos( cur_value );
    }
}
