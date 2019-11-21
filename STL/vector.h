#ifndef VECTOR_H
#define VECTOR_H
#include <memory>
#include <algorithm>
#include "Iterator.h"
#include <initializer_list>
#include "Trait.h"
namespace CCSTL{
	template <class T, class Alloc = std::allocator<T>>
	class vector {
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type* iterator;
		typedef const value_type* const_iterator;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
	protected:
		iterator start;
		iterator finish;
		iterator end_of_storage;
		void insert_aux(iterator position, const T& x);
		void deallocate() {
			if(start)	
				a.deallocate(start, end_of_storage-start);
		}
		void destroy(iterator start, iterator end) {
			while(start < end)
				a.destroy(start++);
		}
		void fill_initialize(size_type n, const T& value) {
			start = allocate_and_fill(n, value);
			finish = start + n;
			end_of_storage = finish;
		}
	public:
		iterator begin() { return start; }
		const_iterator begin() const { return start; }
		iterator end() { return finish; }
		const_iterator end() const { return finish; }
		size_type size() const { return size_type(end() - start()); } 
		bool empty() const { return begin() == end(); }
		reference operator[](size_type n) { return *(begin() + n); }
		const_reference operator[](size_type n) const { return *(begin() + n); }

		vector(): start(0), finish(0), end_of_storage(0) {}
		vector(size_type n, const T& value) { fill_initialize(n, value); }
		vector(int n, const T& value) { fill_initialize(n, value); }
		vector(long n, const T& value) { fill_initialize(n, value); }
		vector(std::initializer_list<T> l) { range_initialize(l.begin(), l.end(), iterator_category(l.begin())); }
		explicit vector(size_type n) { fill_initialize(n, T()); }

		template <class InputIterator>
		vector(InputIterator first, InputIterator last): start(0), finish(0), end_of_storage(0)
		{
			range_initialize(first, last, iterator_category(first));
		}

		~vector() {
			destroy(start, finish);
			deallocate();
		}

		void push_back(const T& x) {
			if(finish != end_of_storage) {
				a.construct(finish, x);
				++finish;
			} else 
				insert_aux(end(), x);
		}
	protected:
		iterator allocate_and_fill(size_type n, const T& x) {
			iterator result = a.allocate(n);
			uninitialized_fill_n(result, n, x);
			return result;
		}

		template <class InputIterator>
		void range_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
			for(; first != last; ++first) {
				push_back(*first);
			}
		}

	private:
		Alloc a;
	};

	template <class T, class Alloc>
	void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
		// 还有备用空间
		if(finish != end_of_storage) {
			a.construct(finish, *(finish-1));
			++finish;
			T x_copy = x;
			copy_backward(position, finish-2, finish-1);
			*position = x_copy;
		} else { // 无备用空间
			const size_type old_size = size();
			const size_type len = old_size != 0 ? 2*old_size:1;
			iterator new_start = a.allocate(len);
			iterator new_finish = new_start;
			try {
				new_finish = uninitialized_copy(start, position, new_start);
				a.construct(new_finish, x);
				++new_finish;
				new_finish = uninitialized_copy(position, finish, new_finish);
			} catch(...) {
				destroy(new_start, new_finish);
				a.deallocate(new_start, len);
				throw;
			}

			destroy(begin(), end());
			deallocate();

			start = new_start;
			finish = new_finish;
			end_of_storage = new_start + len;
		}
	}
}

#endif