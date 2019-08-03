/**
 *
 */
#ifndef CJ_FORMULA_HH_
#define CJ_FORMULA_HH_

#include "cj/common.hh"
#include "cj/utils/value_ptr.hh"

namespace cj {

  template<typename A>
  class unary_conn;

  template<typename A>
  class binary_conn;

  template<typename A>
  class quantifier;

  using string = std::string;

  enum class unary_kind : uint32_t {
    negation,
    delta
  };

  enum class binary_kind : uint32_t {
    conjunction,
    weak_conjunction,
    disjunction,
    weak_disjunction,
    implication,
    equivalence,
    ex_disjunction
  };

  constexpr auto is_weak(binary_kind k) -> bool {
    switch(k) {
      case binary_kind::weak_conjunction:
      case binary_kind::weak_disjunction:
        return true;
      default:
        return false;
    }
  }

  enum class quantifier_kind : uint32_t {
    universal,
    existential,
    unique
  };

  auto precedence(unary_kind k) -> uint32_t {
    return 12;
  }

  auto precedence(binary_kind k) -> uint32_t {
    switch (k) {
      case binary_kind::conjunction:
        return 9;
      case binary_kind::weak_conjunction:
        return 9;
      case binary_kind::disjunction:
        return 7;
      case binary_kind::weak_disjunction:
        return 7;
      case binary_kind::implication:
        return 5;
      case binary_kind::equivalence:
        return 3;
      case binary_kind::ex_disjunction:
        return 1;
    }
  }

  template<typename A>
  using formula = std::variant<
    A,
    ptr<unary_conn<A>>,
    ptr<binary_conn<A>>,
    ptr<quantifier<A>>>;

  class logic_symbols {
   public:
    template<typename K, typename V>
    using map_type = boost::container::flat_map<K, V>;

    logic_symbols(
      map_type<unary_kind, string> const& u,
      map_type<binary_kind, string> const& b,
      map_type<quantifier_kind, string> const& q,
      map_type<string, string> const& others) : m_u{u}, m_b{b}, m_q{q}, m_others(others) {}

    auto operator()(unary_kind k) const -> string {
      return m_u.at(k);
    }

    auto operator()(binary_kind k) const -> string {
      return m_b.at(k);
    }

    auto operator()(quantifier_kind k) const -> string {
      return m_q.at(k);
    }

    auto operator()(string const& s) const -> string {
      return m_others.at(s);
    }

   private:
    map_type<unary_kind, string> m_u;
    map_type<binary_kind, string> m_b;
    map_type<quantifier_kind, string> m_q;
    map_type<string, string> m_others;
  };

  static const auto unicode = logic_symbols{
    {{unary_kind::negation,"¬"}, {unary_kind::delta,"Δ"}},
    {{binary_kind::conjunction,"⊗"}, {binary_kind::weak_conjunction,"∧"}, {binary_kind::disjunction,"⊕"}, {binary_kind::weak_disjunction,"∨"}, {binary_kind::implication,"⇒"}, {binary_kind::equivalence,"⇔"}, {binary_kind::ex_disjunction,"⊻"}},
    {{quantifier_kind::universal,"∀"}, {quantifier_kind::existential,"∃"}, {quantifier_kind::unique,"∃!"}},
    {}
  };

  template<typename A>
  class unary_conn {
   public:
    unary_conn(formula<A> e, unary_kind k) : m_child{std::move(e)}, m_kind{k} {}

    auto kind() const -> unary_kind {
      return m_kind;
    }

    auto set_kind(unary_kind k) -> void {
      m_kind = k;
    }

    auto child() const -> formula<A> const& {
      return m_child;
    }

    auto child() -> formula<A>& {
      return m_child;
    }

    auto get() -> formula<A>* {
      return &m_child;
    }

   private:
    formula<A> m_child;
    unary_kind m_kind;
  };

  template<typename T>
  auto operator==(unary_conn<T> const& lhs, unary_conn<T> const& rhs) -> bool {
    return lhs.kind() == rhs.kind() && lhs.child() == rhs.child();
  }

  template<typename T>
  auto operator!=(unary_conn<T> const& lhs, unary_conn<T> const& rhs) -> bool {
    return lhs.kind() != rhs.kind() || lhs.child() != rhs.child();
  }

  namespace std {
    template<typename A>
    struct hash<unary_conn<A>> {
      auto operator()(unary_conn<A> const& u) const -> size_t {
        auto seed = size_t{17606328859805673430LU};
        std_hash_combine(seed, u.kind());
        std_hash_combine(seed, u.child());
        return seed;
      }
    };
  }

  template<typename A>
  class binary_conn {
   public:
    // Using move and forward?!?!?!?!?!?
    binary_conn(formula<A> lhs, formula<A> rhs, binary_kind k)
      : m_lchild{std::move(lhs)}, m_rchild{std::move(rhs)}, m_kind{k} {}

    auto kind() const -> binary_kind {
      return m_kind;
    }

