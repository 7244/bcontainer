#include "_m.h"

#if defined(bcontainer_set_NodeData)
  typedef bcontainer_set_NodeData _bcontainer_P(Node_t);
#else
  typedef void _bcontainer_P(Node_t);
#endif

#if bcontainer_set_MultiThread
  #define BME_set_Prefix _bcontainer_P(_FastLock)
  #define BME_set_LockValue 1
  #define BME_set_Sleep 0
  #define BME_set_CountLockFail bcontainer_set_CountLockFail
  #ifdef bcontainer_set_CountLockFailGlobal
    #define BME_set_CountLockFailGlobal bcontainer_set_CountLockFailGlobal
  #endif
  #include <BME/BME.h>
#endif

typedef struct{
  #if bcontainer_set_StoreFormat == 0
    bcontainer_set_NodeType Current;
    bcontainer_set_NodeType Possible;
    _bcontainer_P(Node_t) *ptr;
  #elif bcontainer_set_StoreFormat == 1
    _bcontainer_P(Node_t) *NodeLists[
      (uintptr_t)1 << __compile_time_log2(sizeof(bcontainer_set_NodeType) * 8)
    ];
    bcontainer_set_NodeType Current;

    #if bcontainer_set_StoreFormat1_StoreNodeList
      #if bcontainer_set_RuntimePreallocate
        #error not implemented
      #endif
      #if bcontainer_set_MultiThread
        #error not implemented
      #endif

      bcontainer_set_NodeType Possible;
      uint8_t NodeList;
    #else
      #if bcontainer_set_RuntimePreallocate
        bcontainer_set_NodeType Possible;

        #if bcontainer_set_MultiThread
          _bcontainer_P(_FastLock_t) AllocateLock;
        #endif
      #else
        #if bcontainer_set_MultiThread
          _bcontainer_P(_FastLock_t) NodeListsLocks[
            (uintptr_t)1 << __compile_time_log2(sizeof(bcontainer_set_NodeType) * 8)
          ];
        #endif
      #endif
    #endif
  #else
    #error ?
  #endif

  #ifndef bcontainer_set_NodeData
    bcontainer_set_NodeSizeType NodeSize;
  #endif

  #if bcontainer_set_Recycle
    struct{
      bcontainer_set_NodeType c;
      bcontainer_set_NodeType p;
    }e;
  #endif
}_bcontainer_P(t);

static
__forceinline
bcontainer_set_NodeSizeType
_bcontainer_P(_PadGivenNodeSize)
(
  _bcontainer_P(t) *This,
  bcontainer_set_NodeSizeType Size
){
  #if bcontainer_set_Recycle
    if(Size < sizeof(bcontainer_set_NodeSizeType)){
      Size = sizeof(bcontainer_set_NodeSizeType);
    }
  #endif

  return Size;
}

static
__forceinline
bcontainer_set_NodeSizeType
_bcontainer_P(GetNodeSize)
(
  _bcontainer_P(t) *This
){
  #if defined(bcontainer_set_NodeData)
    return _bcontainer_P(_PadGivenNodeSize)(This, sizeof(bcontainer_set_NodeData));
  #else
    return This->NodeSize;
  #endif
}

static
__forceinline
uint8_t
_bcontainer_P(_clz_nodeid)(
  bcontainer_set_NodeType node_id
){
  if(sizeof(bcontainer_set_NodeType) * 8 == 8){
    return __builtin_clz(node_id) - 24;
  }
  else if(sizeof(bcontainer_set_NodeType) * 8 == 16){
    return __builtin_clz(node_id) - 16;
  }
  else if(sizeof(bcontainer_set_NodeType) * 8 == 32){
    return __builtin_clz(node_id);
  }
  else if(sizeof(bcontainer_set_NodeType) * 8 == 64){
    return __builtin_clzll(node_id);
  }
  else{
    __abort();
  }
}

