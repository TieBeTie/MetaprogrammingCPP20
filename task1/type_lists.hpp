#pragma once

#include <concepts>

#include <type_tuples.hpp>

namespace type_lists
{

    template <class TL>
    concept TypeSequence =
        requires {
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

    template <int N, class TL>
    struct Take
    {
        using Head = typename TL::Head;
        using Tail = Take<N - 1, typename TL::Tail>;
    };

    template <int N, Empty TE>
    struct Take<N, TE> : Nil
    {
    };

    template <TypeList TL>
    struct Take<0, TL> : Nil
    {
    };

    template <int N, TypeList TL>
    struct DropImpl
    {
        using Type = typename DropImpl<N - 1, typename TL::Tail>::Type;
    };

    template <TypeList TL>
    struct DropImpl<0, TL>
    {
        using Type = TL;
    };

    template <int N, Empty TE>
    struct DropImpl<N, TE> : Nil
    {
        using Type = Nil;
    };

    template <int N, TypeList TL>
    using Drop = typename DropImpl<N, TL>::Type;

    template <int N, class T>
    using Replicate = Take<N, Repeat<T>>;

    template <class T, template <class> class F>
    struct IterateImpl
    {
        using Head = T;
        using Tail = IterateImpl<F<T>, F>;
    };

    template <template <class> class F, class T>
    using Iterate = IterateImpl<T, F>;

    template <TypeList CurrentTS, TypeList FullTS>
    struct CycleImpl
    {
        using Head = typename CurrentTS::Head;
        using Tail = CycleImpl<typename CurrentTS::Tail, FullTS>;
    };

    template <Empty EmptyTL, Empty EmptyFullTL>
    struct CycleImpl<EmptyTL, EmptyFullTL> : Nil
    {
    };

    template <Empty TL, TypeSequence FullTS>
    struct CycleImpl<TL, FullTS>
    {
        using Head = typename FullTS::Head;
        using Tail = typename CycleImpl<FullTS, FullTS>::Tail;
    };

    template <TypeList TL>
    using Cycle = CycleImpl<TL, TL>;

    template <template <class> class F, TypeList TL>
    struct MapImpl
    {
        using Head = F<typename TL::Head>;
        using Tail = MapImpl<F, typename TL::Tail>;
    };

    template <template <class> class F, Empty TL>
    struct MapImpl<F, TL> : Nil
    {
    };

    template <template <class> class F, TypeList TL>
    using Map = MapImpl<F, TL>;

    template <template <class> class P, TypeList TL>
    struct FilterImpl
    {
        using Head = typename TL::Head;
        using Tail = FilterImpl<P, typename TL::Tail>;
    };

    template <template <class> class P, TypeList TL>
        requires(!P<typename TL::Head>::Value)
    struct FilterImpl<P, TL> : FilterImpl<P, typename TL::Tail>
    {
    };

    template <template <class> class P, Empty TL>
    struct FilterImpl<P, TL> : Nil
    {
    };

    template <template <class> class P, TypeList TL>
    using Filter = FilterImpl<P, TL>;

    template <TypeList TL, class... Ts>
    struct InitsImpl
    {
        using Head = FromTuple<type_tuples::TTuple<Ts...>>;
        using Tail = InitsImpl<
            typename TL::Tail,
            Ts...,
            typename TL::Head>;
    };

    template <Empty TL, class... Ts>
    struct InitsImpl<TL, Ts...>
    {
        using Head = FromTuple<type_tuples::TTuple<Ts...>>;
        using Tail = Nil;
    };

    template <TypeList TL>
    using Inits = InitsImpl<TL>;

    template <TypeList TL>
    struct TailsImpl
    {
        using Head = TL;
        using Tail = TailsImpl<typename TL::Tail>;
    };

    template <Empty TL>
    struct TailsImpl<TL>
    {
        using Head = Nil;
        using Tail = Nil;
    };

    template <TypeList TL>
    using Tails = TailsImpl<TL>;

    template <template <class, class> class OP, class T, TypeList TL, bool begin = true>
    struct ScanlImpl
    {
        using Head = T;
        using Tail = ScanlImpl<OP, T, TL, false>;
    };

    template <template <class, class> class OP, class T, TypeList TL>
    struct ScanlImpl<OP, T, TL, false>
    {
        using Head = OP<T, typename TL::Head>;
        using Tail = ScanlImpl<OP, Head, typename TL::Tail, false>;
    };

    template <template <class, class> class OP, class T, Empty TL>
    struct ScanlImpl<OP, T, TL, false> : Nil
    {
    };

    template <template <class, class> class OP, class T, TypeList TL>
    using Scanl = ScanlImpl<OP, T, TL>;

    template <template <class, class> class OP, class T, TypeList TL>
    struct FoldlImpl
    {
        using Type = typename FoldlImpl<
            OP,
            OP<T, typename TL::Head>,
            typename TL::Tail>::Type;
    };

    template <template <class, class> class OP, class T, Empty TL>
    struct FoldlImpl<OP, T, TL>
    {
        using Type = T;
    };

    template <template <class, class> class OP, class T, TypeList TL>
    using Foldl = typename FoldlImpl<OP, T, TL>::Type;

    template <TypeList L, TypeList R>
    struct Zip2Impl : Nil
    {
    };

    template <TypeSequence L, TypeSequence R>
    struct Zip2Impl<L, R>
    {
        using Head = type_tuples::TTuple<typename L::Head, typename R::Head>;
        using Tail = Zip2Impl<typename L::Tail, typename R::Tail>;
    };

    template <TypeList L, TypeList R>
    using Zip2 = Zip2Impl<L, R>;

    template <TypeList... TLs>
    struct ZipImpl
    {
        using Head = type_tuples::TTuple<typename TLs::Head...>;
        using Tail = ZipImpl<typename TLs::Tail...>;
    };

    template <TypeList... TLs>
    using Zip = ZipImpl<TLs...>;
} // namespace type_lists
