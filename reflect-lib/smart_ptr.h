#pragma once

template<typename T>
class smart_ptr final
{
public:
    template<typename... Args>
    smart_ptr(Args&&... args) : ptr{std::forward<Args>(args)...} {}

    smart_ptr(smart_ptr<T> const&) = delete;
    smart_ptr(smart_ptr<T>&&) = default;

    smart_ptr<T>& operator=(smart_ptr<T> const&) = delete;
    smart_ptr<T>& operator=(smart_ptr<T>&& other) {
        std::swap(ptr, other.ptr);
        delete other.ptr;
        return *this;
    }

    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }

    explicit operator bool() const { return ptr == nullptr; }

    ~smart_ptr() { delete ptr; }

private:
    T* ptr = nullptr;
};
