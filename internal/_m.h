static void *_bcontainer_P(_mmap)(
  uintptr_t size
){
  for(uint32_t iRetry = 0; iRetry < bcontainer_set_alloc_RetryAmount; iRetry++){
    #if bcontainer_set_use_mmap
      void *new_ptr = __generic_mmap(size);
      if((uintptr_t)new_ptr > (uintptr_t)-4096){
        continue;
      }
    #else
      void *new_ptr = bcontainer_set_alloc_open(size);
      if(new_ptr == NULL){
        continue;
      }
    #endif
    return new_ptr;
  }
  __abort();
}
static void *_bcontainer_P(_mremap)(
  void *ptr,
  uintptr_t old_size,
  uintptr_t new_size
){
  for(uint32_t iRetry = 0; iRetry < bcontainer_set_alloc_RetryAmount; iRetry++){
    #if bcontainer_set_use_mmap
      void *new_ptr = __generic_mremap(ptr, old_size, new_size);
      if((uintptr_t)new_ptr > (uintptr_t)-4096){
        continue;
      }
    #else
      void *new_ptr = bcontainer_set_alloc_resize(ptr, new_size);
      if(new_ptr == NULL){
        continue;
      }
    #endif
    return new_ptr;
  }
  __abort();
}
static void *_bcontainer_P(_mmalloc)(
  uintptr_t size
){
  if(size){
    return _bcontainer_P(_mmap)(size);
  }
  else{
    return NULL;
  }
}
static void *_bcontainer_P(_mrealloc)(
  void *ptr,
  uintptr_t old_size,
  uintptr_t new_size
){
  if(ptr == NULL){
    return _bcontainer_P(_mmap)(new_size);
  }
  else{
    return _bcontainer_P(_mremap)(ptr, old_size, new_size);
  }
}
static void _bcontainer_P(_munmap)(
  void *ptr,
  uintptr_t size
){
  #if bcontainer_set_use_mmap
    __generic_munmap(ptr, size);
  #else
    bcontainer_set_alloc_close(ptr);
  #endif
}
static void _bcontainer_P(_mfree)(
  void *ptr,
  uintptr_t size
){
  if(ptr != NULL){
    _bcontainer_P(_munmap)(ptr, size);
  }
}
