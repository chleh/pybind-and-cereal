#pragma once

#include <memory>
#include <cassert>

#include <iostream>
#include <typeinfo>

template<typename T>
class smart_ptr final
{
    struct item
    {
        item() = default;
        explicit item(T* p) : ptr{p} {
            std::cout << "new smartptr item " << typeid(T).name() << " " << p << '\n';
        }

        std::size_t count = 1;
        std::unique_ptr<T> ptr;

        /*
        ~item() {
            // std::cout << "smart_ptr item dtor " << typeid(T).name()
            //     << ' ' << ptr.get() << '\n';

            T* p = ptr.release();
            if (p) {
                std::cout << "smart_ptr item being overwritten " << typeid(T).name()
                    << ' ' << p << '\n';
                p->~T();
                std::fill(reinterpret_cast<char*>(p), reinterpret_cast<char*>(p)+sizeof(T), '\0');
                delete reinterpret_cast<char*>(p);
                std::cout << "smart_ptr item overwritten " << typeid(T).name()
                    << ' ' << p << '\n';
            }
        }
        */
    };

public:
    smart_ptr() : i{new item} {}

    explicit smart_ptr(T* p) : i{new item{p}} {}

private:
    item& add_owner(item& i_) { i_.count++; return i_; }
    smart_ptr(item& i_) : i{&add_owner(i_)} {}

public:
    smart_ptr(smart_ptr<T> const& other) : smart_ptr{*other.i} {}
    smart_ptr(smart_ptr<T>&& other) { std::swap(i, other.i); }

    smart_ptr<T>& operator=(smart_ptr<T> const& other) {
        cleanup();
        add_owner(*other.i);
        i = other.i;
        return *this;
    }
    smart_ptr<T>& operator=(smart_ptr<T>&& other) {
        std::swap(i, other.i);
        other.cleanup();
        return *this;
    }

    T* get() const { return i->ptr.get(); }
    T& operator*() const { return *(i->ptr); }

    explicit operator bool() const { return static_cast<bool>(i->ptr); }

    T* steal() {
        assert(i);
        T* p = i->ptr.release();
        cleanup();
        return p;
    }

    ~smart_ptr() {
        cleanup();
        // std::cout << "smart_ptr dtor " << typeid(T).name() << '\n';
    }

private:
    void cleanup() {
        assert(i);
        if (!--(i->count)) {
            delete i;
        }
        i = new item();
    }

    item* i = nullptr;
};
