////////////////////////////////////////////////////////////////////////////
//    Module         : object_factory.h
//    Created     : 27.05.2004
//  Modified     : 27.05.2004
//    Author        : Dmitriy Iassenev
//    Description : Object factory
////////////////////////////////////////////////////////////////////////////

#ifndef object_factoryH
#define object_factoryH

#pragma once
#include "../xrEngine/IGame_ObjectFactory.h"
#include "../xrScripts/script_export_space.h"
#include "object_item_abstract.h"
#include "xrServer_Objects.h"

class CObjectFactory:
    public IGame_ObjectFactory
{
public:
#ifndef NO_XR_GAME
    using CLIENT_BASE_CLASS = ObjectFactory::CLIENT_BASE_CLASS ;
    using CLIENT_SCRIPT_BASE_CLASS = ObjectFactory::CLIENT_SCRIPT_BASE_CLASS;
#endif

    using SERVER_BASE_CLASS = ObjectFactory::SERVER_BASE_CLASS ;
    using SERVER_SCRIPT_BASE_CLASS = ObjectFactory::SERVER_SCRIPT_BASE_CLASS;

protected:
    struct CObjectItemPredicate {
        IC    bool                    operator()                            (const CObjectItemAbstract *item1, const CObjectItemAbstract *item2) const;
        IC    bool                    operator()                            (const CObjectItemAbstract *item, const CLASS_ID &clsid) const;
    };

    struct CObjectItemPredicateCLSID {
        CLASS_ID                    m_clsid;

        IC                            CObjectItemPredicateCLSID            (const CLASS_ID &clsid);
        IC    bool                    operator()                            (const CObjectItemAbstract *item) const;
    };

    struct CObjectItemPredicateScript {
        shared_str                    m_script_clsid_name;

        IC                            CObjectItemPredicateScript            (const shared_str &script_clsid_name);
        IC    bool                    operator()                            (const CObjectItemAbstract *item) const;
    };

public:
    typedef xr_vector<CObjectItemAbstract*>        OBJECT_ITEM_STORAGE;
    typedef OBJECT_ITEM_STORAGE::iterator        iterator;
    typedef OBJECT_ITEM_STORAGE::const_iterator    const_iterator;

protected:
    mutable OBJECT_ITEM_STORAGE            m_clsids;
    mutable bool                        m_actual;

protected:
            void                        register_classes                ();
    IC        void                        add                                (CObjectItemAbstract *item);
    IC        const OBJECT_ITEM_STORAGE    &clsids                            () const;
    IC        void                        actualize                        () const;
    template <typename _unknown_type>
    IC        void                        add                                (const CLASS_ID &clsid, LPCSTR script_clsid);

#ifndef NO_XR_GAME
    template <typename _client_type, typename _server_type>
    IC        void                        add                                (const CLASS_ID &clsid, LPCSTR script_clsid);
    IC        const CObjectItemAbstract    &item                            (const CLASS_ID &clsid) const;
#else
    IC        const CObjectItemAbstract    *item                            (const CLASS_ID &clsid, bool no_assert) const;
#endif

public:
                                        CObjectFactory                    ();
    virtual                                ~CObjectFactory                    ();
    virtual    void                        init                            ();
#ifndef NO_XR_GAME
    IC        CLIENT_BASE_CLASS            *client_object                    (const CLASS_ID &clsid) const;
    IC        SERVER_BASE_CLASS            *server_object                    (const CLASS_ID &clsid, LPCSTR section) const;
#else
    IC        SERVER_BASE_CLASS            *server_object                    (const CLASS_ID &clsid, LPCSTR section) const;
#endif

    IC virtual int                        script_clsid                    (const CLASS_ID &clsid) const;
    virtual    void                        register_script                    () const;
            void                        register_script_class            (LPCSTR client_class, LPCSTR server_class, LPCSTR clsid, LPCSTR script_clsid);
            void                        register_script_class            (LPCSTR unknown_class, LPCSTR clsid, LPCSTR script_clsid);
            void                        register_script_classes            ();

    virtual    void                        init_script(const char* str);
    virtual    void                        export_classes(lua_State* L);

        DECLARE_SCRIPT_REGISTER_FUNCTION
    };

IC    const CObjectFactory &object_factory();

#include "object_factory_inline.h"
#endif