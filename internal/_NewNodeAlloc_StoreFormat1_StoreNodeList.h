#if bcontainer_set_RuntimePreallocate
  #error not implemented
#endif
#if bcontainer_set_MultiThread
  #error not implemented
#endif

if(EXPECT(This->Current == This->Possible, 0)){
  This->NodeList++;
  uintptr_t size = _bcontainer_P(_StoreFormat1_NodeListAllocateSize)(This, This->NodeList);
  This->NodeLists[This->NodeList] = (_bcontainer_P(Node_t) *)_bcontainer_P(_mmap)(size);
  This->Possible = (uintptr_t)This->NodeLists[This->NodeList];
  This->Current = This->Possible + size;
}

This->Current -= _bcontainer_P(GetNodeSize)(This);

return This->Current;
