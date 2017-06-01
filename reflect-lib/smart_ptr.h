#pragma once

#include <memory>
#include <cassert>

#include <iostream>

template<typename T>
class smart_ptr final
{
    struct item
    {
        item() = default;
        explicit item(T* p) : ptr{p} {}

        std::size_t count = 1;
        std::unique_ptr<T> ptr;
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

    ~smart_ptr() { cleanup(); }

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
