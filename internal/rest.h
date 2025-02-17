#if defined(bcontainer_set_MultipleType_Sizes)
  inline
  static
  const
  bcontainer_set_NodeSizeType
  _bcontainer_P(_MultipleType_Sizes)[] = {bcontainer_set_MultipleType_Sizes};
  #define _bcontainer_MultipleType_Amount \
    (sizeof(_bcontainer_P(_MultipleType_Sizes)) / sizeof(_bcontainer_P(_MultipleType_Sizes)[0]))
#elif defined(bcontainer_set_MultipleType_SizeArray)
  inline
  static
  const
  auto
  _bcontainer_P(_MultipleType_Sizes) = bcontainer_set_MultipleType_SizeArray;
  #define _bcontainer_MultipleType_Amount \
    _bcontainer_P(_MultipleType_Sizes).size()
#elif defined(bcontainer_set_NodeData)
  typedef bcontainer_set_NodeData _bcontainer_P(Node_t);
#else
  typedef void _bcontainer_P(Node_t);
#endif

typedef struct{
  bcontainer_set_NodeType Current;
  bcontainer_set_NodeType Possible;
  #if defined(bcontainer_set_MultipleType)
    uint8_t *ptr[_bcontainer_MultipleType_Amount];
  #else
    _bcontainer_P(Node_t) *ptr;
    #ifndef bcontainer_set_NodeData
      bcontainer_set_NodeSizeType NodeSize;
      #if bcontainer_set_BufferingFormat == 0
        bcontainer_set_NodeType BufferAmount;
      #endif
    #endif
  #endif
}_bcontainer_P(t);

static
bcontainer_set_NodeSizeType
_bcontainer_P(GetNodeSize)
(
  _bcontainer_P(t) *List
  #if defined(bcontainer_set_MultipleType)
    , uintptr_t PointerIndex
  #endif
){
  #if defined(bcontainer_set_MultipleType)
    return _bcontainer_P(_MultipleType_Sizes)[PointerIndex];
  #elif defined(bcontainer_set_NodeData)
    return sizeof(bcontainer_set_NodeData);
  #else
    return List->NodeSize;
  #endif
}

static
#if defined(bcontainer_set_MultipleType)
  void *
#else
  _bcontainer_P(Node_t) *
#endif
_bcontainer_P(GetNode)
(
  _bcontainer_P(t) *List,
  bcontainer_set_NodeType NR
  #if defined(bcontainer_set_MultipleType)
    , uintptr_t PointerIndex
  #endif
){
  #if defined(bcontainer_set_MultipleType)
    return (void *)(List->ptr[PointerIndex] + NR * _bcontainer_P(GetNodeSize)(List, PointerIndex));
  #elif defined(bcontainer_set_NodeData)
    return &List->ptr[NR];
  #else
    return (_bcontainer_P(Node_t) *)&((uint8_t *)List->ptr)[NR * _bcontainer_P(GetNodeSize)(List)];
  #endif
}

static
void
_bcontainer_P(Close)
(
  _bcontainer_P(t) *List
){
  #if bcontainer_set_HandleAllocate == 1
    #if defined(bcontainer_set_MultipleType)
      for(uintptr_t i = 0; i < _bcontainer_MultipleType_Amount; i++){
        bcontainer_set_alloc_close(List->ptr[i]);
      }
    #else
      bcontainer_set_alloc_close(List->ptr);
    #endif
  #endif
}
static
void
_bcontainer_P(Open)
(
  _bcontainer_P(t) *List
  #if !defined(bcontainer_set_MultipleType)
    #ifndef bcontainer_set_NodeData
      , bcontainer_set_NodeSizeType NodeSize
    #endif
  #endif
){
  List->Current = 0;
  List->Possible = 0;
  #if defined(bcontainer_set_MultipleType)
    for(uintptr_t i = 0; i < _bcontainer_MultipleType_Amount; i++){
      List->ptr[i] = NULL;
    }
  #else
    List->ptr = NULL;
    #ifndef bcontainer_set_NodeData
      List->NodeSize = NodeSize;
      #if bcontainer_set_BufferingFormat == 0
        List->BufferAmount = bcontainer_set_WantedBufferByteAmount / List->NodeSize;
        if(List->BufferAmount == 0){
          List->BufferAmount = 1;
        }
      #endif
    #endif
  #endif
}
static
void
_bcontainer_P(Clear)
(
  _bcontainer_P(t) *List
){
  List->Current = 0;
}

