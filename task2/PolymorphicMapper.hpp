#pragma once

#include <optional>

template <class From, auto target>
struct Mapping
{
  using Target = decltype(target);
};

template <class Base, class Target, class... Mappings>
  requires(std::same_as<
               std::remove_cv_t<typename Mappings::Target>,
               Target> &&
           ...)
struct PolymorphicMapper
{
  static std::optional<Target> map(const Base &)
  {
    return {};
  }
};

template <class Base, class Target, Target tg, class Cast, class... Mappings>
  requires std::is_base_of_v<Base, Cast>
struct PolymorphicMapper<Base, Target, Mapping<Cast, tg>, Mappings...>
{
  static std::optional<Target> map(const Base &object)
  {
    if (dynamic_cast<const Cast *>(std::addressof(object)) != nullptr)
    {
      return {tg};
    }
    else
    {
      return PolymorphicMapper<Base, Target, Mappings...>::map(object);
    }
  }
};