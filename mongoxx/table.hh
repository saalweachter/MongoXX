/* table.hh
   Try to make the query syntax a little less crazy.

*/

#ifndef MONGOXX_TABLE_HH
#define MONOGXX_TABLE_HH

#include "filter.hh"
#include "mapper.hh"

namespace mongoxx {

  template <typename T>
  class Table {
  public:
    Table(std::string const& collection) : m_collection(collection) { }
    Table(std::string const& collection, Mapper<T> const& mapper)
      : m_collection(collection), m_mapper(mapper) { }

    std::string const& collection() const { return m_collection; }
    Mapper<T> const* mapper() const { return &m_mapper; }

    // This doesn't actually do anything.
    template <typename U>
    Field<T, U> operator[](U T::*member) const { return field(member); }

    template <typename U>
    Table& add_field(std::string const& name, U T::*field) {
      m_mapper.add_field(name, field);
      return *this;
    }

    template <typename U>
    Table& add_field(std::string const& name,
		      U (T::*getter)(), void (T::*setter)(U const&)) {
      m_mapper.add_field(name, getter, setter);
      return *this;
    }

  private:
    std::string m_collection;
    Mapper<T> m_mapper;
  };

};

#endif

