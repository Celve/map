/**
 * implement a container like std::map
 */
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

template<
	class Key,
	class Value,
	class Compare = std::less<Key>
> class map {
public:
    /**
     * the internal type of data.
     * it should have a default constructor, a copy constructor.
     * You can use sjtu::map as value_type by typedef.
     */
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

    Node *root, *nil;
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

    Node *Insert(Key key, bool &flag) {
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

    Node *Insert(Key key) {
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
                throw index_out_of_bound();
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

    void Delete(Node *x) {
        Node *y, *t;
        Color temp;
        bool flag = false;
        if (x == root && !x->child[0] && !x->child[1]) {
            --n;
            root = nullptr;
            delete x;
            return;
        }
        if (!x->child[0] && !x->child[1]) {
            temp = x->color;
            t = y = x;
            flag = true;
        }
        else if (!x->child[0] || !x->child[1]) {
            temp = x->color;
            y = x->child[(x->child[1] != nullptr)];
            Transplant(t = x, y);
        }
        else {
            Node *z = Minimum(x->child[1]);
            std::swap(x->package, z->package);
            temp = z->color;
            if (!z->child[1]) {
                t = y = z;
                flag = true;
            }
            else {
                y = z->child[1];
                Transplant(t = z, y);
            }
        }
        if (temp == BLACK && y)
            DeleteFixUp(y);
        y = t->fa;
        if (flag && y)
            y->child[y->ChildNumber(t)] = nullptr;
        --n;
        delete t;
    }

public:
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
	private:
		/**
		 *
		 * TODO add data members
		 *   just add whatever you want.
		 */
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
                    ptr = source->nil;
            }
            else
                ptr = source->nil;
        }
        
	public:
		// The following code is written for the C++ type_traits library.
		// Type traits is a C++ feature for describing certain properties of a type.
		// For instance, for an iterator, iterator::value_type is the type that the
		// iterator points to.
		// STL algorithms and containers may use these type_traits (e.g. the following
		// typedef) to work properly.
		// See these websites for more information:
		// https://en.cppreference.com/w/cpp/header/type_traits
		// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
		// About iterator_category: https://en.cppreference.com/w/cpp/iterator
		using difference_type = std::ptrdiff_t;
		using value = Value;
		using pointer = Value*;
		using reference = Value&;
		using iterator_category = std::output_iterator_tag;
		// If you are interested in type_traits, toy_traits_test provides a place to
		// practice. But the method used in that test is old and rarely used, so you
		// may explore on your own.
		// Notice: you may add some code in here and class const_iterator and namespace sjtu to implement toy_traits_test,
		// this part is only for bonus.

        iterator() {
           ptr = nullptr;
           source = nullptr;
        }

		iterator(Node *ptr, const map *source):ptr(ptr), source(source) {}
        
		iterator(const iterator &other):ptr(other.ptr), source(other.source) {}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
            iterator res = *this;
            operator++();
            return res;
        }
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
            if (ptr == source->nil)
                throw invalid_iterator();
            Move(1);
            return *this;
        }
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
            iterator res = *this;
            operator--();
            return res;
        }
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
            if (ptr == source->nil)
                ptr = source->End();
            else
                Move(0);
            if (ptr == source->nil)
                throw invalid_iterator();
            return *this;
        }
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
            return *ptr->package;
        }
		bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
		bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
		bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }

		/**
		 * for the support of it->first.
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
            return ptr->package;
        }
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
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
                    ptr = source->nil;
            }
            else
                ptr = source->nil;
        }
    
        // data members.
    public:
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
        /**
         * TODO ++iter
         */
        const_iterator & operator++() {
            if (ptr == source->nil)
                throw invalid_iterator();
            Move(1);
            return *this;
        }
        /**
         * TODO iter--
         */
        const_iterator operator--(int) {
            const_iterator res = *this;
            operator--();
            return res;
        }
        /**
         * TODO --iter
         */
        const_iterator & operator--() {
            if (ptr == source->nil)
                ptr = source->End();
            else
                Move(0);
            if (ptr == source->nil)
                throw invalid_iterator();
            return *this;
        }
        /**
         * a operator to check whether two iterators are same (pointing to the same memory).
         */
        const value_type & operator*() const {
            return *ptr->package;
        }
        bool operator==(const iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        bool operator==(const const_iterator &rhs) const {
            return ptr == rhs.ptr;
        }
        /**
         * some other operator for iterator.
         */
        bool operator!=(const iterator &rhs) const {
            return ptr != rhs.ptr;
        }
        bool operator!=(const const_iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        /**
         * for the support of it->first.
         * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
         */
        const value_type* operator->() const noexcept {
            return ptr->package;
        }
	};
	/**
	 * TODO two constructors
	 */
	map() {
        root = nullptr;
        nil = new Node;
        n = 0;
    }
	map(const map &other) {
        n = 0;
        nil = new Node;
        Construct(root, other.root);
    }
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
        if (this == &other)
            return *this;
        n = 0;
        Destruct(root);
        Construct(root, other.root);
        return *this;
    }
	/**
	 * TODO Destructors
	 */
	~map() {
        Destruct(root);
        delete nil;
    }
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	Value & at(const Key &key) {
        Node *x = Find(key);
        return x->package->second;
    }
	const Value & at(const Key &key) const {
        Node *x = Find(key);
        return x->package->second;
    }
	/**
	 * TODO
	 * access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	Value & operator[](const Key &key) {
        Node *x = Insert(key);
        return x->package->second;
    }
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const Value & operator[](const Key &key) const {
        return at(key);
    }
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
        Node *x = root;
        if (!x)
            return iterator(nil, this);
        while (x->child[1])
            x = x->child[1];
        return iterator(x, this);
    }
    
	const_iterator cbegin() const {
        Node *x = root;
        if (!x)
            return iterator(nil, this);
        while (x->child[1])
            x = x->child[1];
        return const_iterator(x, this);
    }
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
        return iterator(nil, this);
    }

	const_iterator cend() const {
        return iterator(nil, this);
    }

    Node *End() const {
        Node *x = root;
        if (!x)
            return nil;
        while (x->child[0])
            x = x->child[0];
        return x;
    }
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
        return !n;
    }
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
        return n;
    }
	/**
	 * clears the contents
	 */
	void clear() {
        n = 0;
        Destruct(root);
    }
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
        Node *y;
        try {
            y = Find(value.first);
        }
        catch (index_out_of_bound) {
            Node *x = Insert(value.first);
            x->package->second = value.second;
            return pair<iterator, bool>(iterator(x, this), true);
        }
        return pair<iterator, bool>(iterator(y, this), false);
    }
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
        Delete(pos.ptr);
    }
	/**
	 * Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
        try {
            Node *x = Find(key);
        }
        catch (index_out_of_bound) {
            return 0;
        }
        return 1;
    }
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
        Node *x = nil;
        try {
            x = Find(key);
        }
        catch (index_out_of_bound) {}
        return iterator(x, this);
    }
	const_iterator find(const Key &key) const {
        Node *x = nil;
        try {
            x = Find(key);
        }
        catch (index_out_of_bound) {}
        return const_iterator(x, this);
    }

    void Debug() {
        Debug(root);
    }
};

}

#endif
