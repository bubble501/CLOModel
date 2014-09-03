#ifndef ProjectGlobals_h__
#define ProjectGlobals_h__

//Comment if not used for library
//#define BLOOMBERG_LIB_BUILD

#define QBBG_PRIVATE


#ifdef BLOOMBERG_LIB_BUILD
#ifdef QBLOOMBERG_LIB
#define BLOOMBERG_EXPORT __declspec(dllexport)
#define BLOOMBERG_EXTERN 
#else
# define BLOOMBERG_EXPORT __declspec(dllimport)
#define BLOOMBERG_EXTERN extern
#endif
#else 
#define BLOOMBERG_EXPORT
#endif // BLOOMBERG_LIB_BUILD



#endif
