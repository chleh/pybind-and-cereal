#pragma once

#include "smart_ptr.h"

namespace reflect_lib
{
namespace detail
{
template <typename T>
class UniquePtrReference
{
public:
    static UniquePtrReference<T> new_copied(reflect_lib::smart_ptr<T> const& p)
    {
        return UniquePtrReference<T>{p.new_copied()};
    }
    static UniquePtrReference<T> new_moved(reflect_lib::smart_ptr<T> const& p)
    {
        return UniquePtrReference<T>{p.new_moved()};
    }

    UniquePtrReference(T* p, bool cleanup)
        : p_(std::make_shared<std::pair<std::unique_ptr<T>, bool>>(
              std::unique_ptr<T>(p), cleanup))
    {
    }

    ~UniquePtrReference()
    {
        if (p_.unique() && !p_->second)
            p_->first.release();
    }

    std::unique_ptr<T>& get() { return p_->first; }
    std::unique_ptr<T> const& getConst() const { return p_->first; }
    std::unique_ptr<T>&& getRValue() { return std::move(p_->first); }

private:
    explicit UniquePtrReference(T* p)
        : p_(std::make_shared<std::pair<std::unique_ptr<T>, bool>>(
              std::unique_ptr<T>(p), true))
    {
    }

    std::shared_ptr<std::pair<std::unique_ptr<T>, bool>> p_;
};

template <typename T>
UniquePtrReference<T> copy_to_unique_ptr(reflect_lib::smart_ptr<T> const& p)
{
    return UniquePtrReference<T>::new_copied(p);
}

template <typename T>
UniquePtrReference<T> move_to_unique_ptr(reflect_lib::smart_ptr<T> const& p)
{
    return UniquePtrReference<T>::new_moved(p);
}

template <typename T>
class RValueReference
{
public:
    static RValueReference<T> new_copied(reflect_lib::smart_ptr<T> const& p)
    {
        return RValueReference<T>{p.new_copied()};
    }
    static RValueReference<T> new_moved(reflect_lib::smart_ptr<T> const& p)
    {
        return RValueReference<T>{p.new_moved()};
    }

    T&& get() { return std::move(*p_); }

private:
    RValueReference(T* p) : p_(p) {}
    std::shared_ptr<T> p_;
};

template <typename T>
RValueReference<T> copy_to_rvalue_reference(reflect_lib::smart_ptr<T> const& p)
{
    return RValueReference<T>::new_copied(p);
}

template <typename T>
RValueReference<T> move_to_rvalue_reference(reflect_lib::smart_ptr<T> const& p)
{
    return RValueReference<T>::new_moved(p);
}

}  // namespace detail
}  // namespace reflect_lib
