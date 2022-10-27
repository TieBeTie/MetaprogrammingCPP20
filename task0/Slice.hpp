#include <span>
#include <concepts>
#include <cstdlib>
#include <array>
#include <iterator>
#include <vector>


inline constexpr std::ptrdiff_t dynamic_stride = -1;


template
  < class T
  , std::size_t extent = std::dynamic_extent
  , std::ptrdiff_t stride = 1
  >
class Slice {
public:

  using iterator = T*;
  using const_iterator = const T*;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  
  template<class U>
  Slice(U& container) {
    extent_ = container.size();
    data_ = new T[extent_];
    for (size_t i = 0; i < extent_; ++i) {
        data_[i] = container[i];
    }
  }

  template <std::contiguous_iterator It>
  Slice(It first, std::size_t count, std::ptrdiff_t skip) {
    extent_ = count;
    data_ = new T[extent_];
    for (int i = 0; i < extent_; i += skip) {
        data_[i] = *(first + i);
    }
  }

  // Data, Size, Stride, begin, end, casts, etc...
  constexpr T& operator[](size_t idx) const {
    return *(Data() + idx);
  }

  constexpr size_t Size() const {
    return extent_;
  }

  constexpr size_t Stride() const {
    return stride;
  }

  constexpr T* Data() const noexcept {
    return data_;
  }

  constexpr iterator begin() const noexcept {
    return Data();
  }

  constexpr iterator end() const noexcept {
    return {Data() + Size()};
  }

  Slice<T, std::dynamic_extent, stride>
    First(std::size_t count) const {
        std::vector<T> values(count);
        for (size_t i = 0; i < count; ++i) {
            values[i] = data_[stride * i];
        }
        return Slice(values);
    }

  template <std::size_t count>
  Slice<T, count, stride>
    First() const {
        std::vector<T> values(count);
        for (size_t i = 0; i < count; ++i) {
            values[i] = data_[stride * i];
        }
        // return Slice<std::contiguous_iterator<T>, count, stride>(&values[0]);
        return Slice<T, count, stride>(values);
    }

  Slice<T, std::dynamic_extent, stride>
    Last(std::size_t count) const {
        std::vector<T> values(count);
        for (size_t i = 0; i < count; ++i){
            values[i] = data_[extent_ - 1 - stride * i];
        }
        return Slice(values);
    }

  template <std::size_t count>
  Slice<T, count, stride>
    Last() const {
      return Last(count);
    }

  Slice<T, std::dynamic_extent, stride>
    DropFirst(std::size_t count) const {
      return Last(extent_ - count);
    }

  template <std::size_t count>
  Slice<T, extent - count, stride>
    DropFirst() const {
      return Last(extent_ - count);
    }

  Slice<T, std::dynamic_extent, stride>
    DropLast(std::size_t count) const {
      return First(extent_ - count);
    }

  // template <std::size_t count>
  // Slice<T, /*?*/, stride>
  //   DropLast() const;

  // Slice<T, /*?*/, /*?*/>
  //   Skip(std::ptrdiff_t skip) const;

  // template <std::ptrdiff_t skip>
  // Slice<T, /*?*/, /*?*/>
  //   Skip() const;

private:
  T* data_;
  std::size_t extent_;
  // std::ptrdiff_t stride_; 
};

//deduction guide

template <class T, std::size_t N>
Slice(T (&)[N]) -> Slice<T, N>;

template <class T, std::size_t N>
Slice(std::array<T, N>&) -> Slice<T, N>;

template <class T, std::size_t N>
Slice(const std::array<T, N>&) -> Slice<const T, N>;

template <class Cont>
Slice(Cont&) -> Slice<typename Cont::value_type>;

template <class Cont>
Slice(const Cont&) -> Slice<const typename Cont::value_type>;