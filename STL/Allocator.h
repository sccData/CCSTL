#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "Alloc.h"
#include <new>

namespace CCSTL {
	template <class T>
	class allocator {
	public:
		typedef T value_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
	public:
		static T* allocate();
		static T* allocate(size_t n);
		static void deallocate(T* p);
		static void deallocate(T* p, size_t n);

		static void construct(T* p);
		template <class T1, class T2>
		static void construct(T1* p, const T2& value);
		static void destroy(T* p);
		static void destroy(T* first, T* last);
	};
}
#endif