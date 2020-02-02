//
// Created by NING MEI on 2019/11/21.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <memory>
#include "Allocator.h"
#include <algorithm>
#include "iterator.h"
#include <initializer_list>
#include "trait.h"

namespace CCSTL{
    template <class T, class Alloc = allocator<T>>
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
    private:
        iterator start;
        iterator finish;
        iterator end_of_storage;

        typedef Alloc dataAllocator;

        void insert_aux(iterator position, const T& x);
        void deallocate() {
            if(start)
                dataAllocator::deallocate(start, end_of_storage-start);
        }
        void destroy(iterator start, iterator end) {
            while(start < end)
                dataAllocator::destroy(start++);
        }
        void fill_initialize(size_type n, const T& value) {
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }
    public:

        // 构造、复制、析构相关函数
        vector(): start(0), finish(0), end_of_storage(0) {}
        vector(size_type n, const T& value) { fill_initialize(n, value); }
        vector(int n, const T& value) { fill_initialize(n, value); }
        vector(long n, const T& value) { fill_initialize(n, value); }
        vector(std::initializer_list<T> li): start(0), finish(0), end_of_storage(0)
        { range_initialize(li.begin(), li.end(), iterator_category(li.begin())); }
        explicit vector(size_type n) { fill_initialize(n, T()); }
        template <class InputIterator>
        vector(InputIterator first, InputIterator last): start(0), finish(0), end_of_storage(0) {
            range_initialize(first, last, iterator_category(first));
        }
        vector(const vector& v) {
            allocate_and_copy(v.begin(), v.end());
        }
        vector(vector&& v) {
            start = v.start;
            finish = v.finish;
            end_of_storage = v.end_of_storage;
            v.start = v.finish = v.end_of_storage = 0;
        }

        vector& operator=(const vector& v) {
            if(this != &v) {
                allocate_and_copy(v.begin(), v.end());
            }

            return *this;
        }

        vector& operator=(vector&& v) {
            if(this != &v) {
                destroy(start, finish);
                deallocate();
                start = v.start;
                finish = v.finish;
                end_of_storage = v.end_of_storage;
                v.start = v.finish = v.end_of_storage = 0;
            }

            return *this;
        }
        ~vector() {
            destroy(start, finish);
            deallocate();
        }

        // 比较操作
        bool operator==(const vector& v) const;
        bool operator!=(const vector& v) const;

        // 迭代器相关
        iterator begin() { return start; }
        const_iterator begin() const { return start; }
        iterator end() { return finish; }
        const_iterator end() const { return finish; }

        // 与容量相关
        size_type size() const { return size_type(end() - begin()); }
        bool empty() const { return begin() == end(); }
        difference_type capacity() const { return end_of_storage - first; }
        size_type max_size() const { return size_type(-1) / sizeof(T); }
        // 访问元素相关
        reference operator[](size_type n) { return *(begin() + n); }
        const_reference operator[](size_type n) const { return *(begin() + n); }
        reference front() { return *(begin()); }
        reference back() { return *(end() - 1); }

        // 修改容器相关的操作
        // 清空容器, 销毁容器中的所有对象并使容器的size为0, 但不回收容器已有的空间
        void clear() {
            destroy(start, finish);
            finish = start;
        }

        void swap(vector& v) {
            if(this != &v) {
                std::swap(start, v.start);
                std::swap(finish, v.finish);
                std::swap(end_of_storage, v.end_of_storage);
            }
        }

        void push_back(const T& x) {
            if(finish != end_of_storage) {
                dataAllocator::construct(finish, x);
                ++finish;
            } else
                insert_aux(end(), x);
        }

        void pop_back() {
            --finish;
            dataAllocator::destroy(finish);
        }

        iterator erase(iterator first, iterator last) {
            iterator i = std::copy(last, finish, first);
            destroy(i, finish);
            finish = finish - (last - first);
            return first;
        }

        iterator erase(iterator position) {
            if(position + 1 != end())
                copy(position + 1, finish, position);
            --finish;
            dataAllocator::destroy(finish);
            return position;
        }


        iterator insert(iterator position, const T& x) {
            size_type n = position - begin();
            if(finish != end_of_storage && position == end()) {
                dataAllocator::construct(finish, x);
                ++finish;
            } else 
                insert_aux(position, x);
            return begin() + n;
        }