#if bcontainer_set_StoreFormat == 1
  static
  __forceinline
  uint8_t
  _bcontainer_P(_GetNodeListByNodeID)(
    bcontainer_set_NodeType node_id
  ){
    if(sizeof(bcontainer_set_NodeType) * 8 == 8){
      return __fast_8log2(node_id);
    }
    else if(sizeof(bcontainer_set_NodeType) * 8 == 16){
      return __fast_16log2(node_id);
    }
    else if(sizeof(bcontainer_set_NodeType) * 8 == 32){
      return __fast_32log2(node_id);
    }
    else if(sizeof(bcontainer_set_NodeType) * 8 == 64){
      return __fast_64log2(node_id);
    }
    else{
      __abort();
    }
  }

  static
  uintptr_t
  _bcontainer_P(_StoreFormat1_NodeListAllocateSize)(
    _bcontainer_P(t) *This,
    uint8_t NodeList
  ){
    return ((uintptr_t)1 << NodeList) * _bcontainer_P(GetNodeSize)(This);
  }
  static
  _bcontainer_P(Node_t) *
  _bcontainer_P(_StoreFormat1_AllocateNodeList)(
    _bcontainer_P(t) *This,
    uint8_t NodeList
  ){
    return (_bcontainer_P(Node_t) *)_bcontainer_P(_mmap)(
      _bcontainer_P(_StoreFormat1_NodeListAllocateSize)(This, NodeList)
    );
  }
#endif

static
_bcontainer_P(Node_t) *
_bcontainer_P(GetNode)
(
  _bcontainer_P(t) *This,
  #if bcontainer_set_PointerNodeType
    _bcontainer_P(Node_t) *node_id
  #else
    bcontainer_set_NodeType node_id
  #endif
){
  #if bcontainer_set_StoreFormat == 0
    return (_bcontainer_P(Node_t) *)((uint8_t *)This->ptr + node_id * _bcontainer_P(GetNodeSize)(This));
  #elif bcontainer_set_StoreFormat == 1
    #if bcontainer_set_PointerNodeType
      return node_id;
    #else
      uint8_t NodeList = _bcontainer_P(_GetNodeListByNodeID)(node_id);
      node_id -= (bcontainer_set_NodeType)1 << NodeList;
      return &This->NodeLists[NodeList][node_id];
    #endif
  #else
    #error ?
  #endif
}

__forceinline
static
bcontainer_set_NodeType
_bcontainer_P(_PageNodeDiv)(
  _bcontainer_P(t) *This
){
  /* TODO 0x1000 is smallest page size in i386. make it dynamic in compile time */

  /* TODO store this in somewhere if below calculation is runtime */

  return 0x1000 / _bcontainer_P(GetNodeSize)(This);
}

#if bcontainer_set_StoreFormat == 1
  __forceinline
  static
  bcontainer_set_NodeType
  _bcontainer_P(_WhatFirstNodeListWouldBe)(
    _bcontainer_P(t) *This
  ){
    #if bcontainer_set_PreserveSome
      bcontainer_set_NodeType ret = _bcontainer_P(_PageNodeDiv)(This);
    #else
      bcontainer_set_NodeType ret = 0;
    #endif
    if(ret < 1){
      ret = 1;
    }

    ret = __compile_time_log2(ret);

    #if bcontainer_set_RuntimePreallocate
      if(ret < 1){
        ret = 1;
      }
    #endif

    return ret;
  }
#endif

__forceinline
static
bcontainer_set_NodeType
_bcontainer_P(WhatFirstWouldBe)(
  _bcontainer_P(t) *This
){
  bcontainer_set_NodeType r;

  #if bcontainer_set_StoreFormat == 0
    #if bcontainer_set_PreserveSome
      #error no
    #endif

    r = 0;
  #elif bcontainer_set_StoreFormat == 1
    #if bcontainer_set_StoreFormat1_StoreNodeList
      #if bcontainer_set_RuntimePreallocate
        #error not implemented
      #endif
      #if bcontainer_set_MultiThread
        #error not implemented
      #endif

      r = 0;
    #else
      r = (bcontainer_set_NodeType)1 << _bcontainer_P(_WhatFirstNodeListWouldBe)(This);
    #endif
  #else
    #error ?
  #endif

  #if bcontainer_set_UseZeroAsInvalid
    if(r < 1){
      r = 1;
    }
  #endif

  return r;
}

