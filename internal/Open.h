#if bcontainer_set_StoreFormat == 0
  This->Possible = 0;
  This->ptr = NULL;
#elif bcontainer_set_StoreFormat == 1
  for(
    uintptr_t i = (uintptr_t)1 << __compile_time_log2(sizeof(bcontainer_set_NodeType) * 8);
    i--;
  ){
    This->NodeLists[i] = NULL;
  }

  #if bcontainer_set_StoreFormat1_StoreNodeList
    #if bcontainer_set_RuntimePreallocate
      #error not implemented
    #endif
    #if bcontainer_set_MultiThread
      #error not implemented
    #endif

    /* all is in _SetStuffAfterOpen */
  #else
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

_bcontainer_P(_SetStuffAfterOpen)(This);