static
void
_bcontainer_P(ClearWithBuffer)
(
  _bcontainer_P(t) *List
){
  #if bcontainer_set_HandleAllocate == 1
    /* a lot shorter to call this */
    _bcontainer_P(Close)(List);
  #endif

  List->Current = 0;
  List->Possible = 0;
  #if defined(bcontainer_set_MultipleType)
    for(uintptr_t i = 0; i < _bcontainer_MultipleType_Amount; i++){
      List->ptr[i] = NULL;
    }
  #else
    List->ptr = NULL;
  #endif
}

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
  _bcontainer_P(t) *List,
  bcontainer_set_NodeType Size
){
  _bcontainer_P(_SetPossible)(List,
  #if bcontainer_set_BufferingFormat == 0
    #ifdef bcontainer_set_NodeData
      Size + bcontainer_BufferAmount
    #else
      Size + List->BufferAmount
    #endif
  #elif bcontainer_set_BufferingFormat == 1
    ((uintptr_t)2 << sizeof(uintptr_t) * 8 - __clz(Size | 1)) - 1
  #else
    #error ?
  #endif
  );
}

static
void
_bcontainer_P(SetPointer)(
  _bcontainer_P(t) *List,
  void *Pointer
){
  List->ptr = (_bcontainer_P(Node_t) *)Pointer;
}

#if bcontainer_set_HandleAllocate == 1
  static
  void
  _bcontainer_P(_Resize)(
    _bcontainer_P(t) *List
  ){
    #if defined(bcontainer_set_MultipleType)
      for(uintptr_t i = 0; i < _bcontainer_MultipleType_Amount; i++){
        for(uint32_t iRetry = 0; iRetry < bcontainer_set_alloc_RetryAmount; iRetry++){
          void *np = bcontainer_set_alloc_resize(
            List->ptr[i],
            (uintptr_t)List->Possible * _bcontainer_P(_MultipleType_Sizes)[i]
          );
          if(np == NULL){
            continue;
          }
          List->ptr[i] = (uint8_t *)np;
          goto gt_NextType;
        }
        __abort();
        gt_NextType:;
      }
    #else
      for(uint32_t iRetry = 0; iRetry < bcontainer_set_alloc_RetryAmount; iRetry++){
        void *np = bcontainer_set_alloc_resize(
          List->ptr,
          (uintptr_t)List->Possible * _bcontainer_P(GetNodeSize)(List)
        );
        if(np == NULL){
          continue;
        }
        List->ptr = (_bcontainer_P(Node_t) *)np;
        return;
      }
      __abort();
    #endif
  }

  static
  void
  _bcontainer_P(Reserve)
  (
    _bcontainer_P(t) *List,
    bcontainer_set_NodeType Amount
  ){
    _bcontainer_P(_SetPossible)(List, Amount);
    _bcontainer_P(_Resize)(List);
  }

  static
  void
  _bcontainer_P(_AllocateBufferWith)
  (
    _bcontainer_P(t) *List,
    bcontainer_set_NodeType Amount
  ){
    _bcontainer_P(SetPossibleWith)(List, Amount);
    _bcontainer_P(_Resize)(List);
  }
#endif

#if !defined(bcontainer_set_MultipleType)
  static
  void
  _bcontainer_P(Add)
  (
    _bcontainer_P(t) *List,
    _bcontainer_P(Node_t) *Node
  ){
    #if bcontainer_set_HandleAllocate == 1
      if(List->Current == List->Possible){
        _bcontainer_P(_AllocateBufferWith)(List, List->Current);
      }
    #endif

    #ifdef bcontainer_set_NodeData
      List->ptr[List->Current] = *Node;
    #else
      __MemoryCopy(Node, _bcontainer_P(GetNode)(List, List->Current), List->NodeSize);
    #endif
    ++List->Current;
  }
#endif

static
void
_bcontainer_P(AddEmpty)
(
  _bcontainer_P(t) *List,
  bcontainer_set_NodeType Amount
){
  bcontainer_set_NodeType Current = List->Current + Amount;
  #if bcontainer_set_HandleAllocate == 1
    if(Current >= List->Possible){
      _bcontainer_P(_AllocateBufferWith)(List, Current);
    }
  #endif
  List->Current = Current;
}

#if defined(bcontainer_set_MultipleType)
  #undef _bcontainer_MultipleType_Amount
#endif