static
bcontainer_set_NodeType
_bcontainer_P(GetCapacity)(
  _bcontainer_P(t) *This
){
  #if bcontainer_set_StoreFormat == 0
    #if bcontainer_set_MultiThread
      #error not implemented yet
    #endif

    return This->Possible;
  #elif bcontainer_set_StoreFormat == 1
    #if bcontainer_set_StoreFormat1_StoreNodeList
      #if bcontainer_set_MultiThread
        #error not implemented yet
      #endif
      #if bcontainer_set_RuntimePreallocate
        #error not implemented yet
      #endif

      uint8_t lnl = This->NodeList;
    #else
      #if bcontainer_set_MultiThread
        bcontainer_set_NodeType Current = __atomic_load_n(&This->Current, __ATOMIC_SEQ_CST);
      #else
        bcontainer_set_NodeType Current = This->Current;
      #endif
      uint8_t lnl = _bcontainer_P(_GetNodeListByNodeID)(Current);
      if(Current > (bcontainer_set_NodeType)1 << lnl){
        lnl++;
      }
    #endif

    return
      ((bcontainer_set_NodeType)1 << lnl) -
      ((bcontainer_set_NodeType)1 << _bcontainer_P(_WhatFirstNodeListWouldBe)(This));
  #else
    #error ?
  #endif
}

static
bcontainer_set_NodeType
_bcontainer_P(Usage)(
  _bcontainer_P(t) *This
){
  return
  #if bcontainer_set_StoreFormat == 0
    This->Current
  #elif bcontainer_set_StoreFormat == 1
    This->Current
  #else
    #error ?
  #endif

  #if bcontainer_set_Recycle
    - This->e.p
  #endif

  ;
}

/* TODO port this to other store formats */
#if bcontainer_set_StoreFormat == 0
  static
  bcontainer_set_NodeType
  _bcontainer_P(SizeNormalized)(
    _bcontainer_P(t) *This
  ){
    return This->Current - _bcontainer_P(WhatFirstWouldBe)(This);
  }

  static
  bcontainer_set_NodeType
  _bcontainer_P(Normalize)(
    _bcontainer_P(t) *This,
    #if bcontainer_set_PointerNodeType
      _bcontainer_P(Node_t) *node_id
    #else
      bcontainer_set_NodeType node_id
    #endif
  ){
    return node_id - _bcontainer_P(WhatFirstWouldBe)(This);
  }

  static
  #if bcontainer_set_PointerNodeType
    _bcontainer_P(Node_t) *
  #else
    bcontainer_set_NodeType
  #endif
  _bcontainer_P(Unnormalize)(
    _bcontainer_P(t) *This,
    bcontainer_set_NodeType node_id
  ){
    return node_id + _bcontainer_P(WhatFirstWouldBe)(This);
  }
#endif


#if bcontainer_set_StoreFormat == 0
  static
  _bcontainer_P(Node_t) *
  _bcontainer_P(GetSinglePointer)
  (
    _bcontainer_P(t) *This
  ){
    return This->ptr;
  }
#endif

static
void
_bcontainer_P(Close)
(
  _bcontainer_P(t) *This
){
  #if bcontainer_set_StoreFormat == 0
    _bcontainer_P(_mfree)(This->ptr, This->Possible * _bcontainer_P(GetNodeSize)(This));
  #elif bcontainer_set_StoreFormat == 1
    for(
      uintptr_t i = (uintptr_t)1 << __compile_time_log2(sizeof(bcontainer_set_NodeType) * 8);
      i--;
    ){
      _bcontainer_P(_mfree)(This->NodeLists[i], ((uintptr_t)1 << i) * _bcontainer_P(GetNodeSize)(This));
    }
  #else
    #error ?
  #endif
}

static
void
_bcontainer_P(_SetStuffAfterOpen)
(
  _bcontainer_P(t) *This
){
  #if bcontainer_set_StoreFormat == 0
    This->Current = _bcontainer_P(WhatFirstWouldBe)(This);
  #elif bcontainer_set_StoreFormat == 1
    #if bcontainer_set_StoreFormat1_StoreNodeList
      #if bcontainer_set_RuntimePreallocate
        #error not implemented
      #endif
      #if bcontainer_set_MultiThread
        #error not implemented
      #endif

      This->Current = 0;
      This->Possible = 0;
      This->NodeList = _bcontainer_P(_WhatFirstNodeListWouldBe)(This);
    #else
      This->Current = _bcontainer_P(WhatFirstWouldBe)(This);
    #endif
  #else
    #error
  #endif
}

