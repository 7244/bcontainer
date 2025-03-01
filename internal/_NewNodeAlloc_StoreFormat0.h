#if bcontainer_set_MultiThread
  #error not gonna be implemented
#endif
#if bcontainer_set_RuntimePreallocate
  #error not gonna be implemented
#endif

if(This->Current == This->Possible){
  bcontainer_set_NodeType old_possible = This->Possible;

  uint8_t clz;
  if(old_possible){
    clz = _bcontainer_P(_clz_nodeid)(old_possible);
  }
  else{
    clz = sizeof(bcontainer_set_NodeType) * 8;
  }

  This->Possible =
    !clz ?
    (bcontainer_set_NodeType)-1 :
    ((bcontainer_set_NodeType)1 << sizeof(bcontainer_set_NodeType) * 8 - clz)
  ;
  _bcontainer_P(_InformCapacity)(
    This,
    This->Possible
  );

  This->ptr = (_bcontainer_P(Node_t) *)_bcontainer_P(_mrealloc)(
    This->ptr,
    (uintptr_t)old_possible * _bcontainer_P(GetNodeSize)(This),
    (uintptr_t)This->Possible * _bcontainer_P(GetNodeSize)(This)
  );
}

return This->Current++;
