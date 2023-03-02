#include <array>
#include <concepts>
#include <cstdlib>
#include <iterator>
#include <span>
#include <vector>

inline constexpr std::ptrdiff_t dynamic_stride = -1;

template <ptrdiff_t stride>
struct StrideImpl
{
  StrideImpl(std::ptrdiff_t) {}

  constexpr ptrdiff_t GetStride() const
  {
    return stride;
  }
};

template <>
struct StrideImpl<dynamic_stride>
{
  StrideImpl() : stride_(1) {}

  StrideImpl(std::ptrdiff_t run_time_stride) : stride_(run_time_stride) {}

  constexpr ptrdiff_t GetStride() const
  {
    return stride_;
  }

  void SetStride(ptrdiff_t stride)
  {
    stride_ = stride;
  }

  ptrdiff_t stride_;
};

template <size_t new_extent>
struct ExtentImpl
{
  ExtentImpl(size_t)
  {
  }

  ExtentImpl() = default;

  constexpr size_t GetExtent() const
  {
    return new_extent;
  }
};

template <>
struct ExtentImpl<std::dynamic_extent>
{
  ExtentImpl(size_t run_time_extent) : extent_(run_time_extent)
  {
  }

  constexpr size_t GetExtent() const
  {
    return extent_;
  }

  size_t extent_;
};

namespace MetaFunc
{
  constexpr size_t GetDifference(size_t base, size_t subtrahend)
  {
    return base == std::dynamic_extent ? base : base - subtrahend;
  }

  constexpr std::ptrdiff_t GetStride(std::ptrdiff_t base, size_t multiplier)
  {
    return base == dynamic_stride ? base : multiplier * base;
  }

  constexpr std::size_t GetExtent(std::size_t base, std::ptrdiff_t delimiter)
  {
    if (base == std::dynamic_extent)
    {
      return base;
    }
    return base % delimiter == 0 ? base / delimiter : base / delimiter + 1;
  }
};

template <class T, std::size_t extent = std::dynamic_extent, std::ptrdiff_t stride = 1>
class Slice : public StrideImpl<stride>, public ExtentImpl<extent>
{
public:
  using ExtentImpl<extent>::GetExtent;
  using StrideImpl<stride>::GetStride;

  constexpr size_t Stride()
  {
    return GetStride();
  }

  using value_type = std::remove_cvref_t<T>;
  using element_type = std::remove_reference_t<T>;
  using size_type = std::size_t;
  using pointer = element_type *;
  using reference = element_type &;
  using const_pointer = const value_type *;
  using const_reference = const value_type &;
  using difference_type = std::ptrdiff_t;

  template <class U, size_type ext, difference_type str>
  friend class Slice;

  class Iterator
  {
  public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::remove_cvref_t<T>;

    template <
        class U,
        size_type ext,
        difference_type str>
    friend class Slice;

    Iterator(const Iterator &other) = default;
    Iterator &operator=(const Iterator &other) = default;
    Iterator() : slice_data_(nullptr), data_(nullptr) {}

    Iterator &operator++()
    {
      data_ += slice_data_->GetStride();
      return *this;
    }

    reference operator*() const
    {
      return *data_;
    }

    Iterator &operator--()
    {
      data_ -= slice_data_->GetStride();
      return *this;
    }

    Iterator operator++(int)
    {
      auto cpy = *this;
      return ++cpy;
    }

    Iterator operator--(int)
    {
      auto cpy = *this;
      return --cpy;
    }

    Iterator &operator-=(difference_type delta)
    {
      return *this += -delta;
    }

    Iterator &operator+=(difference_type delta)
    {
      data_ += delta * slice_data_->GetStride();
      return *this;
    }

    Iterator operator+(difference_type delta) const
    {
      auto cpy = *this;
      cpy += delta;
      return cpy;
    }

    friend Iterator operator+(difference_type delta, const Iterator &other)
    {
      return other + delta;
    }

    Iterator operator-(difference_type delta) const
    {
      return *this + -delta;
    }

    ptrdiff_t operator-(const Iterator &other) const
    {
      return data_ - other.data_;
    }

    bool operator==(Iterator other) const
    {
      if (data_ >= slice_data_->end().data_ && other.data_ >= other.slice_data_->end().data_)
      {
        return true;
      }
      return data_ == other.data_;
    }

    std::weak_ordering operator<=>(Iterator other) const
    {
      if (data_ == slice_data_->end().data_ && other.data_ == other.slice_data_->end().data_)
      {
        return std::weak_ordering::equivalent;
      }
      if (data_ < other.data_)
      {
        return std::weak_ordering::less;
      }
      if (data_ > other.data_)
      {
        return std::weak_ordering::greater;
      }
      return std::weak_ordering::equivalent;
    }

    reference operator[](size_t index) const
    {
      return data_[index];
    }

    pointer operator->() const
    {
      return data_;
    }

