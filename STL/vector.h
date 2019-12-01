//
// Created by NING MEI on 2019/11/21.
//

#ifndef VECTOR_H
#define VECTOR_H

#include <memory>
#include <algorithm>
#include "iterator.h"
#include <initializer_list>
#include "trait.h"

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
                alloc.deallocate(start, end_of_storage-start);
        }
        void destroy(iterator start, iterator end) {
            while(start < end)
                alloc.destroy(start++);
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
        size_type size() const { return size_type(end() - begin()); }
        bool empty() const { return begin() == end(); }
        reference operator[](size_type n) { return *(begin() + n); }
        const_reference operator[](size_type n) const { return *(begin() + n); }

        vector(): start(0), finish(0), end_of_storage(0) {}
        vector(size_type n, const T& value) { fill_initialize(n, value); }
        vector(int n, const T& value) { fill_initialize(n, value); }
        vector(long n, const T& value) { fill_initialize(n, value); }
        vector(std::initializer_list<T> li): start(0), finish(0), end_of_storage(0)
        { range_initialize(li.begin(), li.end(), iterator_category(li.begin())); }
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
                alloc.construct(finish, x);
                ++finish;
            } else
                insert_aux(end(), x);
        }

        void pop_back() {
            --finish;
            alloc.destroy(finish);
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
            alloc.destroy(finish);
            return position;
        }


        iterator insert(iterator position, const T& x) {
            size_type n = position - begin();
            if(finish != end_of_storage && position == end()) {
                alloc.construct(finish, x);
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
            iterator result = alloc.allocate(n);
            uninitialized_fill_n(result, n, x);
            return result;
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

    private:
        Alloc alloc;
    };

    template <class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const T& x) {
        // 还有备用空间
        if(finish != end_of_storage) {
            a.construct(finish, *(finish-1));
            ++finish;
            T x_copy = x;
            std::copy_backward(position, finish-2, finish-1);
            *position = x_copy;
        } else { // 无备用空间
            const size_type old_size = size();
            const size_type len = old_size != 0 ? 2*old_size:1;
            iterator new_start = alloc.allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = std::uninitialized_copy(start, position, new_start);
                a.construct(new_finish, x);
                ++new_finish;
                new_finish = std::uninitialized_copy(position, finish, new_finish);
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
                iterator new_start = alloc.allocate(len);
                iterator new_finish = new_start;
                try {
                    new_finish = uninitialized_copy(start, position, new_start);
                    new_finish = uninitialized_fill_n(new_finish, n, x);
                    new_finish = uninitialized_copy(position, finish, new_finish);
                } catch(...) {
                    destroy(new_start, new_finish);
                    a.deallocate(new_start, len);
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
}

#endif //DEMO2_VECTOR_H
