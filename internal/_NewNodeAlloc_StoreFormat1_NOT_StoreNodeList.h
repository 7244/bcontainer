#if !bcontainer_set_MultiThread
  #if bcontainer_set_RuntimePreallocate
    #error not gonna be implemented
  #endif

  bcontainer_set_NodeType node_id = This->Current++;
  uint8_t NodeList = _bcontainer_P(_GetNodeListByNodeID)(node_id);
  if(This->NodeLists[NodeList] == NULL){
    _bcontainer_P(_InformCapacity)(This, NodeList);
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
          _bcontainer_P(_InformCapacity)(This, NodeList);
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
        _bcontainer_P(_InformCapacity)(This, NodeList);
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
  uint8_t _sub_NodeList = _bcontainer_P(_GetNodeListByNodeID)(node_id);
  node_id -= (bcontainer_set_NodeType)1 << _sub_NodeList;
  node_id = (bcontainer_set_NodeType)This->NodeLists[_sub_NodeList] + node_id * _bcontainer_P(GetNodeSize)(This);
#endif

return node_id;
