#pragma once
class ESceneObjectTool;
class UIObjectTool :public UIToolCustom
{
public:
    UIObjectTool();
    virtual ~UIObjectTool();
    virtual void Draw();

    void DrawObjectsList();

    void RefreshList();
    IC const char* Current() { return m_Current; }
    virtual void OnDrawUI();
    ESceneObjectTool* ParentTools;
private:
    void RefreshListInternal();
    void OnItemFocused(ListItem*item);
    void SelByRefObject(bool flag);
    void MultiSelByRefObject(bool flag);
    void ClearSurface(bool selected);
    const char* m_Current;
    UIItemListForm* m_ObjectList;
    bool m_MultiAppend;
    bool m_PropRandom;
    bool m_RandomAppend;
    float m_selPercent;
    bool m_Selection;

    xr_string RAIFile;
private:
    ref_texture m_TextureNull;
    ImTextureID m_RealTexture;
    ImTextureID m_RemoveTexture;
    UIPropertiesForm* m_Props;

    bool bDrawList = true;
};