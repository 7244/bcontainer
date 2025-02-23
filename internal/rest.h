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
  _bcontainer_P(Node_t) *
  _bcontainer_P(_StoreFormat1_AllocateNodeList)(
    _bcontainer_P(t) *This,
    uint8_t NodeList
  ){
    return (_bcontainer_P(Node_t) *)bcontainer_set_alloc_open(
      ((uintptr_t)1 << NodeList) * _bcontainer_P(GetNodeSize)(This)
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
_bcontainer_P(WhatFirstWouldBe)(
  _bcontainer_P(t) *This
){
  #if bcontainer_set_StoreFormat == 0
    #if bcontainer_set_PreserveSome
      #error no
    #endif

    return 0;
  #elif bcontainer_set_StoreFormat == 1
    #if bcontainer_set_PreserveSome
      /* TODO 0x1000 is smallest page size in i386. make it dynamic in compile time */
      /* it should care smallest or optimal allocation, not page size */

      /* TODO store this in somewhere if below calculation is runtime */

      bcontainer_set_NodeType div = 0x1000 / _bcontainer_P(GetNodeSize)(This);
      if(div < 2){
        div = 2;
      }
      return (bcontainer_set_NodeType)1 << __compile_time_log2(div);
    #else
      return 1;
    #endif
  #else
    #error ?
  #endif
}

/* TODO check WhatFirstWouldBe */
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

static
void
_bcontainer_P(Close)
(
  _bcontainer_P(t) *This
){
  #if bcontainer_set_StoreFormat == 0
    bcontainer_set_alloc_close(This->ptr);
  #elif bcontainer_set_StoreFormat == 1
    for(
      uintptr_t i = (uintptr_t)1 << __compile_time_log2(sizeof(bcontainer_set_NodeType) * 8);
      i--;
    ){
      bcontainer_set_alloc_close(This->NodeLists[i]);
    }
  #else
    #error ?
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
  #if bcontainer_set_StoreFormat == 0
    This->Current = 0;
    This->Possible = 0;
    This->ptr = NULL;
  #elif bcontainer_set_StoreFormat == 1
    This->Current = _bcontainer_P(WhatFirstWouldBe)(This);

    for(
      uintptr_t i = (uintptr_t)1 << __compile_time_log2(sizeof(bcontainer_set_NodeType) * 8);
      i--;
    ){
      This->NodeLists[i] = NULL;
    }

    #if bcontainer_set_RuntimePreallocate
      #if bcontainer_set_PreserveSome
        This->Possible = _bcontainer_P(WhatFirstWouldBe)(This) >> 1;
      #else
        This->Possible = _bcontainer_P(WhatFirstWouldBe)(This);
      #endif

      #if bcontainer_set_MultiThread
        _bcontainer_P(_FastLock_Init)(&This->AllocateLock);
      #endif
    #else
      #if bcontainer_set_MultiThread
        for(
          uintptr_t i = (uintptr_t)1 << __compile_time_log2(sizeof(bcontainer_set_NodeType) * 8);
          i--;
        ){
          bcontainer_P(_FastLock_Init)(&This->NodeListsLocks[i]);
        }
      #endif
    #endif
  #else
    #error ?
  #endif

  #if !defined(bcontainer_set_NodeData)
    This->NodeSize = _bcontainer_P(_PadGivenNodeSize)(This, NodeSize);
  #endif

  #if bcontainer_set_Recycle
    This->e.p = 0;
    #if __sanit
      This->e.c = 0;
    #endif
  #endif
}
static
void
_bcontainer_P(Clear)
(
  _bcontainer_P(t) *This
){
  This->Current = 0;

  #if bcontainer_set_Recycle
    This->e.p = 0;
  #endif
}

#if bcontainer_set_StoreFormat == 0
  static
  void
  _bcontainer_P(_SetPossible)(
    _bcontainer_P(t) *bcontainer,
    bcontainer_set_NodeType Possible
  ){
    bcontainer_set_PossibleUpdate
    bcontainer->Possible = Possible;
  }

  static
  void
  _bcontainer_P(SetPossibleWith)(
    _bcontainer_P(t) *This,
    bcontainer_set_NodeType Size
  ){
    _bcontainer_P(_SetPossible)(This, ((uintptr_t)2 << sizeof(uintptr_t) * 8 - __clz(Size | 1)) - 1);
  }

  static
  void
  _bcontainer_P(_Resize)(
    _bcontainer_P(t) *This
  ){
    for(uint32_t iRetry = 0; iRetry < bcontainer_set_alloc_RetryAmount; iRetry++){
      void *np = bcontainer_set_alloc_resize(
        This->ptr,
        (uintptr_t)This->Possible * _bcontainer_P(GetNodeSize)(This)
      );
      if(np == NULL){
        continue;
      }
      This->ptr = (_bcontainer_P(Node_t) *)np;
      return;
    }
    __abort();
  }

  static
  void
  _bcontainer_P(Reserve)
  (
    _bcontainer_P(t) *This,
    bcontainer_set_NodeType Amount
  ){
    _bcontainer_P(_SetPossible)(This, Amount);
    _bcontainer_P(_Resize)(This);
  }

  static
  void
  _bcontainer_P(_AllocateBufferWith)
  (
    _bcontainer_P(t) *This,
    bcontainer_set_NodeType Amount
  ){
    _bcontainer_P(SetPossibleWith)(This, Amount);
    _bcontainer_P(_Resize)(This);
  }
#endif

#if bcontainer_set_StoreFormat == 0
  static
  void
  _bcontainer_P(AddEmpty)
  (
    _bcontainer_P(t) *This,
    bcontainer_set_NodeType Amount
  ){
    bcontainer_set_NodeType Current = This->Current + Amount;
    if(Current >= This->Possible){
      _bcontainer_P(_AllocateBufferWith)(This, Current);
    }
    This->Current = Current;
  }

  static
  void
  _bcontainer_P(Add)
  (
    _bcontainer_P(t) *This,
    _bcontainer_P(Node_t) *Node
  ){
    if(This->Current == This->Possible){
      _bcontainer_P(_AllocateBufferWith)(This, This->Current);
    }

    #ifdef bcontainer_set_NodeData
      This->ptr[This->Current] = *Node;
    #else
      __MemoryCopy(Node, _bcontainer_P(GetNode)(This, This->Current), This->NodeSize);
    #endif
    ++This->Current;
  }
#endif

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
  #if bcontainer_set_StoreFormat == 0
    #if bcontainer_set_MultiThread
      #error not gonna be implemented
    #endif
    #if bcontainer_set_RuntimePreallocate
      #error not gonna be implemented
    #endif

    _bcontainer_P(AddEmpty)(This, 1);

    return This->Current - 1;
  #elif bcontainer_set_StoreFormat == 1
    #if !bcontainer_set_MultiThread
      #if bcontainer_set_RuntimePreallocate
        #error not gonna be implemented
      #endif
      #if bcontainer_set_PreserveSome
        #error not gonna be implemented
      #endif

      bcontainer_set_NodeType node_id = This->Current++;
      uint8_t NodeList = _bcontainer_P(_GetNodeListByNodeID)(node_id);
      if(This->NodeLists[NodeList] == NULL){
        This->NodeLists[NodeList] = _bcontainer_P(_StoreFormat1_AllocateNodeList)(This, NodeList);
      }
    #else
      #if bcontainer_set_RuntimePreallocate
        bcontainer_set_NodeType node_id = __atomic_fetch_add(&This->Current, 1, __ATOMIC_SEQ_CST);
        bcontainer_set_NodeType p = __atomic_load_n(&This->Possible, __ATOMIC_RELAXED);
        if(node_id + 0xc00 * !bcontainer_set_PreserveSome >= p){
          while(1){
            if(_bcontainer_P(_FastLock_LockDontCountFail)(&This->AllocateLock)){
              p = __atomic_load_n(&This->Possible, __ATOMIC_SEQ_CST);
              if(node_id < p << !!bcontainer_set_PreserveSome){
                break;
              }
              _bcontainer_P(_FastLock_CountFail)(&This->AllocateLock);
              continue;
            }
            bcontainer_set_NodeType c = node_id;
            while(c + 0xc00 * !bcontainer_set_PreserveSome >= This->Possible){
              uint8_t NodeList = _bcontainer_P(_GetNodeListByNodeID)(This->Possible << !!bcontainer_set_PreserveSome);
              __atomic_exchange_n(
                &This->NodeLists[NodeList],
                _bcontainer_P(_StoreFormat1_AllocateNodeList)(This, NodeList),
                __ATOMIC_SEQ_CST
              );
              __atomic_exchange_n(&This->Possible, This->Possible << 1, __ATOMIC_RELAXED);
              c = __atomic_load_n(&This->Current, __ATOMIC_RELAXED);
            }
            _bcontainer_P(_FastLock_Unlock)(&This->AllocateLock);
            break;
          }
        }
      #else
        #if bcontainer_set_PreserveSome
          #error not gonna be implemented
        #endif

        bcontainer_set_NodeType node_id = __atomic_fetch_add(&This->Current, 1, __ATOMIC_SEQ_CST);
        uint8_t NodeList = _bcontainer_P(_GetNodeListByNodeID)(node_id);
        if(__atomic_load_n(&This->NodeLists[NodeList], __ATOMIC_RELAXED) == NULL){
          while(_bcontainer_P(_FastLock_Lock)(&This->NodeListsLocks[NodeList])){ /* TOOD cpu relax */ }
          if(__atomic_load_n(&This->NodeLists[NodeList], __ATOMIC_SEQ_CST) == NULL){
            __atomic_exchange_n(
              &This->NodeLists[NodeList],
              _bcontainer_P(_StoreFormat1_AllocateNodeList)(This, NodeList),
              __ATOMIC_SEQ_CST
            );
          }
          _bcontainer_P(_FastLock_Unlock)(&This->NodeListsLocks[NodeList]);
        }
      #endif
    #endif

    #if bcontainer_set_PointerNodeType
      node_id -= (bcontainer_set_NodeType)1 << NodeList;
      node_id = (bcontainer_set_NodeType)This->NodeLists[NodeList] + node_id * _bcontainer_P(GetNodeSize)(This);
    #endif

    return node_id;
  #else
    #error ?
  #endif
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
