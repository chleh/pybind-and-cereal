#pragma once

#include <memory>

#include <pybind11/common.h>

namespace reflect_lib
{

template<typename T>
class smart_ptr final
{
    class item
    {
        using MoveFct = T*(*)(T&&);
        using CopyFct = T*(*)(T const&);

        static MoveFct make_mover() { return make_mover(std::is_move_constructible<T>{}); }
        static MoveFct make_mover(std::true_type) {
            return [](T&& t){ return new T{std::move(t)}; };
        }
        static MoveFct make_mover(std::false_type) {
            return nullptr;
        }

        static CopyFct make_copier() { return make_copier(std::is_copy_constructible<T>{}); }
        static CopyFct make_copier(std::true_type) {
            return [](T const& t){ return new T{t}; };
        }
        static CopyFct make_copier(std::false_type) {
            return nullptr;
        }

    public:
        item() = delete;
        explicit item(T* p) : ptr{p}, data_mover{make_mover()}, data_copier{make_copier()} {
        }

        std::unique_ptr<T> ptr;

        MoveFct data_mover = nullptr;
        CopyFct data_copier = nullptr;
    };

public:
    smart_ptr() = default;

    explicit smart_ptr(T* p) : i{new item{p}} {}

    smart_ptr(smart_ptr<T> const& other) : i{other.i} {}
    smart_ptr(smart_ptr<T>&& other) : i{std::move(other.i)} {}

    smart_ptr<T>& operator=(smart_ptr<T> const& other) {
        i = other.i;
        return *this;
    }
    smart_ptr<T>& operator=(smart_ptr<T>&& other) {
        i = std::move(other.i);
        return *this;
    }

    T* get() const {
        if (!i) return nullptr;
        return i->ptr.get();
    }
    T& operator*() const {
        return *(i->ptr);
    }
    T* operator->() const {
        return get();
    }

    T* release() {
        if (i) return i->ptr.release();
        return nullptr;
    }

    decltype(auto) use_count() const {
        return i.use_count();
    }

    T* new_copied() const {
        if (!*this) return nullptr;
        if (!i->data_copier)
            throw pybind11::type_error{
                "Tried to copy-construct a non-copy-constructible type."};
        return i->data_copier(*i->ptr);
    }
    T* new_moved() const {
        if (!*this) return nullptr;
        if (!i->data_mover)
            throw pybind11::type_error{
                "Tried to move-construct a non-move-constructible type."};
        return i->data_mover(std::move(*i->ptr));
    }

    explicit operator bool() const {
        return static_cast<bool>(i) && static_cast<bool>(i->ptr);
    }

private:
    std::shared_ptr<item> i;
};

}  // namespace reflect_lib
