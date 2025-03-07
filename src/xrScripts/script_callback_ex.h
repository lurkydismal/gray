////////////////////////////////////////////////////////////////////////////
//    Module         : script_callback_ex.h
//    Created     : 06.02.2004
//  Modified     : 11.01.2005
//    Author        : Sergey Zhemeitsev and Dmitriy Iassenev
//    Description : Script callbacks with return value
////////////////////////////////////////////////////////////////////////////

#pragma once
#include "script_engine.h"
#include "script_export_space.h"

IC bool compare_safe(const luabind::object& o1, const luabind::object& o2)
{
    if (o1.type() == LUA_TNIL && o2.type() == LUA_TNIL)
        return                        (true);

    return                            (o1 == o2);
}

#if XRAY_EXCEPTIONS
#    define process_error \
        catch(luabind::error &e) {\
            g_pScriptEngine->print_output(g_pScriptEngine->lua(),"",LUA_ERRRUN);\
        }
#else
#    define process_error
#endif

template<typename TResult>
class CScriptCallbackEx
{
public:
    using ResultType = TResult;

private:
    using functor_type = luabind::functor<ResultType>;
    using object_type = luabind::object;
    using unspecified_bool_type = bool(CScriptCallbackEx::*)() const;

protected:
    functor_type m_functor;
    object_type m_object;

private:
    bool empty() const { return !!m_functor.lua_state(); }

public:
    CScriptCallbackEx() {}
    virtual ~CScriptCallbackEx() {}

    CScriptCallbackEx(const CScriptCallbackEx& callback)
    {
        clear();
        *this = callback;
    }

    CScriptCallbackEx& operator=(const CScriptCallbackEx& callback)
    {
        clear();
        if (callback.m_functor.is_valid() && callback.m_functor.lua_state())
            m_functor = callback.m_functor;
        if (callback.m_object.is_valid() && callback.m_object.lua_state())
            m_object = callback.m_object;
        return *this;
    }

    bool operator==(const CScriptCallbackEx& callback) const
    {
        return compare_safe(m_object, (callback.m_object)) && m_functor == (callback.m_functor);
    }

    bool operator==(const object_type& object) const
    {
        return compare_safe(m_object, object);
    }

    void set(const functor_type& functor)
    {
        clear();
        m_functor = functor;
    }

    void set(const functor_type& functor, const object_type& object)
    {
        clear();
        m_functor = functor;
        m_object = object;
    }

    void clear()
    {
        m_functor.~functor_type();
        new(&m_functor) functor_type();
        m_object.~object_type();
        new(&m_object) object_type();
    }

    operator unspecified_bool_type() const
    {
        return !m_functor.is_valid() ? 0 : &CScriptCallbackEx::empty;
    }

    template<typename... Args>
    TResult operator()(Args &&...args) const
    {
        try
        {
            try
            {
                if (m_functor)
                {
                    VERIFY(m_functor.is_valid());
                    if (m_object.is_valid())
                    {
                        VERIFY(m_object.is_valid());
                        return TResult(m_functor(m_object, std::forward<Args>(args)...));
                    }
                    return TResult(m_functor(std::forward<Args>(args)...));
                }
            }
            process_error catch (std::exception&)
            {
                g_pScriptEngine->print_output(g_pScriptEngine->lua(), "", 1);
            }
        }
        catch (...)
        {
            const_cast<CScriptCallbackEx<TResult>*>(this)->clear();
        }
        return TResult(0);
    }

    template<typename... Args>
    TResult operator()(Args &&...args)
    {
        try
        {
            try
            {
                if (m_functor)
                {
                    VERIFY(m_functor.is_valid());
                    if (m_object.is_valid())
                    {
                        VERIFY(m_object.is_valid());
                        return TResult(m_functor(m_object, std::forward<Args>(args)...));
                    }
                    return TResult(m_functor(std::forward<Args>(args)...));
                }
            }
            process_error catch (std::exception&)
            {
                g_pScriptEngine->print_output(g_pScriptEngine->lua(), "", 1);
            }
        }
        catch (...)
        {
            const_cast<CScriptCallbackEx<TResult>*>(this)->clear();
        }
        return TResult(0);
    }
};

template<>
template<typename... Args>
void CScriptCallbackEx<void>::operator()(Args &&...args) const
{
    try
    {
        try
        {
            if (m_functor)
            {
                VERIFY(m_functor.is_valid());
                if (m_object.is_valid())
                {
                    VERIFY(m_object.is_valid());
                    m_functor(m_object, std::forward<Args>(args)...);
                }
                else
                    m_functor(std::forward<Args>(args)...);
            }
        }
        process_error catch (std::exception&)
        {
            g_pScriptEngine->print_output(g_pScriptEngine->lua(), "", 1);
        }
    }
    catch (...)
    {
        const_cast<CScriptCallbackEx<void>*>(this)->clear();
    }
}

template<>
template<typename... Args>
void CScriptCallbackEx<void>::operator()(Args &&...args)
{
    try
    {
        try
        {
            if (m_functor)
            {
                VERIFY(m_functor.is_valid());
                if (m_object.is_valid())
                {
                    VERIFY(m_object.is_valid());
                    m_functor(m_object, std::forward<Args>(args)...);
                }
                else
                    m_functor(std::forward<Args>(args)...);
            }
        }
        process_error catch (std::exception&)
        {
            g_pScriptEngine->print_output(g_pScriptEngine->lua(), "", 1);
        }
    }
    catch (...)
    {
        const_cast<CScriptCallbackEx<void>*>(this)->clear();
    }
}
