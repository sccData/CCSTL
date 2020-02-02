#include "Allocator.h"

namespace CCSTL {
	template <class T>
	T* allocator<T>::allocate() {
		return static_cast<T*>(alloc::allocate(sizeof(T)));
	}

	template <class T>
	T* allocator<T>::allocate(size_t n) {
		if(n == 0)
			return 0;
		return static_cast<T*>(alloc::allocate(sizeof(T) * n));
	}

	template <class T>
	void allocator<T>::deallocate(T* p) {
		alloc::deallocate(static_cast<void*>(p), sizeof(T));
	}

	template <class T>
	void allocator<T>::deallocate(T* p, size_t n) {
		if(n == 0)
			return;
		alloc::deallocate(static_cast<void*>(p), sizeof(T) * n);
	}

	template <class T>
	void allocator<T>::construct(T* p) {
		new(p) T();
	}

	template <class T>
	template <class T1, class T2>
	void allocator<T>::construct(T1* p, const T2& value) {
		new(p) T(value);
	}

	template <class T>
	void allocator<T>::destroy(T* p) {
		p->~T();
	}

	template <class T>
	void allocator<T>::destroy(T* first, T* last) {
		for(; first != last; ++first) {
			(*first)->~T();
		}
	}

}