    auto set_kind(binary_kind k) -> void {
      m_kind = k;
    }

    auto lchild() const -> formula<A> const& {
      return m_lchild;
    }

    auto rchild() const -> formula<A> const& {
      return m_rchild;
    }

    auto lchild() -> formula<A>& {
      return m_lchild;
    }

    auto rchild() -> formula<A>& {
      return m_rchild;
    }

   private:
    formula<A> m_lchild;
    formula<A> m_rchild;
    binary_kind m_kind;
  };

  template<typename A>
  auto operator==(binary_conn<A> const& lhs, binary_conn<A> const& rhs) -> bool {
    return lhs.kind() == rhs.kind() && lhs.lchild() == rhs.lchild() && lhs.rchild() == rhs.rchild();
  }

  template<typename A>
  auto operator!=(binary_conn<A> const& lhs, binary_conn<A> const& rhs) -> bool {
    return lhs.kind() != rhs.kind() || lhs.lchild() != rhs.lchild() || lhs.rchild() != rhs.rchild();
  }

  namespace std {
    template<typename A>
    struct hash<binary_conn<A>> {
      auto operator()(binary_conn<A> const& b) const -> size_t {
        auto seed = size_t{4043318905421095638LU};
        std_hash_combine(seed, b.kind());
        std_hash_combine(seed, b.lchild());
        std_hash_combine(seed, b.rchild());
        return seed;
      }
    };
  }

  template<typename A>
  class quantifier {
   public:
    quantifier(string const& var, formula<A> child, quantifier_kind k)
      : m_var{var}, m_child{std::move(child)}, m_kind{k} {}

    auto kind() const -> quantifier_kind {
      return m_kind;
    }

    auto set_kind(quantifier_kind k) -> void {
      m_kind = k;
    }

    auto variable() const -> string const& {
      return m_var;
    }

    auto child() const -> formula<A> const& {
      return m_child;
    }

    auto child() -> formula<A>& {
      return m_child;
    }

   private:
    string m_var;
    formula<A> m_child;
    quantifier_kind m_kind;
  };

  template<typename A>
  auto operator==(quantifier<A> const& lhs, quantifier<A> const& rhs) -> bool {
    return lhs.kind() == rhs.kind() && lhs.variable() == rhs.variable() && lhs.child() == rhs.child();
  }

  template<typename A>
  auto operator!=(quantifier<A> const& lhs, quantifier<A> const& rhs) -> bool {
    return lhs.kind() != rhs.kind() || lhs.variable() != rhs.variable() || lhs.child() == rhs.child();
  }

  namespace std {
    template<typename A>
    struct hash<quantifier<A>> {
      auto operator()(quantifier<A> const& q) const -> size_t {
        auto seed = size_t{5391282501167644692LU};
        std_hash_combine(seed, q.kind());
        std_hash_combine(seed, q.variable());
        std_hash_combine(seed, q.child());
        return seed;
      }
    };
  }

  template<typename A>
  inline auto operator!(formula<A>&& f) -> formula<A> {
    return ptr<unary_conn<A>>{new unary_conn<A>{std::move(f), unary_kind::negation}};
  }

  template<typename A>
  inline auto operator&&(formula<A>&& lhs, formula<A>&& rhs) -> formula<A> {
    return ptr<binary_conn<A>>{new binary_conn<A>{std::move(lhs), std::move(rhs), binary_kind::conjunction}};
  }

  template<typename A>
  inline auto operator&(formula<A>&& lhs, formula<A>&& rhs) -> formula<A> {
    return ptr<binary_conn<A>>{new binary_conn<A>{std::move(lhs), std::move(rhs), binary_kind::weak_conjunction}};
  }

  template<typename A>
  inline auto operator||(formula<A>&& lhs, formula<A>&& rhs) -> formula<A> {
    return ptr<binary_conn<A>>{new binary_conn<A>{std::move(lhs), std::move(rhs), binary_kind::disjunction}};
  }

  template<typename A>
  inline auto operator|(formula<A>&& lhs, formula<A>&& rhs) -> formula<A> {
    return ptr<binary_conn<A>>{binary_conn<A>{std::move(lhs), std::move(rhs), binary_kind::weak_disjunction}};
  }

  template<typename A>
  inline auto operator^(formula<A>&& lhs, formula<A>&& rhs) -> formula<A> {
    return ptr<binary_conn<A>>{binary_conn<A>{std::move(lhs), std::move(rhs), binary_kind::ex_disjunction}};
  }

  template<typename A>
  struct deep_cpy_vstr {
    auto operator()(A const& a) -> formula<A> {
      return formula<A>{a};
    }

    auto operator()(ptr<unary_conn<A>> const& u) -> formula<A> {
      return ptr<unary_conn<A>>(new unary_conn<A>{
        std::move(std::visit(deep_cpy_vstr<A>{}, u->child())), u->kind()
      });
    }

