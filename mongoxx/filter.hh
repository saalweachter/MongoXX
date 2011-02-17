/* filter.hh
   This is really about filters, actually.

*/

#ifndef MONGOXX_FIELD_HH
#define MONGOXX_FIELD_HH

namespace mongoxx {
  template <typename T, typename U>
  class Field {
  public:
    Field(U T::*field) : m_field(field) { }

    std::string const& field_name(Mapper<T> const* mapper) const {
      return mapper->lookup_field(m_field);
    }

  private:
    U T::*m_field;
  };

  template <typename T, typename U>
  class Method {
  public:
    Method(U (T::*method)()) : m_method(method) { }

    std::string const& field_name(Mapper<T> const* mapper) const {
      return mapper->lookup_field(m_method);
    }

  private:
    U (T::*m_method)();
  };

  template <typename T, typename U>
  Field<T, U> field(U T::*field) { return Field<T, U>(field); }
  template <typename T, typename U>
  Method<T, U> method(U (T::*method)()) { return Method<T, U>(method); }

  template <typename T>
  class Filter {
  public:
    virtual ~Filter() { }

    virtual mongo::BSONObj apply(Mapper<T> const* mapper) const = 0;
  };

  template <template <class T, class U> class Member, typename T, typename U>
  class MemberEqual : public Filter<T> {
  public:
    MemberEqual(Member<T, U> const& member, U const& value)
      : m_member(member), m_value(value) { }

    mongo::BSONObj apply(Mapper<T> const* mapper) const {
      mongo::BSONObjBuilder builder;
      builder.append(m_member.field_name(mapper), m_value);
      return builder.obj();
    }

  private:
    Member<T, U> m_member;
    U m_value;
  };

  template <template <class T, class U> class Member, typename T, typename U>
  MemberEqual<Member, T, U> operator == (Member<T, U> const& member, U const& value) {
    return MemberEqual<Member, T, U>(member, value);
  }
};

#endif

