# Slab Memory manager C++
This is alternative implementation of slab memory manager, which frequently used in UNIX based systems kernels.  
  
Runtime performance in cases of frequent allocation/deallocation of small memory chunks improved by 5-10\%.  
Memory fragmentation decreased.   
 
Implementation is header-only, so, the only thing you need to do is to download the `slab_allocator.hpp` file and 
include it to your C++ code.  

Also the tests `smoke_test.cpp` helping to understand the usage are provided.