        template <class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last) {
            range_insert(position, first, last, iterator_category(first));
        }

        void insert(iterator position, const_iterator first, const_iterator last);
        void insert(iterator position, size_type n, const T& x);
        void insert(iterator position, int n, const T& x);
    protected:
        iterator allocate_and_fill(size_type n, const T& x) {
            iterator result = dataAllocator::allocate(n);
            std::uninitialized_fill_n(result, n, x);
            return result;
        }
        template <class InputIterator>
        void allocate_and_copy(InputIterator first, InputIterator last) {
            start = dataAllocator::allocate(last - first);
            finish = std::uninitialized_copy(first, last, start);
            end_of_storage = finish;
        }
 
        template <class InputIterator>
        void range_initialize(InputIterator first, InputIterator last, input_iterator_tag) {
            for(; first != last; ++first) {
                push_back(*first);
            }
        }

        template <class InputIterator>
        void range_insert(iterator pos, InputIterator first, InputIterator last,
                          input_iterator_tag);
    };

    template <class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
        // 还有备用空间
        if(finish != end_of_storage) {
            dataAllocator::construct(finish, *(finish-1));
            ++finish;
            T x_copy = x;
            std::copy_backward(position, finish-2, finish-1);
            *position = x_copy;
        } else { // 无备用空间
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2*old_size:1;
            iterator new_start = dataAllocator::allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = std::uninitialized_copy(start, position, new_start);
                dataAllocator::construct(new_finish, x);
                ++new_finish;
                new_finish = std::uninitialized_copy(position, finish, new_finish);
            } catch(...) {
                destroy(new_start, new_finish);
                dataAllocator::deallocate(new_start, len);
                throw;
            }

            destroy(begin(), end());
            deallocate();

            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }


    template <class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T& x) {
        // 插入的元素个数不为0才有效
        if(n != 0) {
            // 备用空间大于等于"新增元素个数"
            if(size_type(end_of_storage - finish) >= n) {
                T x_copy = x;
                const size_type elems_after = finish - position;
                iterator old_finish = finish;
                // 插入点之后的现有元素个数"大于"新增元素个数
                if(elems_after > n) {
                    uninitialized_copy(finish - n, finish, old_finish);
                    finish += n;
                    std::copy_backward(position, old_finish - n, old_finish);
                    finish += elems_after;
                    std::fill(position, old_finish, x_copy);
                } else {
                // 插入点之后的现有元素个数"小于等于"新增元素个数
                    std::uninitialized_fill_n(finish, n - elems_after, x_copy);
                    finish += n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    fill(position, old_finish, x_copy);
                }
            } else {
                const size_type old_size = size();
                const size_type len = old_size + max(old_size, n);
                iterator new_start = dataAllocator::allocate(len);
                iterator new_finish = new_start;
                try {
                    new_finish = uninitialized_copy(start, position, new_start);
                    new_finish = uninitialized_fill_n(new_finish, n, x);
                    new_finish = uninitialized_copy(position, finish, new_finish);
                } catch(...) {
                    destroy(new_start, new_finish);
                    dataAllocator::deallocate(new_start, len);
                    throw;
                }

                destroy(start, finish);
                deallocate();

                start = new_start;
                finish = new_finish;
                end_of_storage = new_start + len;
            }
        }
    }

    template <class T, class Alloc>
    template <class InputIterator>
    void vector<T, Alloc>::range_insert(iterator pos, 
                                        InputIterator first, InputIterator last,
                                        input_iterator_tag) 
    {
        for(; first != last; ++first) {
            pos = insert(pos, *first);
            ++pos;
        }
    }

    template <class T, class Alloc>
    void insert(iterator position, const_iterator first, const_iterator last) {
        range_insert(position, first, last, iterator_category(first));
    }

    template <class T, class Alloc>
    bool vector<T, Alloc>::operator ==(const vector& v) const {
        if(size() != v.size())
            return false;
        else {
            auto p1 = start;
            auto p2 = v.start;
            for(; p1 != finish; ++p1) {
                if(*p1 != *p2)
                    return false;
            }

            return true;
        }
    }

    template <class T, class Alloc>
    bool vector<T, Alloc>::operator !=(const vector& v) const {
        return !(*this == v);
    } 
}

#endif //DEMO2_VECTOR_H
