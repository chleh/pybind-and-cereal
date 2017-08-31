#pragma once

#include <pybind11/stl_bind.h>
#include <iostream>

#include "aux-types.h"
#include "remangle.h"
#include "smart_ptr.h"

namespace reflect_lib
{
namespace detail
{
template <typename T, typename Holder>
struct BindVector {
    static pybind11::class_<T, Holder> bind(pybind11::module& m,
                                            std::string const& name)
    {
        return pybind11::bind_vector<T, Holder>(m, name)
            .def("__getstate__",
                 [](T const& v) {
                     std::cout << "  copying list "
                               << demangle(typeid(T).name()) << std::endl;
                     auto l = pybind11::list(v.size());
                     for (std::size_t i = 0; i < v.size(); ++i) {
                         std::cout << "  copying list " << i << std::endl;
                         // TODO try to save copies: use &v[i] or v[i].get()
                         l[i] = v[i];
                     }
                     return l;
                 })
            .def("__setstate__", [](T& v, pybind11::list& l) {
                new (&v) T();
                v.reserve(pybind11::len(l));
                for (auto& e : l)
                    v.emplace_back(std::move(e).cast<typename T::value_type>());
            });
    }
};

// TODO make vector bindings complete.
template <typename T, typename Holder>
struct BindVector<std::vector<std::shared_ptr<T>>, Holder> {
private:
    using Vector = std::vector<std::shared_ptr<T>>;

public:
    static pybind11::class_<Vector, Holder> bind(pybind11::module& m,
                                                 std::string const& name)
    {
        // return pybind11::bind_vector<T_, Holder>(m, name)
        auto cl = pybind11::class_<Vector, Holder>(m, name.c_str());
        cl.def(pybind11::init<>());
        cl.def(pybind11::init<Vector const&>());
        cl.def("__init__", [](Vector& v, pybind11::iterable it) {
            new (&v) Vector();
            try {
                v.reserve(len(it));
                for (pybind11::handle h : it)
                    v.emplace_back(h.cast<smart_ptr<T>>().new_copied());
            } catch (...) {
                v.~Vector();
                throw;
            }
        });
        cl.def("__setitem__",
               [](Vector& v, std::size_t i, const std::shared_ptr<T>& t) {
                   if (i >= v.size())
                       throw pybind11::index_error();
                   v[i] = t;
               });

        cl.def("__getitem__",
               [](Vector& v, std::size_t i) -> T& {
                   if (i >= v.size())
                       throw pybind11::index_error();
                   return *v[i];
               },
               pybind11::return_value_policy::reference_internal  // ref +
                                                                  // keepalive
               );

        using ItType = typename Vector::iterator;

        class It
        {
        public:
            explicit It(ItType it) : it_(it) {}
            It& operator++()
            {
                ++it_;
                return *this;
            }
            T* operator*() { return it_->get(); }
            bool operator==(It const& other) const { return it_ == other.it_; }

        private:
            ItType it_;
        };

        cl.def("__iter__",
               [](Vector& v) {
                   return pybind11::make_iterator<
                       pybind11::return_value_policy::reference_internal,
                       It,
                       It,
                       T*>(It(v.begin()), It(v.end()));
               },
               pybind11::keep_alive<0, 1>() /* Essential: keep list alive while
                                     iterator
                                     exists */
               );

        cl.def("__len__", &Vector::size);

        cl.def("__getstate__", [](Vector const& v) {
              std::cout << "  copying list " << demangle(typeid(Vector).name())
                        << std::endl;
              auto l = pybind11::list(v.size());
              for (std::size_t i = 0; i < v.size(); ++i) {
                  std::cout << "  copying list " << i << std::endl;
                  // TODO try to save copies: use &v[i] or v[i].get()
                  l[i] = v[i].get();
              }
              return l;
          }).def("__setstate__", [](Vector& v, pybind11::list& l) {
            new (&v) Vector();
            v.reserve(pybind11::len(l));
            std::cout << "  setting state " << demangle(typeid(Vector).name())
                      << std::endl;
            for (auto& e : l) {
                // v.emplace_back(std::move(e).cast<typename
                // Vector::value_type>());
                v.emplace_back(std::move(e).cast<smart_ptr<T>>().new_moved());
            }
        });

        return cl;
    }
};

// TODO make vector bindings complete.
template <typename T, typename Holder>
struct BindVector<std::vector<std::unique_ptr<T>>, Holder> {
private:
    using Vector = std::vector<std::unique_ptr<T>>;

public:
    static pybind11::class_<Vector, Holder> bind(pybind11::module& m,
                                                 std::string const& name)
    {
        // return pybind11::bind_vector<T_, Holder>(m, name)
        auto cl = pybind11::class_<Vector, Holder>(m, name.c_str());
        cl.def(pybind11::init<>());
        cl.def(pybind11::init<Vector const&>());
        cl.def("__init__", [](Vector& v, pybind11::iterable it) {
            new (&v) Vector();
            try {
                v.reserve(len(it));
                for (pybind11::handle h : it)
                    v.emplace_back(h.cast<smart_ptr<T>>().new_copied());
            } catch (...) {
                v.~Vector();
                throw;
            }
        });
        cl.def("__setitem__",
               [](Vector& v, std::size_t i, UniquePtrReference<T>& t) {
                   if (i >= v.size())
                       throw pybind11::index_error();
                   v[i] = t.getRValue();
               });

        cl.def("__getitem__",
               [](Vector& v, std::size_t i) -> T& {
                   if (i >= v.size())
                       throw pybind11::index_error();
                   return *v[i];
               },
               pybind11::return_value_policy::reference_internal  // ref +
                                                                  // keepalive
               );

        using ItType = typename Vector::iterator;

        class It
        {
        public:
            explicit It(ItType it) : it_(it) {}
            It& operator++()
            {
                ++it_;
                return *this;
            }
            T* operator*() { return it_->get(); }
            bool operator==(It const& other) const { return it_ == other.it_; }

        private:
            ItType it_;
        };

        cl.def("__iter__",
               [](Vector& v) {
                   return pybind11::make_iterator<
                       pybind11::return_value_policy::reference_internal,
                       It,
                       It,
                       T*>(It(v.begin()), It(v.end()));
               },
               pybind11::keep_alive<0, 1>() /* Essential: keep list alive while
                                     iterator
                                     exists */
               );

        cl.def("__len__", &Vector::size);

        cl.def("__getstate__", [](Vector const& v) {
              std::cout << "  copying list " << demangle(typeid(Vector).name())
                        << std::endl;
              auto l = pybind11::list(v.size());
              for (std::size_t i = 0; i < v.size(); ++i) {
                  std::cout << "  copying list up " << i << std::endl;
                  // TODO try to save copies: use &v[i] or v[i].get()
                  l[i] = v[i].get();
              }
              return l;
          }).def("__setstate__", [](Vector& v, pybind11::list& l) {
            new (&v) Vector();
            v.reserve(pybind11::len(l));
            std::cout << "  setting state " << demangle(typeid(Vector).name())
                      << std::endl;
            for (auto& e : l) {
                // v.emplace_back(std::move(e).cast<typename
                // Vector::value_type>());
                v.emplace_back(std::move(e).cast<smart_ptr<T>>().new_moved());
            }
        });

        return cl;
    }
};

}  // namespace detail
}  // namespace reflect_lib
