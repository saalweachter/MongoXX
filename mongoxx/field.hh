/* field.hh
   Field's are used to reference the field of a class (and its mapped BSON object)
   so that we can refer to them in filters and updates.

   Jack Saalweachter
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

};

#endif