static
void
_bcontainer_P(Open)
(
  _bcontainer_P(t) *This
  #if !defined(bcontainer_set_NodeData)
    , bcontainer_set_NodeSizeType NodeSize
  #endif
){
  #include "Open.h"
}

#if bcontainer_set_Clear
  static
  void
  _bcontainer_P(Clear)
  (
    _bcontainer_P(t) *This
  ){
    #if bcontainer_set_Recycle
      This->e.p = 0;
    #endif

    _bcontainer_P(_SetStuffAfterOpen)(This);
  }
#endif

static
void
_bcontainer_P(_InformCapacity)(
  _bcontainer_P(t) *This,
  #if bcontainer_set_StoreFormat == 0
    bcontainer_set_NodeType _Possible
  #elif bcontainer_set_StoreFormat == 1
    uint8_t _NodeList
  #else
    #error ?
  #endif
){
  #if bcontainer_set_StoreFormat == 0
    bcontainer_set_NodeType old_capacity =
      _Possible == (bcontainer_set_NodeType)-1 ?
      (bcontainer_set_NodeType)1 << sizeof(bcontainer_set_NodeType) * 8 - 1 :
      _Possible / 2
    ;
    bcontainer_set_NodeType new_capacity = _Possible;
  #elif bcontainer_set_StoreFormat == 1
    bcontainer_set_NodeType old_capacity =
      ((bcontainer_set_NodeType)1 << _NodeList) -
      ((bcontainer_set_NodeType)1 << _bcontainer_P(_WhatFirstNodeListWouldBe)(This));
    bcontainer_set_NodeType new_capacity =
      ((bcontainer_set_NodeType)1 << _NodeList + 1) -
      ((bcontainer_set_NodeType)1 << _bcontainer_P(_WhatFirstNodeListWouldBe)(This));
  #else
    #error ?
  #endif

  bcontainer_set_CapacityUpdateInfo
}

#if bcontainer_set_Recycle
  static
  bcontainer_set_NodeType
  _bcontainer_P(_NewNodeEmpty)(
    _bcontainer_P(t) *This
  ){
    #if bcontainer_set_MultiThread
      #error not implemented
    #endif
    bcontainer_set_NodeType node_id = This->e.c;
    This->e.c = *(bcontainer_set_NodeType *)_bcontainer_P(GetNode)(This,
      #if bcontainer_set_PointerNodeType
        (_bcontainer_P(Node_t ) *)node_id
      #else
        node_id
      #endif
    );
    This->e.p--;
    return node_id;
  }
#endif
static
bcontainer_set_NodeType
_bcontainer_P(_NewNodeAlloc)(
  _bcontainer_P(t) *This
){
  #include "_NewNodeAlloc.h"
}
static
#if bcontainer_set_PointerNodeType
  _bcontainer_P(Node_t) *
#else
  bcontainer_set_NodeType
#endif
_bcontainer_P(NewNode)(
  _bcontainer_P(t) *This
){
  bcontainer_set_NodeType ret;

  if(0){}
  #if bcontainer_set_Recycle
    else if(This->e.p){
      ret = _bcontainer_P(_NewNodeEmpty)(This);
    }
  #endif
  else{
    ret = _bcontainer_P(_NewNodeAlloc)(This);
  }

  #if bcontainer_set_PointerNodeType
    return (_bcontainer_P(Node_t) *)ret;
  #else
    return ret;
  #endif
}

#if bcontainer_set_Recycle
  static
  void
  _bcontainer_P(Recycle)(
    _bcontainer_P(t) *This,
    #if bcontainer_set_PointerNodeType
      _bcontainer_P(Node_t) *node_id
    #else
      bcontainer_set_NodeType node_id
    #endif
  ){
    #if bcontainer_set_MultiThread
      #error not implemented
    #endif

    *(bcontainer_set_NodeType *)_bcontainer_P(GetNode)(This, node_id) = This->e.c;
    This->e.c = (bcontainer_set_NodeType)node_id;
    This->e.p++;
  }
#endif