    auto operator()(ptr<binary_conn<A>> const& c) -> formula<A> {
      return ptr<binary_conn<A>>(new binary_conn<A>{
        std::move(std::visit(deep_cpy_vstr<A>{}, c->lchild())),
        std::move(std::visit(deep_cpy_vstr<A>{}, c->rchild())),
        c->kind()
      });
    }

    auto operator()(ptr<quantifier<A>> const& q) -> formula<A> {
      return ptr<quantifier<A>>(new quantifier<A>{
        q->variable(),
        std::move(std::visit(deep_cpy_vstr<A>{}, q->child())),
        q->kind()
      });
    }
  };

  template<typename A>
  struct show_vstr {
    std::ostream& m_os;
    logic_symbols const& m_sym;

    show_vstr(std::ostream& os, logic_symbols const& sym = unicode) : m_os(os), m_sym(sym) { }

    auto operator()(A const& a) -> void {
      m_os << a;
    }

    auto operator()(ptr<unary_conn<A>> const& u) -> void {
      m_os << m_sym(u->kind());
      std::visit(show_vstr<A>{m_os, m_sym}, u->child());
    }

    auto operator()(ptr<binary_conn<A>> const& c) -> void {
      m_os << '(';
      std::visit(show_vstr<A>{m_os, m_sym}, c->lchild());
      m_os << ' ' << m_sym(c->kind()) << ' ';
      std::visit(show_vstr<A>{m_os, m_sym}, c->rchild());
      m_os << ')';
    }

    auto operator()(ptr<quantifier<A>> const& q) -> void {
      m_os << m_sym(q->kind()) << ' ' << q->variable() << ": ";
      std::visit(show_vstr<A>{m_os, m_sym}, q->child());
    }
  };

  template<typename A>
  struct double_neg_elim_vstr {
    struct inner_vstr;

    auto operator()(A const& a) -> formula<A> {
      return formula<A>{a};
    }

    auto operator()(ptr<unary_conn<A>> const& u) -> formula<A> {
      if (u->kind() == unary_kind::negation) {
        return std::visit(inner_vstr{}, u->child());
      } else {
        return ptr<unary_conn<A>>{new unary_conn<A>{
          std::move(std::visit(double_neg_elim_vstr<A>{}, u->child())),
          unary_kind::delta
        }};
      }
    }

    auto operator()(ptr<binary_conn<A>> const& b) -> formula<A> {
      return ptr<binary_conn<A>>{new binary_conn<A>{
        std::move(std::visit(double_neg_elim_vstr<A>{}, b->lchild())),
        std::move(std::visit(double_neg_elim_vstr<A>{}, b->rchild())),
        b->kind()
      }};
    }

    auto operator()(ptr<quantifier<A>> const& q) -> formula<A> {
      return ptr<quantifier<A>>{new quantifier<A>{
        q->variable(),
        std::move(std::visit(double_neg_elim_vstr<A>{}, q->child())),
        q->kind()
      }};
    }

    struct inner_vstr {
      auto operator()(A const& a) -> formula<A> {
        return !formula<A>{a};
      }

      auto operator()(ptr<unary_conn<A>> const& u) -> formula<A> {
        if (u->kind() == unary_kind::negation) {
          return std::visit(double_neg_elim_vstr<A>{}, u->child());
        } else {
          return !formula<A>{ptr<unary_conn<A>>{new unary_conn<A>{
            std::move(std::visit(double_neg_elim_vstr<A>{}, u->child())),
            unary_kind::delta
          }}};
        }
      }

      auto operator()(ptr<binary_conn<A>> const& b) -> formula<A> {
        return !formula<A>{ptr<binary_conn<A>>{new binary_conn<A>{
          std::move(std::visit(double_neg_elim_vstr<A>{}, b->lchild())),
          std::move(std::visit(double_neg_elim_vstr<A>{}, b->rchild())),
          b->kind()
        }}};
      }

      auto operator()(ptr<quantifier<A>> const& q) -> formula<A> {
        return !formula<A>{ptr<quantifier<A>>{new quantifier<A>{
          q->variable(),
          std::move(std::visit(double_neg_elim_vstr<A>{}, q->child())),
          q->kind()
        }}};
      }

    };
  };

  template<typename A>
  auto operator<<(std::ostream& os, formula<A> const& f) -> std::ostream& {
    std::visit(show_vstr<A>{os}, f);
    return os;
  }

  template<typename A>
  auto double_neg_elim(formula<A> const& f) -> formula<A> {
    return std::visit(double_neg_elim_vstr<A>{}, f);
  }

  auto main() -> int {
    using fm = formula<string>;
    auto kb = std::unordered_set<fm> {};
    auto x = !!!!!!!!!(!fm{string{"x"}} & !!!!fm{string{"y"}});
    auto y = double_neg_elim(x);
    auto z = double_neg_elim(x);
    kb.insert(std::move(x));
    kb.insert(std::move(y));
    kb.insert(std::move(z));
    for (auto const& f : kb) {
      std::cout << f << '\n';
    }
    return 0;
  }

} /* end namespace cj */

#endif
