/* field.hh
   Field's are used to reference the field of a class (and its mapped BSON
   object) so that we can refer to them in filters and updates.

   A Field<T, U> can only be created by a Mapper<T>, which is intended to
   protect the user against type/name typos.

   Jack Saalweachter
*/

#ifndef MONGOXX_FIELD_HH
#define MONGOXX_FIELD_HH

#include "update.hh"
#include "filter.hh"

namespace mongoxx {

  template <typename T> class Mapper;

  template <typename T, typename U>
  class Field {
  public:

    Mapper<T> const* mapper() const { return m_mapper; }
    std::string const& name() const { return m_name; }

    Field& operator = (Field const& a) {
      m_mapper = a.m_mapper;
      m_name = a.m_name;
      return *this;
    }

    template <typename V> Update operator += (V const& v) const {
      mongo::BSONObjBuilder builder;
      builder.append(name(), v);
      return Update("$inc", builder.obj());
    }

    template <typename V> Update operator = (V const& v) const {
      mongo::BSONObjBuilder builder;
      builder.append(name(), v);
      return Update("$set", builder.obj());
    }


    template <typename V> Filter operator == (V const& v) const {
      mongo::BSONObjBuilder builder;
      builder.append(name(), v);
      return Filter(builder.obj());
    }

    template <typename V> Filter operator != (V const& v) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObjBuilder builder2;
      builder2.append("$ne", v);
      builder.append(name(), builder2.obj());
      return Filter(builder.obj());
    }

    template <typename V> Filter operator < (V const& v) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObjBuilder builder2;
      builder2.append("$lt", v);
      builder.append(name(), builder2.obj());
      return Filter(builder.obj());
    }

    template <typename V> Filter operator > (V const& v) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObjBuilder builder2;
      builder2.append("$gt", v);
      builder.append(name(), builder2.obj());
      return Filter(builder.obj());
    }

    template <typename V> Filter operator <= (V const& v) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObjBuilder builder2;
      builder2.append("$lte", v);
      builder.append(name(), builder2.obj());
      return Filter(builder.obj());
    }

    template <typename V> Filter operator >= (V const& v) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObjBuilder builder2;
      builder2.append("$gte", v);
      builder.append(name(), builder2.obj());
      return Filter(builder.obj());
    }


  private:
    friend class Mapper<T>;

    Field(Mapper<T> const* mapper, std::string const& name)
      : m_mapper(mapper), m_name(name) { }

    Mapper<T> const* m_mapper;
    std::string m_name;
  };

};

#endif

