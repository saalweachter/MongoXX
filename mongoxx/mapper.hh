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
      m_fields.push_back(direct_member<U>(name,
					  member_direct(field),
					  BasicCoder<U>()));
      return *this;
    }

    template <typename U, typename Alloc>
    Mapper& add_field(std::string const& name, std::vector<U, Alloc> T::*field, Mapper<U> const& mapper) {
      m_fields.push_back(direct_member<U>(name, member_direct(field),
					  array_coder<U, Alloc>(mapper_coder(mapper))));
      return *this;
    }

    template <typename U>
    Mapper& add_field(std::string const& name,
		      U const& (T::*getter)() const, void (T::*setter)(U const&)) {
      m_fields.push_back(indirect_member<U>(name,
					    member_fxns_indirect<U const&>(getter, setter),
					    BasicCoder<U>()));
      return *this;
    }

    template <typename U>
    Mapper& add_field(std::string const& name,
		      U (T::*getter)() const, void (T::*setter)(U)) {
      m_fields.push_back(indirect_member<U>(name,
					    member_fxns_indirect<U>(getter,
								    setter),
					    BasicCoder<U>()));
      return *this;
    }

    template <typename U>
    std::string const& lookup_field(U T::*member) const {
      for (typename std::vector<Member*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	if (TypedMember<U> const* typed = dynamic_cast<TypedMember<U>*>(*i)) {
	  if (typed->matches(member)) {
	    return typed->name();
	  }
	}
      }
      throw std::invalid_argument("Attempted to lookup an unmapped field.");
    }

    template <typename U>
    std::string const& lookup_field(U const& (T::*getter)()) const {
      for (typename std::vector<Member*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	if (TypedMember<U> const* typed = dynamic_cast<TypedMember<U>*>(*i)) {
	  if (typed->matches(getter)) {
	    return typed->name();
	  }
	}
      }
      throw std::invalid_argument("Attempted to lookup an unmapped field.");
    }

    template <typename U>
    std::string const& lookup_field(U (T::*getter)()) const {
      for (typename std::vector<Member*>::const_iterator i = m_fields.begin(); i != m_fields.end(); ++i) {
	if (TypedMember<U> const* typed = dynamic_cast<TypedMember<U>*>(*i)) {
	  if (typed->matches(getter)) {
	    return typed->name();
	  }
	}
      }
      throw std::invalid_argument("Attempted to lookup an unmapped field.");
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
      virtual ~Member() { }
      virtual Member* clone() const = 0;

      virtual std::string const& name() const = 0;
      virtual void to_bson(T const&, mongo::BSONObjBuilder&) const = 0;
      virtual void from_bson(mongo::BSONObj const&, T &t) const = 0;
    };

    template <typename U>
    class TypedMember : public Member {
    public:
      virtual ~TypedMember() { }

      virtual bool matches(U T::*) const = 0;
      virtual bool matches(U (T::*)()) const = 0;
      virtual bool matches(U const& (T::*)()) const = 0;
    };

    template <typename U, typename ACCESSOR, typename CODER>
    class DirectMember : public TypedMember<U> {
    public:
      DirectMember(std::string const& name, ACCESSOR accessor, CODER coder)
	: m_name(name), m_accessor(accessor), m_coder(coder) { }

      DirectMember* clone() const { return new DirectMember(*this); }

      bool matches(U T::*m) const { return m_accessor.matches(m); }
      bool matches(U (T::*m)()) const { return m_accessor.matches(m); }
      bool matches(U const& (T::*m)()) const { return m_accessor.matches(m); }

      std::string const& name() const { return m_name; }
      void to_bson(T const& t, mongo::BSONObjBuilder &builder) const {
	builder.append(name(), m_coder.encode(m_accessor.get_const(t)));
      }
      void from_bson(mongo::BSONObj const& bson, T &t) const {
	if (not bson.hasField(m_name.c_str())) {
	  throw bson_error("Field '" + m_name + "' is not in the BSON object.");
	}
	mongo::BSONElement element = bson.getField(m_name.c_str());
	m_coder.decode(m_accessor.get(t), element);
      }

    private:
      std::string m_name;
      ACCESSOR m_accessor;
      CODER m_coder;
    };

    template <typename U, typename ACCESSOR, typename CODER>
    static DirectMember<U, ACCESSOR, CODER>*
    direct_member(std::string const& name, ACCESSOR accessor, CODER coder) {
      return new DirectMember<U, ACCESSOR, CODER>(name, accessor, coder);
    }

    template <typename U, typename ACCESSOR, typename CODER>
    class IndirectMember : public Member {
    public:
      IndirectMember(std::string const& name, ACCESSOR accessor, CODER coder)
	: m_name(name), m_accessor(accessor), m_coder(coder) { }

      IndirectMember* clone() const { return new IndirectMember(*this); }

      bool matches(U T::*m) const { return m_accessor.matches(m); }
      bool matches(U (T::*m)()) const { return m_accessor.matches(m); }
      bool matches(U const& (T::*m)()) const { return m_accessor.matches(m); }

      std::string const& name() const { return m_name; }
      void to_bson(T const& t, mongo::BSONObjBuilder &builder) const {
	builder.append(name(), m_coder.encode(m_accessor.get_const(t)));
      }
      void from_bson(mongo::BSONObj const& bson, T &t) const {
	if (not bson.hasField(this->name().c_str())) {
	  throw bson_error("Field '" + this->name() + "' is not in the BSON object.");
	}
	mongo::BSONElement element = bson.getField(this->name().c_str());
	U u;
	m_coder.decode(u, element);
	m_accessor.set(t, u);
      }

    private:
      std::string m_name;
      ACCESSOR m_accessor;
      CODER m_coder;
    };

    template <typename U, typename ACCESSOR, typename CODER>
    static IndirectMember<U, ACCESSOR, CODER>*
    indirect_member(std::string const& name, ACCESSOR accessor, CODER coder) {
      return new IndirectMember<U, ACCESSOR, CODER>(name, accessor, coder);
    }

    template <typename U>
    class MemberDirect {
    public:
      MemberDirect(U T::*member) : m_member(member) { }

      bool matches(U T::*m) const { return m == m_member; }
      template <typename K> bool matches(K) const { return false; }

      U const& get_const(T const& t) const { return t.*m_member; }
      U& get(T &t) const { return t.*m_member; }

    private:
      U T::*m_member;
    };

    template <typename U>
    static MemberDirect<U> member_direct(U T::*member) {
      return MemberDirect<U>(member);
    }

    template <typename U, typename GET_CONST, typename GET>
    class MemberFxnsDirect {
    public:
      MemberFxnsDirect(GET_CONST get_const, GET get)
	: m_get_const(get_const), m_get(get) { }

      bool matches(GET_CONST m) const { return m == m_get_const; }
      template <typename K> bool matches(K) const { return false; }

      U const& get_const(T const& t) const { return (t.*m_get_const)(); }
      U& get(T &t) const { return (t.*m_get)(); }

    private:
      GET_CONST m_get_const;
      GET m_get;
    };

    template <typename U, typename GET_CONST, typename GET>
    static MemberFxnsDirect<U, GET_CONST, GET>
    member_fxns_direct(GET_CONST get_const, GET get) {
      return MemberFxnsDirect<U, GET_CONST, GET>(get_const, get);
    }

    template <typename U, typename GET_CONST, typename SET>
    class MemberFxnsIndirect {
    public:
      MemberFxnsIndirect(GET_CONST get_const, SET set)
	: m_get_const(get_const), m_set(set) { }

      bool matches(GET_CONST m) const { return m == m_get_const; }
      template <typename K> bool matches(K) const { return false; }

      U get_const(T const& t) const { return (t.*m_get_const)(); }
      void set(T &t, U u) const { (t.*m_set)(u); }

    private:
      GET_CONST m_get_const;
      SET m_set;
    };

    template <typename U, typename GET_CONST, typename SET>
    static MemberFxnsIndirect<U, GET_CONST, SET>
    member_fxns_indirect(GET_CONST get_const, SET set) {
      return MemberFxnsIndirect<U, GET_CONST, SET>(get_const, set);
    }

    std::vector<Member*> m_fields;
  };

  template <typename U>
  class MapperCoder {
  public:
    MapperCoder(Mapper<U> const& mapper) : m_mapper(mapper) { }

    mongo::BSONObj encode(U const& u) const { return m_mapper.to_bson(u); }
    void decode(U &u, mongo::BSONElement const& bson) const {
      _check(bson, mongo::Object, "object");
      m_mapper.from_bson(bson.Obj(), u);
    }

  private:
    Mapper<U> m_mapper;
  };

  template <typename U>
  static MapperCoder<U> mapper_coder(Mapper<U> const& mapper) {
    return MapperCoder<U>(mapper);
  }

};

#endif
