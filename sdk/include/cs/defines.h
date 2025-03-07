////////////////////////////////////////////////////////////////////////////
//    Module         : defines.h
//    Created     : 23.04.2008
//  Modified     : 23.04.2008
//    Author        : Dmitriy Iassenev
//    Description : important macro definitions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_DEFINES_H_INCLUDED
#define CS_DEFINES_H_INCLUDED

// DECLSPEC_NOVTABLE macro
#ifndef DECLSPEC_NOVTABLE
#    if (_MSC_VER >= 1100) && defined(__cplusplus)
#        define DECLSPEC_NOVTABLE            __declspec(novtable)
#    else // #if (_MSC_VER >= 1100) && defined(__cplusplus)
#        define DECLSPEC_NOVTABLE
#    endif // #if (_MSC_VER >= 1100) && defined(__cplusplus)
#endif // #ifndef DECLSPEC_NOVTABLE

// CS_STRING_CONCAT macro
#if defined(CS_STRING_CONCAT) || defined(CS_STRING_CONCAT_HELPER)
    static_assert(false, "CS STRING_CONCAT or CS_STRING_CONCAT_HELPER or CS_STRING_CONCAT4 macro already defined");
#endif // #if defined(CS_STRING_CONCAT) || defined(CS_STRING_CONCAT_HELPER)

#define CS_STRING_CONCAT_HELPER(a,b)        a##b
#define CS_STRING_CONCAT(a,b)                CS_STRING_CONCAT_HELPER(a,b)

// CS_MAKE_STRING macro
#if defined(CS_MAKE_STRING) || defined(CS_MAKE_STRING_HELPER)
    static_assert(false, "CS MAKE_STRING or CS_MAKE_STRING_HELPER macro already defined");
#endif // #if defined(CS_MAKE_STRING) || defined(CS_MAKE_STRING_HELPER)

#define CS_MAKE_STRING_HELPER(a)            #a
#define CS_MAKE_STRING(a)                    CS_MAKE_STRING_HELPER(a)

#define CS_LIBRARY_NAME(library, extension)    \
    CS_MAKE_STRING(\
        CS_STRING_CONCAT(\
            CS_LIBRARY_PREFIX,\
            CS_STRING_CONCAT(\
                library,\
                CS_STRING_CONCAT(\
                    CS_PLATFORM_ID,\
                    CS_STRING_CONCAT(\
                        CS_SOLUTION_CONFIGURATION_ID,\
                        CS_STRING_CONCAT(\
                            .,\
                            extension\
                        )\
                    )\
                )\
            )\
        )\
    )

#endif // #ifndef CS_DEFINES_H_INCLUDED