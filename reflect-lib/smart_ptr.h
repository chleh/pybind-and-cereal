#pragma once

#include <memory>
#include <cassert>

#include <iostream>
#include <typeinfo>

// TODO use something like std::shared_ptr<Item<T>> instead
// Why not unique_ptr? --> Because it has to be copyable.
template<typename T>
class smart_ptr final
{
    struct item
    {
        using MoveFct = T*(*)(T&&);
        using CopyFct = T*(*)(T const&);

        static MoveFct make_mover() { return make_mover(std::is_move_constructible<T>{}); }
        static MoveFct make_mover(std::true_type) {
            // std::cout << "make_mover " << typeid(T).name() << " moveable\n";
            return [](T&& t){ return new T{std::move(t)}; };
        }
        static MoveFct make_mover(std::false_type) {
            // std::cout << "make_mover " << typeid(T).name() << " not moveable\n";
            return nullptr;
        }

        static CopyFct make_copier() { return make_copier(std::is_copy_constructible<T>{}); }
        static CopyFct make_copier(std::true_type) {
            // std::cout << "make_copier " << typeid(T).name() << " copyable\n";
            return [](T const& t){ return new T{t}; };
        }
        static CopyFct make_copier(std::false_type) {
            // std::cout << "make_copier " << typeid(T).name() << " not copyable\n";
            return nullptr;
        }

        item() = default;
        explicit item(T* p) : ptr{p}, data_mover{make_mover()}, data_copier{make_copier()} {
            // std::cout << "new smartptr item " << typeid(T).name() << " " << p << '\n';
        }

        std::size_t count = 1;
        std::unique_ptr<T> ptr;

        MoveFct data_mover = nullptr;
        CopyFct data_copier = nullptr;
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

    T* new_copied() const {
        // TODO throw
        assert(i->ptr);
        assert(i->data_copier);
        return i->data_copier(*i->ptr);
    }
    T* new_moved() const {
        // TODO throw
        assert(i->ptr);
        assert(i->data_mover);
        return i->data_mover(std::move(*i->ptr));
    }

    explicit operator bool() const { return static_cast<bool>(i->ptr); }

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
