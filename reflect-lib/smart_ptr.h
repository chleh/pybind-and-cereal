#pragma once

template<typename T>
class smart_ptr final
{
public:
    smart_ptr() = default;
    // template<typename... Args>
    // smart_ptr(Args&&... args) : ptr(new T{std::forward<Args>(args)...}) {}

    // template<typename U>
    smart_ptr(T* p) : ptr(p) {}

    smart_ptr(smart_ptr<T> const&) = default;
    smart_ptr(smart_ptr<T>&&) = default;

    smart_ptr<T>& operator=(smart_ptr<T> const&) = default;
    smart_ptr<T>& operator=(smart_ptr<T>&& other) {
        std::swap(ptr, other.ptr);
        delete other.ptr;
        return *this;
    }

    T* get() const { return ptr; }
    T& operator*() const { return *ptr; }

    explicit operator bool() const { return ptr == nullptr; }

    T* release() { T* tmp = ptr; ptr = nullptr; return tmp; }

    ~smart_ptr() { delete ptr; }

private:
    T* ptr = nullptr;
};
