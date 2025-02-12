#pragma once
class CPHReqBase;
class CPHReqComparerV;

#include "../xrPhysics/IPHWorld.h"
class CPhysicsShell;

class CPHReqBase
{
public:
    virtual                        ~CPHReqBase                        ()                                    {}
    virtual bool                 obsolete                        ()                            const    =0                    ;
    virtual bool                compare                            (const CPHReqComparerV* v)    const    {return false;}        ;
};

class CPHCondition :
    public CPHReqBase
{
public:
    virtual bool             is_true                            ()                        =0                    ;
};

class CPHAction:
    public CPHReqBase
{
public:
    virtual void             run                                ()                        =0                    ;
};

class CPHOnesCondition:
    public CPHCondition
{
    bool b_called;
public:
                            CPHOnesCondition                (){b_called=false;}
    virtual bool             is_true                            (){b_called =true;return true;}
    virtual bool             obsolete                        ()const{return b_called;}
};

class CPHDummiAction:
    public CPHAction
{
public:
    virtual void             run                                (){;}
    virtual bool             obsolete                        ()const    {return false;}
};

class CPHCall
{
    CPHAction*        m_action            ;
    CPHCondition*    m_condition            ;
public:
                    CPHCall                            (CPHCondition* condition,CPHAction* action)                    ;
                    ~CPHCall                        ()                                                            ;
    void             check                            ()                                                            ;
    bool             obsolete                        ()                                                            ;
    bool            equal                            (CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action);
    bool            is_any                            (CPHReqComparerV* v)                                        ;
#ifdef DEBUG
const CPHAction        *action                            ()const{    return m_action;    }
const CPHCondition    *condition                        ()const{    return m_condition;    }
#endif
};

using PHCALL_STORAGE = xr_vector<CPHCall*>;
using PHCALL_I = PHCALL_STORAGE::iterator;

class CPHCommander:
    public IPHWorldUpdateCallbck
{
    xrCriticalSection    lock;
    PHCALL_STORAGE    m_calls;
public:
                        ~CPHCommander                ()                                                                ;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool                add_call_unique                (CPHCondition* condition,CPHReqComparerV* cmp_condition,CPHAction* action,CPHReqComparerV* cmp_action);
    void                add_call                    (CPHCondition* condition,CPHAction* action)                        ;

    void                remove_call                    (PHCALL_I i)                                                    ;
    bool                has_call                    (CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)    ;    
    PHCALL_I            find_call                    (CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)    ;                
    void                remove_call                    (CPHReqComparerV* cmp_condition,CPHReqComparerV* cmp_action)    ;
    void                remove_calls                (CPHReqComparerV* cmp_object)                                    ;

    void                update                      ()                                                                ;

    void                clear                        ()                                                                ;

private:
    virtual    void        update_step            ()                        {update();}
    virtual    void        phys_shell_relcase    (CPhysicsShell* sh)        ;
};