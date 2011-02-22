/* filter.hh
   This is really about filters, actually.

*/

#ifndef MONGOXX_FILTER_HH
#define MONGOXX_FILTER_HH

#include "field.hh"

namespace mongoxx {

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

  template <template <class T, class U> class Member, typename T, typename U, typename V>
  MemberEqual<Member, T, U> operator == (Member<T, U> const& member, V const& value) {
    return MemberEqual<Member, T, U>(member, value);
  }



  template <template <class T, class U> class Member, typename T, typename U>
  class MemberLess : public Filter<T> {
  public:
    MemberLess(Member<T, U> const& member, U const& value)
      : m_member(member), m_value(value) { }

    mongo::BSONObj apply(Mapper<T> const* mapper) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObjBuilder builder2;
      builder2.append("$lt", m_value);
      builder.append(m_member.field_name(mapper), builder2.obj());
      return builder.obj();
    }

  private:
    Member<T, U> m_member;
    U m_value;
  };

  template <template <class T, class U> class Member, typename T, typename U, typename V>
  MemberLess<Member, T, U> operator < (Member<T, U> const& member, V const& value) {
    return MemberLess<Member, T, U>(member, value);
  }



  template <template <class T, class U> class Member, typename T, typename U>
  class MemberGreater : public Filter<T> {
  public:
    MemberGreater(Member<T, U> const& member, U const& value)
      : m_member(member), m_value(value) { }

    mongo::BSONObj apply(Mapper<T> const* mapper) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObjBuilder builder2;
      builder2.append("$gt", m_value);
      builder.append(m_member.field_name(mapper), builder2.obj());
      return builder.obj();
    }

  private:
    Member<T, U> m_member;
    U m_value;
  };

  template <template <class T, class U> class Member, typename T, typename U, typename V>
  MemberGreater<Member, T, U> operator > (Member<T, U> const& member, V const& value) {
    return MemberGreater<Member, T, U>(member, value);
  }

};

#endif

