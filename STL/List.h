#ifndef LIST_H
#define LIST_H
namespace CCSTL{

    template <class T>
    struct list_node {
        typedef list_node* node_pointer;
        node_pointer prev;
        node_pointer next;
        T data;
        list_node(const T& d, node_pointer p=nullptr, node_pointer n=nullptr):
                data(d), prev(p), next(n) {}
    };

    template <class T>
    struct list_iterator {
        typedef list_iterator<T> self;

        typedef T value_type;
        typedef T* pointer;
        typedef T& reference;
        typedef list_node<T>* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;

        link_type node;         // 指向list的节点

        list_iterator(link_type ptr = nullptr):node(ptr) {}
        list_iterator() = default;
        list_iterator(const iterator& x):node(x.node) {}

        bool operator==(const self& x) const { return node == x.node; }
        bool operator!=(const self& x) const { return node != x.node; }

        self& operator++() {
            node = node->next;
            return *this;
        }
        self operator++(int) {
            self tmp = *this;
            ++*this;
            return tmp;
        }
        self& operator--() {
            node = node->prev;
            return *this;
        }
        self operator--(int) {
            self tmp = *this;
            --*this;
            return tmp;
        }
        reference operator*() const { return node->data; }
        pointer operator->() const { return &(operator*()); }
    };

    template <class T, class Alloc = std::allocator<T>>
    class list {
    protected:
        typedef list_node<T> list_node;
        typedef std::allocator<list_node> list_node_allocator;
    public:
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef list_node* link_type;
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
    protected:
        link_type get_node() { return alloc.allocate(1); }
        void put_node(link_type p) { alloc.deallocate(p, 1); }
        link_type create_node(const T& x) {
            link_type p = get_node();
            alloc.construct(p, x);
            return p;
        }

        void destroy_node(link_type p) {
            alloc.destroy(p);
            put_node(p);
        }
    protected:
        void empty_initialize() {
            node = get_node();
            node->next = node;
            node->prev = node;
        }

        void fill_initialize(size_type n, const T& value) {
            empty_initialize();
            insert(begin(), n, value);
        }

        template <class InputIterator>
        void range_initialize(InputIterator first, InputIterator last) {
            empty_initialize();
            insert(begin(), first, last);
        }

    public:
        typedef list_iterator<T> iterator;
        typedef list_iterator<const T> const_iterator;
    protected:
        void transfer(iterator position, iterator first, iterator last) {
            if(position != last) {
                (*((*last.node).prev)).next = position.node;
                (*((*first.node).prev)).next = last.node;
                (*((*position.node).prev)).next = first.node;
                link_type tmp = (*position.node).prev;
                (*position.node).prev = (*last.node).prev;
                (*last.node).prev = (*first.node).prev;
                (*first.node).prev = tmp;
            }
        }

    public:

        list() { empty_initialize(); }
        list(size_type n, const T& value) { fill_initialize(n, value); }
        list(int n, const T& value) { fill_initialize(n, value); }
        list(long n, const T& value) { fill_initialize(n, value); }
        explicit list(size_type n) { fill_initialize(n, T()); }

        template <class InputIterator>
        list(InputIterator first, InputIterator last) {
            range_initialize(first, last);
        }

        list(const list<T, Alloc>& x) {
            range_initialize(x.begin(), x.end());
        }
        ~list() {
            clear();
            put_node(node);
        }

        iterator begin() { return node->next; }
        const_iterator begin() const { return node->next; }
        iterator end() { return node; }
        const_iterator end() const { return node; }
        bool empty() const { return node->next == node; }
        iterator insert(iterator position, const T& x) {
            link_type tmp = create_node(x);
            tmp->next = position.node;
            tmp->prev = position.node->prev;
            position.node->prev->next = tmp;
            position.node->prev = tmp;
            return tmp;
        }

        void insert(iterator pos, size_type n, const T& x);
        void insert(iterator pos, int n, const T& x);
        void insert(iterator pos, long n, const T& x);

        template <class InputIterator>
        void insert(iterator positionm, InputIterator first, InputIterator last);

        void push_front(const T& x) { insert(begin(), x); }
        void push_back(const T& x) { insert(end(), x); }
        iterator erase(iterator position) {
            link_type next_node = position.node->next;
            link_type prev_node = position.node->prev;
            prev_node->next = next_node;
            next_node->prev = prev_node;
            destroy_node(position.node);
            return iterator(next_node);
        }

        void pop_front() {
            erase(begin());
        }

        void pop_back() {
            iterator tmp = end();
            erase(--tmp);
        }

        void remove(const T& x);
        void unique();
        void clear();

        void splice(iterator position, list&, iterator first, iterator last) {
            if(first != last)
                transfer(position, first, last);
        }

    protected:
        link_type node;
    private:
        list_node_allocator alloc;
    };

    template <class T, class Alloc>
    template <class InputIterator>
    void list<T, Alloc>::insert(iterator position,
                                InputIterator first, InputIterator last) {
        for(; first != last; ++first) {
            insert(position, *first);
        }
    }

    template <class T, class Alloc>
    void list<T, Alloc>::insert(iterator position, size_type n, const T& x) {
        for(; n>0; --n)
            insert(position, x);
    }

    template <class T, class Alloc>
    void list<T, Alloc>::insert(iterator position, int n, const T& x) {
        insert(position, (int)n, x);
    }

    template <class T, class Alloc>
    void list<T, Alloc>::insert(iterator position, long n, const T& x) {
        insert(position, (long)n, x);
    }

    template <class T, class Alloc>
    void list<T, Alloc>::remove(const T& value) {
        iterator first = begin();
        iterator last = end();
        while(first != last) {
            iterator next = first;
            ++next;
            if(*first == value)
                erase(first);
            first = next;
        }
    }

    template <class T, class Alloc>
    void list<T, Alloc>::unique() {
        iterator first = begin();
        iterator last = end();
        if(first == last)
            return;
        iterator next = first;
        while(++next != last) {
            if(*first == *next)
                erase(next);
            else
                first = next;
            next = first;
        }
    }

    template <class T, class Alloc>
    void list<T, Alloc>::clear() {
        link_type cur = node->next;
        while(cur != node) {
            link_type tmp = cur;
            cur = cur->next;
            destroy_node(tmp);
        }

        node->prev = node;
        node->next = node;
    }
}

#endif