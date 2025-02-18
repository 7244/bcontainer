/* +++ deprecated +++ */

#ifdef bcontainer_set_BufferingFormat
  #error its no longer here. dont set it.
#endif

#ifdef bcontainer_set_MultipleType
  #error this will be implemented later
#endif
#ifdef bcontainer_set_MultipleType_Sizes
  #error this will be implemented later
#endif
#ifdef bcontainer_set_MultipleType_SizeArray
  #error this will be implemented later
#endif

#ifdef bcontainer_set_HandleAllocate
  #error will be implemented later
#endif

/* --- deprecated --- */

#ifndef bcontainer_set_Prefix
  #error ifndef bcontainer_set_Prefix
#endif

#ifndef bcontainer_set_PossibleUpdate
  #define bcontainer_set_PossibleUpdate
#endif

#ifndef bcontainer_set_NodeType
  #define bcontainer_set_NodeType uint32_t
#endif
#ifndef bcontainer_set_NodeSizeType
  #define bcontainer_set_NodeSizeType uint32_t
#endif

#ifndef bcontainer_set_Recycle
  #define bcontainer_set_Recycle 0
#endif

#ifndef bcontainer_set_MultiThread
  #define bcontainer_set_MultiThread 0
#endif

#ifndef bcontainer_set_CountLockFail
  #define bcontainer_set_CountLockFail 0
#endif

#ifndef bcontainer_set_StoreFormat
  #if !bcontainer_set_MultiThread
    #define bcontainer_set_StoreFormat 0
  #else
    #define bcontainer_set_StoreFormat 1
  #endif
#endif

#ifndef bcontainer_set_RuntimePreallocate
  #if !bcontainer_set_MultiThread
    #define bcontainer_set_RuntimePreallocate 0
  #else
    #define bcontainer_set_RuntimePreallocate 1
  #endif
#endif

#ifndef bcontainer_set_PreserveSome
  #if !bcontainer_set_RuntimePreallocate
    #define bcontainer_set_PreserveSome 0
  #else
    /* TODO overflow check sizeof(node_id) */
    #define bcontainer_set_PreserveSome 1
  #endif
#endif

#ifndef bcontainer_set_alloc_open
  #if defined(__generic_malloc)
    #define bcontainer_set_alloc_open __generic_malloc
  #else
    #define bcontainer_set_alloc_open malloc
  #endif
#endif
#ifndef bcontainer_set_alloc_resize
  #if defined(__generic_realloc)
    #define bcontainer_set_alloc_resize __generic_realloc
  #else
    #define bcontainer_set_alloc_resize realloc
  #endif
#endif
#ifndef bcontainer_set_alloc_close
  #if defined(__generic_free)
    #define bcontainer_set_alloc_close __generic_free
  #else
    #define bcontainer_set_alloc_close free
  #endif
#endif

#ifndef bcontainer_set_alloc_RetryAmount
  #define bcontainer_set_alloc_RetryAmount 0x10
#endif

#define _bcontainer_P(p0) CONCAT3(bcontainer_set_Prefix, _, p0)

#include "internal/rest.h"

#undef _bcontainer_P

#undef bcontainer_set_alloc_RetryAmount

#undef bcontainer_set_alloc_close
#undef bcontainer_set_alloc_resize
#undef bcontainer_set_alloc_open

#ifdef bcontainer_set_NodeData
  #undef bcontainer_set_NodeData
#endif
#undef bcontainer_set_PreserveSome
#undef bcontainer_set_RuntimePreallocate
#undef bcontainer_set_StoreFormat
#ifdef bcontainer_set_CountLockFailGlobal
  #undef bcontainer_set_CountLockFailGlobal
#endif
#undef bcontainer_set_CountLockFail
#undef bcontainer_set_MultiThread
#undef bcontainer_set_Recycle
#undef bcontainer_set_NodeSizeType
#undef bcontainer_set_NodeType
#undef bcontainer_set_PossibleUpdate
#undef bcontainer_set_Prefix
