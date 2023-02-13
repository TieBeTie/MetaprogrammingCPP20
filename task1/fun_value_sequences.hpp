#pragma once

#include <value_types.hpp>

using namespace type_tuples;
using namespace type_lists;
using namespace value_types;

template<class T>
concept HasValue = requires { T::Value; };

template <HasValue V>
using Inc = ValueTag<V::Value + 1>;

using Nats = Iterate<Inc, ValueTag<0>>;

template <HasValue L, HasValue R>
using Plus = ValueTag<L::Value + R::Value>;

template <HasValue Prev, HasValue PrevPrev>
struct FibImpl {
    using Head = ValueTag<PrevPrev::Value + Prev::Value>;
    using Tail = FibImpl<Head, Prev>; 
};

template <TypeList L, TypeList R>
struct Concatenate {
    using Head = typename L::Head;
    using Tail = Concatenate<typename L::Tail, R>;
};

template <Empty L, TypeList R>
struct Concatenate<L, R> {
    using Head = typename R::Head;
    using Tail = typename R::Tail;
};

using Fib = Concatenate<FromTuple<VTuple<int, 0, 1>>, FibImpl<ValueTag<1>, ValueTag<0>>>;

constexpr bool IsPrime(int x, int d = 2) {
  if (x <= 1) {
    return false;
  }
  if (d * d > x) {
    return true;
  }
  if (x % d == 0) {
    return false;
  }
  return IsPrime(x, d + 1);
}

template<HasValue T> 
struct IsSimple {
  static constexpr bool Value = IsPrime(T::Value);
};

using Primes = Filter<IsSimple, Nats>;
