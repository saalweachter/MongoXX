/* table.hh
   Try to make the query syntax a little less crazy.

*/

#ifndef MONGOXX_TABLE_HH
#define MONOGXX_TABLE_HH

#include "field.hh"
#include "mapper.hh"

namespace mongoxx {

  /**
   * The Table object wraps a collection name and its Mapper object.
   */
  template <typename T>
  class Table {
  public:

    /**
     * Constructs a new Table using just a collection name.  You will need to
     * map fields after the fact.
     * @param collection the name of the collection
     */
    Table(std::string const& collection) : m_collection(collection) { }

    /**
     * Constructs a new Table using both a collection name and a Mapper.  The
     * Table will copy and own the mapper.
     * @param collection the name of the collection
     * @param mapper the mapper mapping the fields of the collection
     */
    Table(std::string const& collection, Mapper<T> const& mapper)
      : m_collection(collection), m_mapper(mapper) { }

    /**
     * Gets the name of the collection.
     * @return a reference to the name of the collection
     */
    std::string const& collection() const { return m_collection; }

    /**
     * Gets the mapper mapping the field of the collection.
     * @return a pointer to the mapper
     */
    Mapper<T> const* mapper() const { return &m_mapper; }

    /**
     * Access a mapped field.  This mapped field may be used to specify updates
     * or query filters.
     * @param member a pointer to a mapped member
     * @return a mapped Field corresponding to the member
     * @throws std::invalid_argument if the member is not mapped
     */
    template <typename U>
    Field<T, U> operator[](U T::*member) const { return m_mapper[member]; }

    /**
     * Maps a field.  Adds the specified member to the underlying Mapper.
     * @param name the name of the field in the database
     * @param field the field of the object to map it to
     */
    template <typename U>
    Table& add_field(std::string const& name, U T::*field) {
      m_mapper.add_field(name, field);
      return *this;
    }

    /**
     * Maps a field.  Adds the specified member to the underlying Mapper, using
     * the getter and setter to access it.
     * @param name the name of the field in the database
     * @param getter a function to access the field in the object
     * @param setter a function to set the field in the object
     */
    template <typename U>
    Table& add_field(std::string const& name,
		      U (T::*getter)(), void (T::*setter)(U)) {
      m_mapper.add_field(name, getter, setter);
      return *this;
    }


  private:
    std::string m_collection;
    Mapper<T> m_mapper;
  };

};

#endif

