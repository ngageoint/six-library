#pragma once

#pragma once

// Don't want to include all of GSL right now ...

#include <assert.h>

#include <exception>
#include <type_traits>
#include <utility>  

#include <mem/BufferView.h>

namespace gsl
{
    namespace details
    {
        [[noreturn]] inline void terminate() noexcept
        {
            std::terminate();
        }

        template <typename Exception>
        [[noreturn]] void throw_exception(Exception&&) noexcept
        {
            gsl::details::terminate();
        }
    }

    // narrow_cast(): a searchable way to do narrowing casts of values
    template <class T, class U> constexpr T narrow_cast(U&& u) noexcept
    {
        return static_cast<T>(std::forward<U>(u));
    }

    struct narrowing_error : public std::exception { };

    namespace details
    {
        template <class T, class U>
        struct is_same_signedness
            : public std::integral_constant<bool, std::is_signed<T>::value == std::is_signed<U>::value> { };
    } // namespace details

    // narrow() : a checked version of narrow_cast() that throws if the cast changed the value
    namespace details
    {
        template <class T>
        constexpr T narrow_throw_exception(T t) noexcept(false)
        {
            return gsl::details::throw_exception(narrowing_error()), t;
        }
        template <class T, class U>
        constexpr T narrow1_(T t, U u) noexcept(false)
        {
            return (static_cast<U>(t) != u) ? narrow_throw_exception(t) : t;
        }
        template <class T, class U>
        constexpr T narrow2_(T t, U u) noexcept(false)
        {
            return (!details::is_same_signedness<T, U>::value && ((t < T{}) != (u < U{}))) ?
                narrow_throw_exception(t) : t;
        }

        template <class T, class U>
        constexpr T narrow(T t, U u) noexcept(false)
        {
            return narrow1_(t, u) ? narrow2_(t, u) : t;
        }
    }
    template <class T, class U>
    constexpr T narrow(U u) noexcept(false)
    {
        return details::narrow(narrow_cast<T>(u), u);
    }
}

namespace gsl
{
    // super-simple version of span
    template<typename T>
    class span final
    {
        T* p_;
        size_t sz_;

    public:
        span(T* p, size_t sz) : p_(p), sz_(sz) {}

        template<typename TContainer>
        span(TContainer&& c) : span(const_cast<T*>(c.data()), c.size()) {}

        T* data() { return p_; }
        const T* data() const { return p_; }
        size_t size() const { return sz_; }
    };

    template<typename T>
    inline span<T> make_span(T* d, size_t sz)
    {
        return span<T>(d, sz);
    }

    template<typename TContainer>
    inline span<typename TContainer::value_type> make_span(TContainer& c)
    {
        using value_type = typename TContainer::value_type;
        return span<value_type>(c);
    }
}
