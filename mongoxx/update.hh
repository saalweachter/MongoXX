/* update.hh
   How we specify updates.

   Jack Saalweachter
*/

#ifndef MONGOXX_UPDATE_HH
#define MONGOXX_UPDATE_HH

#include "mongoxx/field.hh"

namespace mongoxx {

  template <typename T>
  class Update {
  public:
    virtual ~Update() { }

    virtual mongo::BSONObj apply(Mapper<T> const* mapper) const = 0;
  };

  template <template <class T, class U> class Member, typename T, typename U>
  class MemberIncrement : public Update<T> {
  public:
    MemberIncrement(Member<T, U> const& member, U const& value)
      : m_member(member), m_value(value) { }

    mongo::BSONObj apply(Mapper<T> const* mapper) const {
      mongo::BSONObjBuilder builder2;
      builder2.append(m_member.field_name(mapper), m_value);
      mongo::BSONObjBuilder builder;
      builder.append("$inc", builder2.obj());
      return builder.obj();
    }

  private:
    Member<T, U> m_member;
    U m_value;
  };

  template <template <class T, class U> class Member, typename T, typename U, typename V>
  MemberIncrement<Member, T, U> operator += (Member<T, U> const& member, V const& value) {
    return MemberIncrement<Member, T, U>(member, value);
  }


};

#endif

