#pragma once


namespace value_types
{

template<auto V>
struct ValueTag{ static constexpr auto Value = V; };

template<class T, T... ts>
using VTuple = type_tuples::TTuple<ValueTag<ts>...>;

}
