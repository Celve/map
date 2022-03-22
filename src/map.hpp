#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    struct my_true_type {};
    struct my_false_type {};

    template<class T>
    struct my_type_traits {
        using iterator_assignable = typename T::iterator_assignable;
    };

    template<
            class Key,
            class Value,
            class Compare = std::less<Key>
    > class map {
    public:
        typedef pair<const Key, Value> value_type;

    private:
        Compare comp;
        enum Color {RED, BLACK};

        class Node {
        public:
            value_type *package;
            Color color;
            Node *child[2];
            Node *fa;

            Node() {
                package = nullptr;
                fa = child[0] = child[1] = nullptr;
            }

            Node(Key key, Value value, Color color = RED):color(color) {
                package = new value_type(key, value);
                fa = child[0] = child[1] = nullptr;
            }

            Node(const Node &obj) {
                package = new value_type(*obj.package);
                color = obj.color;
                fa = child[0] = child[1] = nullptr;
            }

            bool ChildNumber(const Node * const &obj) const {
                return obj == child[1];
            }

            ~Node() {
                if (package)
                    delete package;
            }
        };

        Node *root, *verge;
        int n;

        bool Equal(const Key &a, const Key &b) const {
            return !comp(a, b) && !comp(b, a);
        }

        void Construct(Node *&x, Node *y) {
            if (!y)
                return;
            x = new Node(*y);
            ++n;
            Construct(x->child[0], y->child[0]);
            Construct(x->child[1], y->child[1]);
            if (x->child[0])
                x->child[0]->fa = x;
            if (x->child[1])
                x->child[1]->fa = x;
        }

        void Destruct(Node *&x) {
            if (!x)
                return;
            Destruct(x->child[0]);
            Destruct(x->child[1]);
            delete x;
            x = nullptr;
        }

        Node *Insert(const Key &key, bool &flag) {
            Node *x = root, *y = nullptr;
            while (true) {
                if (!x) {
                    x = new Node(key, Value());
                    ++n;
                    x->fa = y;
                    y->child[comp(key, y->package->first)] = x;
                    flag = false;
                    return x;
                }
                if (Equal(key, x->package->first)) {
                    flag = true;
                    return x;
                }
                y = x;
                x = x->child[comp(key, x->package->first)];
            }
        }

        bool CheckColor(Node *x, Color color) {
            if (!x)
                return color == BLACK;
            return x->color == color;
        }

        void SetColor(Node *x, Color color) {
            if (!x)
                return;
            x->color = color;
        }

        void Debug(Node *x) {
            if (!x)
                return;
            int a = !x->child[0] ? -1 : x->child[0]->package->first;
            int b = !x->child[1] ? -1 : x->child[1]->package->first;
            printf("%d(%d): %d %d\n", x->package->first, x->color, a, b);
            Debug(x->child[0]);
            Debug(x->child[1]);
        }

        void Rotate(Node *x) {
            Node *y = x->fa, *w = y->fa;
            bool c = y->ChildNumber(x);
            Node *z = x->child[!c];
            x->child[!c] = y;
            y->child[c] = z;
            x->fa = w;
            y->fa = x;
            if (z)
                z->fa = y;
            if (w)
                w->child[w->ChildNumber(y)] = x;
            if (!x->fa)
                root = x;
        }

        Node *Insert(const Key &key) {
            if (!root) {
                root = new Node(key, Value(), BLACK);
                ++n;
                return root;
            }
            bool flag;
            Node *x = Insert(key, flag), *res = x;
            if (flag)
                return res;
            while (true) {
                Node *y = x->fa; // y can't be nullptr
                if (y->color == BLACK)
                    break;
                Node *z = y->fa; // z can't be nullptr
                bool c = z->ChildNumber(y);
                if (CheckColor(z->child[!c], RED)) { // situation 1
                    z->color = RED;
                    y->color = BLACK;
                    SetColor(z->child[!c], BLACK);
                    if (z == root) {
                        z->color = BLACK;
                        break;
                    }
                    x = z;
                }
                else if (z->ChildNumber(y) == y->ChildNumber(x)){ // situation 2
                    Rotate(y);
                    y->color = BLACK;
                    SetColor(y->child[!c], RED);
                    break;
                }
                else {
                    Rotate(x);
                    Rotate(x);
                    x->color = BLACK;
                    z->color = RED;
                    break;
                }
            }
            return res;
        }

        Node *Find(const Key &key) const {
            Node *x = root;
            while (true) {
                if (!x)
                    return verge;
                if (Equal(key, x->package->first))
                    return x;
                x = x->child[comp(key, x->package->first)];
            }
        }

        Node *Minimum(Node *x) {
            while (x->child[0])
                x = x->child[0];
            return x;
        }

        void Transplant(Node *x, Node *y) {
            if (!x->fa)
                root = y;
            else {
                Node *z = x->fa;
                z->child[z->ChildNumber(x)] = y;
                y->fa = z;
            }
        }

        void DeleteFixUp(Node *x) {
            Node *y = x->fa, *z = y->child[!y->ChildNumber(x)];
            int c = y->ChildNumber(x);
            while (true) {
                if (x->color == RED) {
                    x->color = BLACK;
                    break;
                }
                if (x == root)
                    break;
                if (CheckColor(z, RED)) {
                    z->color = BLACK;
                    y->color = RED;
                    Rotate(z);
                    y = x->fa, c = y->ChildNumber(x), z = y->child[!c];
                }
                if (CheckColor(z->child[0], BLACK) && CheckColor(z->child[1], BLACK)) {
                    z->color = RED;
                    x = y, y = x->fa;
                    if (y)
                        c = y->ChildNumber(x), z = y->child[!c];
                    continue;
                }
                if (CheckColor(z->child[c], RED) && CheckColor(z->child[!c], BLACK)) {
                    z->child[c]->color = BLACK;
                    z->color = RED;
                    Rotate(z->child[c]);
                    y = x->fa, c = y->ChildNumber(x), z = y->child[!c];
                }
                if (CheckColor(z->child[!c], RED)) {
                    z->color = y->color;
                    y->color = BLACK;
                    z->child[!c]->color = BLACK;
                    Rotate(z);
                    break;
                }
            }
        }

        void SetFa(Node *x, Node *y) {
            if (x)
                x->fa = y;
        }

        void ReplaceChild(Node *x, Node *y, Node *z) {
            if (x)
                x->child[x->ChildNumber(y)] = z;
        }

        void Delete(Node *x) {
            Node *y, *t;
            Color removed;
            bool flag = false; // delay removing it from tree
            --n;
            if (x == root && !x->child[0] && !x->child[1]) {
                root = nullptr;
                delete x;
                return;
            }
            if (!x->child[0] && !x->child[1]) {
                removed = x->color;
                t = y = x;
                flag = true;
            }
            else if (!x->child[0] || !x->child[1]) {
                removed = x->color;
                y = x->child[(x->child[1] != nullptr)];
                Transplant(t = x, y);
            }
            else {
                Node *z = Minimum(x->child[1]);
                std::swap(x->color, z->color);
                SetFa(x->child[0], z);
                SetFa(z->child[1], x);
                if (z->fa == x) {
                    z->fa = x->fa;
                    x->fa = z;
                    std::swap(x->child, z->child);
                    z->child[1] = x;
                    ReplaceChild(z->fa, x, z);
                }
                else {
                    SetFa(x->child[1], z);
                    ReplaceChild(x->fa, x, z);
                    ReplaceChild(z->fa, z, x);
                    std::swap(x->child, z->child);
                    std::swap(x->fa, z->fa);
                }
                if (!z->fa)
                    root = z;
                removed = x->color;
                if (!x->child[1]) {
                    t = y = x;
                    flag = true;
                }
                else {
                    y = x->child[1];
                    Transplant(t = x, y);
                }
            }
            if (removed == BLACK && y)
                DeleteFixUp(y);
            y = t->fa;
            if (flag && y)
                ReplaceChild(y, t, nullptr);
            delete t;
        }

    public:
        class const_iterator;
        class iterator {
        private:
            Node *ptr;
            const map *source;

            friend map;

            void Move(int c) {
                if (ptr->child[!c]) {
                    ptr = ptr->child[!c];
                    while (ptr->child[c])
                        ptr = ptr->child[c];
                }
                else if (ptr->fa) {
                    Node *las = ptr;
                    ptr = ptr->fa;
                    while (ptr && ptr->ChildNumber(las) == (!c)) {
                        las = ptr;
                        ptr = ptr->fa;
                    }
                    if (!ptr)
                        ptr = source->verge;
                }
                else
                    ptr = source->verge;
            }

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Value;
            using pointer = Value*;
            using reference = Value&;
            using iterator_category = std::output_iterator_tag;
            using iterator_assignable = my_true_type;

            iterator() {
                ptr = nullptr;
                source = nullptr;
            }

            iterator(Node *ptr, const map *source):ptr(ptr), source(source) {}

            iterator(const iterator &other):ptr(other.ptr), source(other.source) {}

            iterator operator++(int) {
                iterator res = *this;
                operator++();
                return res;
            }

            iterator & operator++() {
                if (ptr == source->verge)
                    throw invalid_iterator();
                Move(1);
                return *this;
            }

            iterator operator--(int) {
                iterator res = *this;
                operator--();
                return res;
            }

            iterator & operator--() {
                if (ptr == source->verge)
                    ptr = source->End();
                else
                    Move(0);
                if (ptr == source->verge)
                    throw invalid_iterator();
                return *this;
            }

            map::value_type & operator*() const {
                return *ptr->package;
            }

            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }
            bool operator!=(const const_iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            map::value_type* operator->() const noexcept {
                return ptr->package;
            }
        };
        class const_iterator {
        private:
            const Node *ptr;
            const map *source;

            friend map;

            void Move(int c) {
                if (ptr->child[!c]) {
                    ptr = ptr->child[!c];
                    while (ptr->child[c])
                        ptr = ptr->child[c];
                }
                else if (ptr->fa) {
                    const Node *las = ptr;
                    ptr = ptr->fa;
                    while (ptr && ptr->ChildNumber(las) == (!c)) {
                        las = ptr;
                        ptr = ptr->fa;
                    }
                    if (!ptr)
                        ptr = source->verge;
                }
                else
                    ptr = source->verge;
            }

        public:
            using difference_type = std::ptrdiff_t;
            using value_type = Value;
            using pointer = Value*;
            using reference = Value&;
            using iterator_category = std::output_iterator_tag;
            using iterator_assignable = my_false_type;
            const_iterator() {
                ptr = nullptr;
                source = nullptr;
            }

            const_iterator(Node *ptr, const map *source):ptr(ptr), source(source) {}

            const_iterator(const const_iterator &other):ptr(other.ptr), source(other.source) {}

            const_iterator(const iterator &other):ptr(other.ptr), source(other.source) {}

            const_iterator operator++(int) {
                const_iterator res = *this;
                operator++();
                return res;
            }

            const_iterator & operator++() {
                if (ptr == source->verge)
                    throw invalid_iterator();
                Move(1);
                return *this;
            }

            const_iterator operator--(int) {
                const_iterator res = *this;
                operator--();
                return res;
            }

            const_iterator & operator--() {
                if (ptr == source->verge)
                    ptr = source->End();
                else
                    Move(0);
                if (ptr == source->verge)
                    throw invalid_iterator();
                return *this;
            }

            const map::value_type & operator*() const {
                return *ptr->package;
            }

            bool operator==(const iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator==(const const_iterator &rhs) const {
                return ptr == rhs.ptr;
            }

            bool operator!=(const iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            bool operator!=(const const_iterator &rhs) const {
                return ptr != rhs.ptr;
            }

            const map::value_type* operator->() const noexcept {
                return ptr->package;
            }
        };

    private:
        void CheckIterator(iterator it) {
            if (it.source != this)
                throw invalid_iterator();
        }

        void CheckIterator(const_iterator it) {
            if (it.source != this)
                throw invalid_iterator();
        }

    public:

        map() {
            root = nullptr;
            verge = new Node;
            n = 0;
        }
        map(const map &other) {
            root = nullptr;
            n = 0;
            verge = new Node;
            Construct(root, other.root);
        }

        map & operator=(const map &other) {
            if (this == &other)
                return *this;
            n = 0;
            Destruct(root);
            Construct(root, other.root);
            return *this;
        }

        ~map() {
            Destruct(root);
            delete verge;
        }

        Value & at(const Key &key) {
            Node *x = Find(key);
            if (x == verge)
                throw index_out_of_bound();
            return x->package->second;
        }

        const Value & at(const Key &key) const {
            Node *x = Find(key);
            if (x == verge)
                throw index_out_of_bound();
            return x->package->second;
        }

        Value & operator[](const Key &key) {
            Node *x = Insert(key);
            return x->package->second;
        }

        const Value & operator[](const Key &key) const {
            return at(key);
        }

        iterator begin() {
            Node *x = root;
            if (!x)
                return iterator(verge, this);
            while (x->child[1])
                x = x->child[1];
            return iterator(x, this);
        }

        const_iterator cbegin() const {
            Node *x = root;
            if (!x)
                return iterator(verge, this);
            while (x->child[1])
                x = x->child[1];
            return const_iterator(x, this);
        }

        iterator end() {
            return iterator(verge, this);
        }

        const_iterator cend() const {
            return iterator(verge, this);
        }

        Node *End() const {
            Node *x = root;
            if (!x)
                return verge;
            while (x->child[0])
                x = x->child[0];
            return x;
        }

        bool empty() const {
            return !n;
        }

        size_t size() const {
            return n;
        }

        void clear() {
            n = 0;
            Destruct(root);
        }

        pair<iterator, bool> insert(const value_type &value) {
            Node *y = Find(value.first);
            if (y == verge) {
                Node *x = Insert(value.first);
                x->package->second = value.second;
                return pair<iterator, bool>(iterator(x, this), true);
            }
            return pair<iterator, bool>(iterator(y, this), false);
        }

        void erase(iterator pos) {
            CheckIterator(pos);
            if (pos.ptr == verge)
                throw invalid_iterator();
            Delete(pos.ptr);
        }

        size_t count(const Key &key) const {
            return Find(key) != verge;
        }

        iterator find(const Key &key) {
            return iterator(Find(key), this);
        }
        const_iterator find(const Key &key) const {
            return const_iterator(Find(key), this);
        }

        void Debug() {
            Debug(root);
        }
    };


}

#endif