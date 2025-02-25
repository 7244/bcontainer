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

#ifdef bcontainer_set_PossibleUpdate
  #error PossibleUpdate now called CapacityUpdate
#endif

/* --- deprecated --- */

#ifndef bcontainer_set_Prefix
  #error ifndef bcontainer_set_Prefix
#endif

#ifndef bcontainer_set_CapacityUpdate
  #define bcontainer_set_CapacityUpdate
#endif

#ifndef bcontainer_set_PointerNodeType
  #define bcontainer_set_PointerNodeType 0
#endif

#ifndef bcontainer_set_NodeType
  #if bcontainer_set_PointerNodeType
    #define bcontainer_set_NodeType uintptr_t
  #else
    #define bcontainer_set_NodeType uint32_t
  #endif
#else
  #if bcontainer_set_PointerNodeType
    #error dont define NodeType
  #endif
#endif
#ifndef bcontainer_set_NodeSizeType
  #define bcontainer_set_NodeSizeType uint32_t
#endif

#ifndef bcontainer_set_Clear
  #define bcontainer_set_Clear 0
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
    #define bcontainer_set_PreserveSome 1
  #endif
#endif

#ifndef bcontainer_set_StoreFormat1_StoreNodeList
  #define bcontainer_set_StoreFormat1_StoreNodeList 0
#endif

#ifndef bcontainer_set_alloc_RetryAmount
  #define bcontainer_set_alloc_RetryAmount 0x10
#endif

#if \
  !defined(bcontainer_set_alloc_open) && \
  !defined(bcontainer_set_alloc_resize) && \
  !defined(bcontainer_set_alloc_close)

  #if defined(__platform_libc)
    #define bcontainer_set_alloc_open malloc
    #define bcontainer_set_alloc_resize realloc
    #define bcontainer_set_alloc_close free
  #else
    #define bcontainer_set_alloc_open __generic_malloc
    #define bcontainer_set_alloc_resize __generic_realloc
    #define bcontainer_set_alloc_close __generic_free
  #endif
#endif

#ifndef bcontainer_set_use_mmap
  #if defined(__generic_mmap) && defined(__generic_munmap)
    #define bcontainer_set_use_mmap 1
  #else
    #define bcontainer_set_use_mmap 0
  #endif
#endif

#include "internal/PrepareAndInclude.h"

#undef bcontainer_set_use_mmap

#ifdef bcontainer_set_alloc_open
  #undef bcontainer_set_alloc_open
#endif
#ifdef bcontainer_set_alloc_resize
  #undef bcontainer_set_alloc_resize
#endif
#ifdef bcontainer_set_alloc_close
  #undef bcontainer_set_alloc_close
#endif

#undef bcontainer_set_alloc_RetryAmount

#ifdef bcontainer_set_NodeData
  #undef bcontainer_set_NodeData
#endif
#undef bcontainer_set_StoreFormat1_StoreNodeList
#undef bcontainer_set_PreserveSome
#undef bcontainer_set_RuntimePreallocate
#undef bcontainer_set_StoreFormat
#ifdef bcontainer_set_CountLockFailGlobal
  #undef bcontainer_set_CountLockFailGlobal
#endif
#undef bcontainer_set_CountLockFail
#undef bcontainer_set_MultiThread
#undef bcontainer_set_Recycle
#undef bcontainer_set_Clear
#undef bcontainer_set_NodeSizeType
#undef bcontainer_set_NodeType
#undef bcontainer_set_PointerNodeType
#undef bcontainer_set_CapacityUpdate
#undef bcontainer_set_Prefix
