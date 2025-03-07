#include "stdafx.h"

CCustomObject* EScene::FindObjectByName( LPCSTR name, ObjClassID classfilter )
{
    if(!name)
    return NULL;
    
    CCustomObject* object = 0;
    if (classfilter==OBJCLASS_DUMMY)
    {
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; ++_I)
        {
            ESceneCustomOTool* mt = smart_cast<ESceneCustomOTool*>(_I->second);

            if (mt&&(0!=(object=mt->FindObjectByName(name))))
                return object;
        }
    }else{
        ESceneCustomOTool* mt = GetOTool(classfilter); VERIFY(mt);
        if (mt&&(0!=(object=mt->FindObjectByName(name)))) return object;
    }
    return object;
}

CCustomObject* EScene::FindObjectByName( LPCSTR name, CCustomObject* pass_object )
{
    CCustomObject* object = 0;
    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++){
        ESceneCustomOTool* mt = smart_cast<ESceneCustomOTool*>(_I->second);
        if (mt&&(0!=(object=mt->FindObjectByName(name,pass_object)))) return object;
    }
    return 0;
}

bool EScene::FindDuplicateName()
{
// find duplicate name
    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++){
        ESceneCustomOTool* mt = smart_cast<ESceneCustomOTool*>(_I->second);
        if (mt){
            ObjectList& lst = mt->GetObjects(); 
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if (FindObjectByName((*_F)->GetName(), *_F)){
                    ELog.DlgMsg(mtError,"Duplicate object name already exists: '%s'",(*_F)->GetName());
                    return true;
                }
        }
    }
    return false;
}

void EScene::GenObjectName(ObjClassID cls_id, char* buffer, const char* pref)
{
    for (int i = 0; true; i++)
    {
        bool result;
        xr_string temp;
        if (pref && pref[0])
        {
            temp = pref;
            if (i != 0)
            {
                temp += "_" + xr_string::ToString(i - 1);
            }
        }
        else
        {
            ESceneCustomOTool* ot = GetOTool(cls_id);
            VERIFY(ot);

            temp = ot->ClassName() + xr_string("_") + xr_string::ToString(i);
        }

        FindObjectByNameCB(temp.c_str(), result);
        if (!result)
        {
            xr_strcpy(buffer, 256, temp.c_str());
            return;
        }
    }
}