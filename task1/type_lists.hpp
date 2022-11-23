#pragma once

#include <concepts>

#include <type_tuples.hpp>

namespace type_lists
{

    template <class TL>
    concept TypeSequence =
        requires
    {
        typename TL::Head;
        typename TL::Tail;
    };

    struct Nil
    {
    };

    template <class TL>
    concept Empty = std::derived_from<TL, Nil>;

    template <class TL>
    concept TypeList = Empty<TL> || TypeSequence<TL>;

    template <class T, class TL>
    struct Cons
    {
        using Head = T;
        using Tail = TL;
    };

    template <class T>
    struct Repeat
    {
        using Head = T;
        using Tail = Repeat<T>;
    };

    template <TypeList TL>
    struct ToTupleImpl
    {
        using Type = type_tuples::TTuple<>;
    };

    template <TypeSequence TS>
    struct ToTupleImpl<TS>
    {
        using Type =
            typename type_tuples::TTuplePacker<
                typename TS::Head, typename ToTupleImpl<typename TS::Tail>::Type>::Type;
    };

    template <class T>
    struct FromTupleImpl
    {
        using Type = Nil;
    };

    template <class T>
    struct FromTupleImpl<type_tuples::TTuple<T>>
    {
        using Type = Cons<T, Nil>;
    };

    template <class T, class... Ts>
    struct FromTupleImpl<type_tuples::TTuple<T, Ts...>>
    {
        using Type = Cons<T, typename FromTupleImpl<type_tuples::TTuple<Ts...>>::Type>;
    };

    template <class TT>
    using FromTuple = typename FromTupleImpl<TT>::Type;

    template <TypeList TL>
    using ToTuple = typename ToTupleImpl<TL>::Type;

    // template <class T, class Added>
    // using Concatenate = TypeList<T, Added>;

    // template <int N, class T, class... Ts>
    // struct Cut<TypeList<T, Ts...>>() {
    //     using Head = T;
    //     using Tail = TypeList<Ts...>;
    // }

    template <int N, class TL>
    struct Take
    {
        using Head = typename TL::Head;
        using Tail = Take<N - 1, typename TL::Tail>;
    };

    template <int N, Empty TE>
    struct Take<N, TE> : public Nil{};

    template <TypeList TL>
    struct Take<0, TL> : public Nil{};

    template <int N, TypeList TL>
    struct DropImpl 
    {
        using Type = typename DropImpl<N - 1, typename TL::Tail>::Type;
    };

    template <TypeList TL>
    struct DropImpl<0, TL> {
        using Type = TL;
    };

    template <int N, Empty TE>
    struct DropImpl<N, TE> : public Nil{
        using Type = Nil;
    };

    template <int N, TypeList TL>
    using Drop = typename DropImpl<N, TL>::Type;
    // Your fun, fun metaalgorithms :)
} // namespace type_lists
