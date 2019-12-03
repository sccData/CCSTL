#ifndef DEQUE_H
#define DEQUE_H
#include <memory>
#include "Iterator.h"

namespace CCSTL{
	// 如果n不为0, 传回n, 表示buffer_size由使用者自定
	// 如果n为0, 表示buffer_size使用预设值, 那么
	// 如果sz(sizeof(value_type))小于512, 传回512/sz,
	// 如果sz不小于512, 传回1
	inline size_t __deque_buf_size(size_t n, size_t sz) {
		return n != 0 ? n : (sz < 512 ? size_t(512 / sz) : size_t(1));
	}
	template <class T, size_t BufSiz>
	struct deque_iterator {
		typedef deque_iterator<T> iterator;
		typedef deque_iterator<const T> const_iterator;
		static size_t buffer_size() { return __deque_buf_size(BufSiz, sizeof(T)); }

		typedef random_access_iterator_tag iterator_category;
		typedef T value_type;
		typedef T* pointer;
		typedef T& reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T** map_pointer;

		typedef deque_iterator self;

		pointer cur;
		pointer first;
		pointer last;
		map_pointer node;

		deque_iterator(T* x, map_pointer y):
			cur(x), first(*y), last(*y + buffer_size()), node(y) {}
		deque_iterator():cur(0), first(0), last(0), node(0) {}
		deque_iterator(const iterator& x):
			cur(x.cur), first(x.first), last(x.last), node(x.node) {}

		reference operator*() const { return *cur; }
		pointer operator->() const { return &(operator*()); }

		difference_type operator-(const self& x) const {
			return difference_type(buffer_size()) * (node - x.node - 1) +
				(cur - first) + (x.last - cur);
		}

		self& operator++() {
			++cur;
			if(cur == last) {
				set_node(node + 1);
				cur = first;
			}
			return *this;
		}

		self operator++(int) {
			self tmp = *this;
			++*this;
			return tmp;
		}

		self& operator--() {
			if(cur == first) {
				set_node(node - 1);
				cur = last;
			}
			--cur;
			return *this;
		}

		self operator--(int) {
			self tmp = *this;
			--*this;
			return tmp;
		}

		self& operator+=(difference_type n) {
			difference_type offset = n + (cur - first);
			if(offset >= 0 && offset < difference_type(buffer_size())) 
				cur += n;
			else {
				difference_type node_offset = 
					offset > 0 ? offset/difference_type(buffer_size())
							   : -difference_type((-offset - 1) / buffer_size()) - 1;
				set_node(node + node_offset);
				cur = first + (offset - node_offset * difference_type(buffer_size()));
			}

			return *this;
		}

		self operator+(difference_type n) const {
			self tmp = *this;
			return tmp += n;
		}

		self& operator-=(difference_type n) { return *this += -n; }

		self operator-(difference_type n) const {
			self tmp = *this;
			return tmp -= n;
		}

		reference operator[](difference_type n) const { return *(*this + n); }

		bool operator==(const self& x) const { return cur == x.cur; }
		bool operator!=(const self& x) const { return !(*this == x); }
		bool operator<(const self& x) const {
			return (node == x.node)?(cur < x.cur):(node < x.node);
		}

		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = first + difference_type(buffer_size());
		}
	};

	template <class T, class Alloc = std::allocator<T>, size_t BufSiz = 0>
	class deque {
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& cost_reference;
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

	public:
		typedef deque_iterator<T> iterator;
		typedef deque_iterator<const T> const_iterator;
	protected:
		typedef pointer* map_pointer;

		static size_type buffer_size() {
			return __deque_buf_size(BufSiz, sizeof(value_type));
		}

		static size_type initial_map_size() { return 8; }
	protected:
		iterator start;
		iterator finish;

		map_pointer map;
		size_type map_size;
	public:
		iterator begin() { return start; }
		iterator end() { return finish; }
		const_iterator begin() const { return start; }
		const_iterator end() const { return finish; }

		reference operator[](size_type n) { return start[difference_type(n)]; }
		const_reference operator[](size_type n) { return start[difference_type(n)]; }

		reference front() { return *start; }
		reference back() {
			iterator tmp = finish;
			--tmp;
			return *tmp;
		}

		const_reference front() const { return *start; }
		const_reference back() const {
			const_iterator tmp = finish;
			--tmp;
			return *tmp;
		}

		size_type size() const { return finish - start; }
		size_type max_size() const { return size_type(-1); }
		bool empty() const { return finish == start; }
	};
}

#endif