  protected:
    Iterator(const Slice<T, extent, stride> *slice_ptr, pointer ptr)
        : slice_data_(slice_ptr), data_(ptr)
    {
    }

    const Slice<T, extent, stride> *slice_data_;
    pointer data_;
  };

  using iterator = Iterator;
  using const_iterator = const Iterator;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Slice() : data_(nullptr)
  {
  }

  template <typename U, size_type ext>
  Slice(std::array<U, ext> &array)
      : StrideImpl<stride>(stride),
        ExtentImpl<extent>(extent),
        data_(std::to_address(array.begin()))
  {
  }

  template <class U, size_type new_extent, difference_type new_stride>
  Slice(const Slice<U, new_extent, new_stride> &slice)
      : StrideImpl<stride>(new_stride),
        ExtentImpl<extent>(new_extent),
        data_(const_cast<element_type *>(slice.data_))
  {
  }

  template <class Container>
    requires std::contiguous_iterator<typename Container::iterator>
  Slice(Container &container) : Slice(container.begin(),
                                      container.size(), 1)
  {
  }

  Slice(pointer start) : data_(start)
  {
  }

  template <std::contiguous_iterator It>
  Slice(It first, size_type count, difference_type skip) : StrideImpl<stride>(skip),
                                                           ExtentImpl<extent>(count),
                                                           data_(std::to_address(first))
  {
  }

  template <class U, size_type ext, difference_type str>
  bool operator==(const Slice<U, ext, str> &other) const
  {
    auto this_it = begin();
    auto other_it = other.begin();
    while (this_it != end() && other_it != other.end())
    {
      ++this_it;
      ++other_it;
    }
    return this_it == end() && other_it == other.end();
  }

  element_type &operator[](size_t index) const
  {
    return *(begin() + index);
  }

  reverse_iterator rend()
  {
    return reverse_iterator(begin());
  }

  reverse_iterator rbegin()
  {
    return reverse_iterator(end());
  }

  iterator begin() const
  {
    return iterator(this, data_);
  }

  iterator end() const
  {
    return iterator(this, data_ + GetExtent());
  }

  constexpr size_t Size()
  {
    return GetExtent();
  }

  pointer Data()
  {
    return data_;
  }

  Slice<T, std::dynamic_extent, stride>
  First(size_type count) const
  {
    return {data_, count * GetStride(), GetStride()};
  }

  template <size_type count>
  Slice<T, count, stride>
  First() const
  {
    return {data_, count * GetStride(), GetStride()};
  }

  Slice<T, std::dynamic_extent, stride>
  Last(size_type count) const
  {
    return {data_ + (GetExtent() - count) * GetStride(), count * GetStride(),
            GetStride()};
  }

  template <size_type count>
  Slice<T, count, stride>
  Last() const
  {
    return {data_ + (GetExtent() - count) * GetStride(), count * GetStride(),
            GetStride()};
  }

  Slice<T, std::dynamic_extent, stride>
  DropFirst(size_type count) const
  {
    return {data_ + count * GetStride(), GetExtent() - count * GetStride(),
            GetStride()};
  }

  template <size_type count>
  Slice<T, MetaFunc::GetDifference(extent, count), stride>
  DropFirst() const
  {
    return {data_ + count * GetStride(), GetExtent() - count * GetStride(),
            GetStride()};
  }

  Slice<T, std::dynamic_extent, stride>
  DropLast(size_type count) const
  {
    return {data_, GetExtent() - count * GetStride(), GetStride()};
  }

  template <size_type count>
  Slice<T, MetaFunc::GetDifference(extent, count), stride>
  DropLast() const
  {
    return {data_, GetExtent() - count * GetStride(), GetStride()};
  }

  Slice<T, std::dynamic_extent, dynamic_stride>
  Skip(difference_type skip) const
  {
    return {data_, GetExtent() % skip == 0 ? GetExtent() / skip : GetExtent() / skip + 1, GetStride() * skip};
  }

  template <difference_type skip>
  Slice<
      T,
      MetaFunc::GetExtent(extent, skip),
      MetaFunc::GetStride(stride, skip)>
  Skip() const
  {
    return {
        data_,
        GetExtent() % skip == 0 ? GetExtent() / skip : GetExtent() / skip + 1,
        GetStride() * skip};
  }

private:
  pointer data_;
};

template <typename U, std::size_t extend>
Slice(std::array<U, extend> &array)
    -> Slice<
        typename std::array<U, extend>::value_type,
        extend,
        1>;

template <class Container>
Slice(Container &)
    -> Slice<
        typename std::iterator_traits<typename Container::iterator>::value_type,
        std::dynamic_extent,
        1>;

template <class It>
  requires std::contiguous_iterator<It>
Slice(It first, std::size_t count, std::ptrdiff_t skip)
    -> Slice<
        typename std::iterator_traits<It>::value_type,
        std::dynamic_extent,
        dynamic_stride>;
