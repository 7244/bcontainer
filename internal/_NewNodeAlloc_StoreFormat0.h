#if bcontainer_set_MultiThread
  #error not gonna be implemented
#endif
#if bcontainer_set_RuntimePreallocate
  #error not gonna be implemented
#endif

_bcontainer_P(AddEmpty)(This, 1);

return This->Current - 1;
