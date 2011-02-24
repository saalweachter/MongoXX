/* mapper.hh
   The object-document-model.
   Mapper<T> is how the user specifies how his objects should be mapped to a
   object in a MongoDB.



   Jack Saalweachter
*/

#ifndef MONGOXX_MAPPER_HH
#define MONGOXX_MAPPER_HH

#include "bson_decoder.hh"
#include "field.hh"

namespace mongoxx {

  template <typename T>
  class Mapper {
  public:

    Mapper() { }
    ~Mapper() {
      for (typename std::vector<Member*>::iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	delete *i;
      }
    }

    Mapper(Mapper const& mapper) {
      for (typename std::vector<Member*>::const_iterator i = mapper.m_fields.begin(); i != mapper.m_fields.end(); ++i) {
	m_fields.push_back((*i)->clone());
      }
    }

    Mapper& operator = (Mapper const& mapper) {
      if (&mapper == this) return *this;
      for (typename std::vector<Member*>::iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	delete *i;
      }
      m_fields.clear();

      for (typename std::vector<Member*>::const_iterator i = mapper.m_fields.begin(); i != mapper.m_fields.end(); ++i) {
	m_fields.push_back((*i)->clone());
      }

      return *this;
    }

    template <typename U>
    Mapper& add_field(std::string const& name, U T::*field) {
      m_fields.push_back(new DirectMember<U>(name, field));
      return *this;
    }

    template <typename U, typename Alloc>
    Mapper& add_field(std::string const& name, std::vector<U, Alloc> T::*field, Mapper<U> const& mapper) {
      m_fields.push_back(new MappedArrayDirectMember<U, Alloc>(name, field, mapper));
      return *this;
    }

    template <typename U>
    Mapper& add_field(std::string const& name,
		      U const& (T::*getter)() const, void (T::*setter)(U const&)) {
      m_fields.push_back(new IndirectMember<U>(name, getter, setter));
      return *this;
    }

    template <typename U>
    Mapper& add_field(std::string const& name,
		      U (T::*getter)() const, void (T::*setter)(U)) {
      m_fields.push_back(new IndirectMember2<U>(name, getter, setter));
      return *this;
    }

    template <typename U>
    std::string const& lookup_field(U T::*field) const {
      for (typename std::vector<Member*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	if (DirectMember<U> const* f = dynamic_cast<DirectMember<U> const*>(*i)) {
	  if (f->field() == field)
	    return f->name();
	}
      }
      throw std::invalid_argument("Attempted to lookup an unmapped field.");
    }

    template <typename U>
    std::string const& lookup_field(U const& (T::*getter)()) const {
      for (typename std::vector<Member*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	if (IndirectMember<U> const* f = dynamic_cast<IndirectMember<U> const*>(*i)) {
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
      for (typename std::vector<Member*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
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
      for (typename std::vector<Member*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	(*i)->from_bson(bson, t);
      }
    }

    T from_bson(mongo::BSONObj const& bson) const {
      T t;
      from_bson(bson, t);
      return t;
    }

    template <typename U>
    Field<T, U> operator[](U T::*field) const {
      return Field<T, U>(this, lookup_field(field));
    }


  private:
    class Member {
    public:
      Member(std::string const& name) : m_name(name) { }
      virtual ~Member() { }
      virtual Member* clone() const = 0;

      std::string const& name() const { return m_name; }
      virtual void to_bson(T const&, mongo::BSONObjBuilder&) const = 0;
      virtual void from_bson(mongo::BSONObj const&, T &t) const = 0;

    private:
      std::string m_name;
    };

    template <typename U>
    class DirectMember : public Member {
    public:
      DirectMember(std::string const& name, U T::*field)
	: Member(name), m_field(field) { }

      Member* clone() const {
	return new DirectMember(this->name(), field());
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

    template <typename U, typename Alloc>
    class MappedArrayDirectMember : public Member {
    public:
      MappedArrayDirectMember(std::string const& name, std::vector<U, Alloc> T::*field, Mapper<U> const& mapper)
	: Member(name), m_field(field), m_mapper(mapper) { }

      Member* clone() const {
	return new MappedArrayDirectMember(this->name(), field(), mapper());
      }

      std::vector<U, Alloc> T::* field() const { return m_field; }
      Mapper<U> const& mapper() const { return m_mapper; }
      void to_bson(T const& t, mongo::BSONObjBuilder &builder) const {
	mongo::BSONArrayBuilder arrbuilder;
	std::vector<U, Alloc> const& v = t.*m_field;
	for (typename std::vector<U, Alloc>::const_iterator i = v.begin(); i != v.end(); ++i) {
	  arrbuilder.append(m_mapper.to_bson(*i));
	}
	builder.append(this->name(), arrbuilder.arr());
      }
      void from_bson(mongo::BSONObj const &bson, T &t) const {
	(t.*m_field).clear();
	if (not bson.hasField(this->name().c_str())) {
	  throw bson_error("Member '" + this->name() + "' is not in the BSON object.");
	}
	mongo::BSONElement element = bson.getField(this->name());
	_check(element, mongo::Array, "array");
	std::vector<mongo::BSONElement> elements = element.Array();
	for (std::vector<mongo::BSONElement>::const_iterator i = elements.begin();
	     i != elements.end(); ++i) {
	  (t.*m_field).push_back(m_mapper.from_bson(i->Obj()));
	}
      }

    private:
      std::vector<U, Alloc> T::*m_field;
      Mapper<U> m_mapper;
    };

    template <typename U>
    class IndirectMember : public Member {
    public:
      typedef U const& (T::*GETTER)() const;
      typedef void (T::*SETTER)(U const&);

      IndirectMember(std::string const& name, GETTER getter, SETTER setter)
	: Member(name), m_getter(getter), m_setter(setter) { }

      Member* clone() const {
	return new IndirectMember(this->name(), getter(), setter());
      }

      GETTER getter() const { return m_getter; }
      SETTER setter() const { return m_setter; }
      void to_bson(T const& t, mongo::BSONObjBuilder &builder) const {
	builder.append(this->name(), (t.*m_getter)());
      }
      void from_bson(mongo::BSONObj const &bson, T &t) const {
	(t.*m_setter)(decode_field<U>(bson, this->name()));
      }

    private:
      GETTER m_getter;
      SETTER m_setter;
    };

    template <typename U>
    class IndirectMember2 : public Member {
    public:
      typedef U (T::*GETTER)() const;
      typedef void (T::*SETTER)(U);

      IndirectMember2(std::string const& name, GETTER getter, SETTER setter)
	: Member(name), m_getter(getter), m_setter(setter) { }

      Member* clone() const {
	return new IndirectMember2(this->name(), getter(), setter());
      }

      GETTER getter() const { return m_getter; }
      SETTER setter() const { return m_setter; }
      void to_bson(T const& t, mongo::BSONObjBuilder &builder) const {
	builder.append(this->name(), (t.*m_getter)());
      }
      void from_bson(mongo::BSONObj const &bson, T &t) const {
	(t.*m_setter)(decode_field<U>(bson, this->name()));
      }

    private:
      GETTER m_getter;
      SETTER m_setter;
    };


    std::vector<Member*> m_fields;
  };
};

#endif
