#pragma once


namespace type_tuples
{

template<class... Ts>
struct TTuple 
{
};

// template<class Head, class Tale>
// struct TTuplePacker
// {

// };

// template<class Head, class... Tale>
// struct TTuplePacker<Head, TTuple<Tale...>>
// {
//     using Type = TTuple<Head, Tale...>;
// };

template <typename Head, typename Tail>
struct TTuplePacker {
    using Type = decltype([]<typename... Ts>(TTuple<Ts...>){return TTuple<Head, Ts...>();}(Tail()));
};


template<class TT>
concept TypeTuple = requires(TT t) { []<class... Ts>(TTuple<Ts...>){}(t); };
} // namespace type_tuples
