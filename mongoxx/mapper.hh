/* mapper.hh
   The object-document-model.
   Mapper<T> is how the user specifies how his objects should be mapped to a
   object in a MongoDB.



   Jack Saalweachter
*/

#ifndef MONGOXX_MAPPER_HH
#define MONGOXX_MAPPER_HH

#include "bson_decoder.hh"

namespace mongoxx {

  template <typename T>
  class Mapper {
  public:

    Mapper() { }
    ~Mapper() {
      for (typename std::vector<Field*>::iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	delete *i;
      }
    }

    Mapper(Mapper const& mapper) {
      for (typename std::vector<Field*>::const_iterator i = mapper.m_fields.begin(); i != mapper.m_fields.end(); ++i) {
	m_fields.push_back((*i)->clone());
      }
    }

    Mapper& operator = (Mapper const& mapper) {
      if (&mapper == this) return *this;
      for (typename std::vector<Field*>::iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	delete *i;
      }
      m_fields.clear();

      for (typename std::vector<Field*>::const_iterator i = mapper.m_fields.begin(); i != mapper.m_fields.end(); ++i) {
	m_fields.push_back((*i)->clone());
      }

      return *this;
    }

    template <typename U>
    Mapper& add_field(std::string const& name, U T::*field) {
      m_fields.push_back(new DirectField<U>(name, field));
      return *this;
    }

    template <typename U>
    Mapper& add_field(std::string const& name,
		      U (T::*getter)(), void (T::*setter)(U const&)) {
      m_fields.push_back(new IndirectField<U>(name, getter, setter));
      return *this;
    }

    template <typename U>
    std::string const& lookup_field(U T::*field) const {
      for (typename std::vector<Field*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	if (DirectField<U> const* f = dynamic_cast<DirectField<U> const*>(*i)) {
	  if (f->field() == field)
	    return f->name();
	}
      }
      throw std::invalid_argument("Attempted to lookup an unmapped field.");
    }

    template <typename U>
    std::string const& lookup_field(U (T::*getter)()) const {
      for (typename std::vector<Field*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	if (IndirectField<U> const* f = dynamic_cast<IndirectField<U> const*>(*i)) {
	  if (f->getter() == getter)
	    return f->name();
	} 
      }
      throw std::invalid_argument("Attempted to lookup an unmapped getter.");
    }

    std::string to_json(T const& t) const {
      return to_bson(t).jsonString();
    }

    void to_bson(T const& t, mongo::BSONObj &target) const {
      mongo::BSONObjBuilder builder;
      for (typename std::vector<Field*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	(*i)->to_bson(t, builder);
      }
      target = builder.obj();
    }

    mongo::BSONObj to_bson(T const& t) const {
      mongo::BSONObj res;
      to_bson(t, res);
      return res;
    }

    void from_bson(mongo::BSONObj const& bson, T &t) const {
      for (typename std::vector<Field*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	(*i)->from_bson(bson, t);
      }
    }

    T from_bson(mongo::BSONObj const& bson) const {
      T t;
      from_bson(bson, t);
      return t;
    }

  private:
    class Field {
    public:
      Field(std::string const& name) : m_name(name) { }
      virtual ~Field() { }
      virtual Field* clone() const = 0;

      std::string const& name() const { return m_name; }
      virtual void to_bson(T const&, mongo::BSONObjBuilder&) const = 0;
      virtual void from_bson(mongo::BSONObj const&, T &t) const = 0;

    private:
      std::string m_name;
    };

    template <typename U>
    class DirectField : public Field {
    public:
      DirectField(std::string const& name, U T::*field)
	: Field(name), m_field(field) { }

      Field* clone() const {
	return new DirectField(this->name(), field());
      }

      U T::* field() const { return m_field; }
      void to_bson(T const& t, mongo::BSONObjBuilder &builder) const {
	builder.append(this->name(), t.*m_field);
      }
      void from_bson(mongo::BSONObj const &bson, T &t) const {
	t.*m_field = decode_field<U>(bson, this->name());
      }

    private:
      U T::*m_field;
    };

    template <typename U>
    class IndirectField : public Field {
    public:
      typedef U (T::*GETTER)();
      typedef void (T::*SETTER)(U const&);

      IndirectField(std::string const& name, GETTER getter, SETTER setter)
	: Field(name), m_getter(getter), m_setter(setter) { }

      Field* clone() const {
	return new IndirectField(this->name(), getter(), setter());
      }

      GETTER getter() const { return m_getter; }
      SETTER setter() const { return m_setter; }
      void to_bson(T const& t, mongo::BSONObjBuilder &builder) const {
	builder.append(this->name(), (t.*m_getter));
      }
      void from_bson(mongo::BSONObj const &bson, T &t) const {
	(t.*m_setter)(decode_field<U>(bson, this->name()));
      }

    private:
      GETTER m_getter;
      SETTER m_setter;
    };


    std::vector<Field*> m_fields;
  };
};

#endif
