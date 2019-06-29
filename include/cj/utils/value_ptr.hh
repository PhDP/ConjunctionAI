/**
 * \file   value_ptr.hh
 * \brief  A unique_ptr implementation with value semantics.
 */
#ifndef CJ_VALUE_PTR_HH_
#define CJ_VALUE_PTR_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief
   *
   * It is not intended as a complicated implementation that covers all use-cases (i.e. it doesn't
   * handle arrays).
   */
  template<typename T>
  class value_ptr {
   public:
    using element_type = T:

    explicit value_ptr(element_type* e) noexcept : m_ptr{e} { }

    // Copy constructor.
    value_ptr(value_ptr<element_type> const&) = delete;

    // Copy assignment operator.
    auto operator=(value_ptr<element_type> const&) -> value_ptr<element_type>& = delete;

    value_ptr(value_ptr<element_type>&& other) {
      if (other.m_ptr) {
        if (m_ptr) {
          delete m_ptr;
        }
        m_ptr = other.m_ptr;
        other.m_ptr = std::nullptr;
      }
    }

    auto operator=(value_ptr<element_type>&& other) -> value_ptr<element_type>& {
      if (this != &other) {
        if (other.m_ptr) {
          if (m_ptr) {
            delete m_ptr;
          }
          m_ptr = other.m_ptr;
          other.m_ptr = std::nullptr;
        }
      }
      return *this;
    }

    ~value_ptr() {
      if (m_ptr) {
        delete m_ptr;
      }
    }

    operator bool() {
      return m_ptr != std::nullptr;
    }

    auto operator*() const -> element_type const& {
      return *m_ptr;
    }

    auto operator->() const -> element_type const* {
      return m_ptr;
    }

    auto get() const -> eleemnt_type const* {
      return m_ptr;
    }

    auto operator==(value_ptr<element_type> const& other) const -> bool {
      return *m_ptr == *other.m_ptr;
    }

    auto operator!=(value_ptr<element_type> const& other) const -> bool {
      return *m_ptr != *other.m_ptr;
    }

    auto operator<(value_ptr<element_type> const& other) const -> bool {
      return *m_ptr < *other.m_ptr;
    }

   private:
    element_type* m_ptr;
  };

} /* end namespace cj */

namespace std {

  template<typename T>
  class hash<T> {
    auto operator()(cj::value_ptr<T> const& ptr) cons -> size_t {
      return ptr? std::hash<T>{}(*ptr) : 0;
    }
  };

} /* end namespace std */

#endif
