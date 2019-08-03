/**
 * \file   value_ptr.hh
 * \brief  A simple unique_ptr implementation with value semantics.
 */
#ifndef CJ_VALUE_PTR_HH_
#define CJ_VALUE_PTR_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief A value pointer with value semantics (==, !=, <, and std::hash will use the data owned
   *        by the pointer, not the pointer itself). It is not intended as a complicated
   *        implementation that covers all use-cases (i.e. it does not handle arrays).
   */
  template<typename T>
  class value_ptr {
   public:
    using element_type = T;

    /**
     * \brief Builds from a pointer. The newly formed object will now owned the data.
     */
    explicit value_ptr(element_type* e) noexcept
      : m_ptr{e} {
    }

    /**
     * \brief True for non-null pointers.
     */
    operator bool() const {
      return m_ptr != nullptr;
    }

    /**
     * \brief Returns the object.
     */
    auto operator*() const -> element_type const& {
      return *m_ptr;
    }

    /**
     * \brief Returns a pointer to the object owned by the object.
     */
    auto operator->() const -> element_type const* {
      return m_ptr;
    }

    /**
     * \brief Returns a copy of the pointer.
     */
    auto get() const -> element_type const* {
      return m_ptr;
    }

    /**
     * \brief Whether the values at the end of two non-null value_ptr are the same.
     */
    auto operator==(value_ptr<element_type> const& other) const -> bool;

    /**
     * \brief Whether the values at the end of two non-null value_ptr are different.
     */
    auto operator!=(value_ptr<element_type> const& other) const -> bool;

    /**
     * \brief Whether the values at the end of this non-null value_ptr is smaller than another.
     */
    auto operator<(value_ptr<element_type> const& other) const -> bool;

   private:
    std::unique_ptr<element_type> m_ptr;
  };

  template<typename T>
  auto value_ptr<T>::operator==(value_ptr<element_type> const& other) const -> bool {
    return bool(m_ptr) && bool(other.m_ptr)? *m_ptr == *other.m_ptr : !bool(m_ptr) && !bool(other.m_ptr);
  }

  template<typename T>
  auto value_ptr<T>::operator!=(value_ptr<element_type> const& other) const -> bool {
    return bool(m_ptr) && bool(other.m_ptr)? *m_ptr != *other.m_ptr : !bool(m_ptr) != !bool(other.m_ptr);
  }

  template<typename T>
  auto value_ptr<T>::operator<(value_ptr<element_type> const& other) const -> bool {
    return bool(m_ptr) && bool(other.m_ptr)? *m_ptr < *other.m_ptr : !bool(m_ptr) && bool(other.m_ptr);
  }

} /* end namespace cj */

namespace std {

  template<typename T>
  struct hash<cj::value_ptr<T>> {
    auto operator()(cj::value_ptr<T> const& p) const -> size_t {
      return bool(p)? hash<T>{}(*p) : 0;
    }
  };

} /* end namespace std */

#endif
