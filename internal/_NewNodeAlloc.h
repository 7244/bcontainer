#if bcontainer_set_StoreFormat == 0
  #include "_NewNodeAlloc_StoreFormat0.h"
#elif bcontainer_set_StoreFormat == 1
  #include "_NewNodeAlloc_StoreFormat1.h"
#else
  #error ?
#endif
