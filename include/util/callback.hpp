// Copied from https://github.com/0x1F9F1/Swage/blob/master/src/core/callback.h#L53

#pragma once

#include "types.h"

template <typename Func>
class Callback;

template <typename Result, typename... Params>
class Callback<Result(Params...)>
{
public:
    constexpr Callback() noexcept = default;

    constexpr Callback(std::nullptr_t) noexcept
        : Callback()
    {}

    template <typename F>
    inline Callback(const F& func) noexcept
        : data_(&reinterpret_cast<const char&>(func))
        , invoke_([](const void* context, Params... params) -> Result {
        return (*static_cast<const F*>(context))(std::forward<Params>(params)...);
            })
    {}

    constexpr explicit operator bool() const noexcept
    {
        return invoke_ != nullptr;
    }

    template <typename... Args>
    inline Result operator()(Args&&... args) const
    {
        return invoke_(data_, std::forward<Args>(args)...);
    }

private:
    Result(*invoke_)(const void* context, Params... args) {};
    const void* data_{};
};
