/* bson_decoder.hh
   Decodes bson-fields in a template-friendly way.

   This is just to provide a mapping for the primitive types.  Specifically,
   this provides a default mapping for ints, bools, doubles, strings, and
   homogenous maps and lists of these.

   Jack Saalweachter
*/

#ifndef MONGOXX_BSON_DECODER_HH
#define MONGOXX_BSON_DECODER_HH

#include "mongo/client/dbclient.h"

namespace mongoxx {

  class bson_error : public std::runtime_error {
  public:
    explicit bson_error(std::string const &message) : runtime_error(message) { }
  };

  
  inline void _check(mongo::BSONElement const& element,
		     mongo::BSONType type, std::string const& type_name) {
    if (element.type() != type) {
      throw bson_error("BSONElement is not of " + type_name + " type.");
    }
  }


  template <typename T>
  class BSONDecoderBackend;

  template <>
  class BSONDecoderBackend<std::string> {
  public:
    static void decode(std::string &s, mongo::BSONElement const& element) {
      _check(element, mongo::String, "string");
      s = element.String();
    }
  };

  template <>
  class BSONDecoderBackend<int> {
  public:
    static void decode(int &i, mongo::BSONElement const& element) {
      _check(element, mongo::NumberInt, "int");
      i = element.Int();
    }
  };

  template <>
  class BSONDecoderBackend<unsigned int> {
  public:
    static void decode(unsigned int &ui, mongo::BSONElement const& element) {
      _check(element, mongo::NumberInt, "int");
      ui = element.Int();
    }
  };

  template <>
  class BSONDecoderBackend<long long> {
  public:
    static void decode(long long &l, mongo::BSONElement const& element) {
      _check(element, mongo::NumberLong, "long");
      l = element.Long();
    }
  };

  template <>
  class BSONDecoderBackend<unsigned long long> {
  public:
    static void decode(unsigned long long &l, mongo::BSONElement const& element) {
      _check(element, mongo::NumberLong, "long");
      l = element.Long();
    }
  };

  template <>
  class BSONDecoderBackend<bool> {
  public:
    static void decode(bool &b, mongo::BSONElement const& element) {
      _check(element, mongo::Bool, "boolean");
      b = element.Bool();
    }
  };

  template <>
  class BSONDecoderBackend<double> {
  public:
    static void decode(double &d, mongo::BSONElement const& element) {
      _check(element, mongo::NumberDouble, "double");
      d = element.Double();
    }
  };

  template <typename K, typename Alloc>
  class BSONDecoderBackend<std::vector<K, Alloc> > {
  public:
    static void decode(std::vector<K, Alloc> &v,
		       mongo::BSONElement const& element) {
      _check(element, mongo::Array, "array");
      std::vector<mongo::BSONElement> elements = element.Array();
      for (std::vector<mongo::BSONElement>::const_iterator i = elements.begin();
	   i != elements.end(); ++i) {
	K k;
	BSONDecoderBackend<K>::decode(k, *i);
	v.push_back(k);
      }
    }
  };

  template <typename V, typename Compare, typename Alloc>
  class BSONDecoderBackend<std::map<std::string, V, Compare, Alloc> > {
  public:
    static void decode(std::map<std::string, V, Compare, Alloc> &m,
		       mongo::BSONElement const& element) {
      _check(element, mongo::Object, "object");
      mongo::BSONObj obj = element.Obj();
      std::set<std::string> fields;
      obj.getFieldNames(fields);
      for (std::set<std::string>::const_iterator i = fields.begin();
	   i != fields.end(); ++i) {
	V v;
	BSONDecoderBackend<V>::decode(v, obj.getField(*i));
	m[*i] = v;
      }
    }
  };


  template <typename T>
  void decode_element(T &t, mongo::BSONElement const element) {
    BSONDecoderBackend<T>::decode(t, element);
  }

  template <typename T>
  T decode_element(mongo::BSONElement const& element) {
    T t;
    decode_element(t, element);
    return t;
  }

  template <typename T>
  void decode_field(T &t, mongo::BSONObj const& obj,
		    std::string const& field_name) {
    if (not obj.hasField(field_name.c_str())) {
      throw bson_error("Field '" + field_name + "' is not in the BSON object.");
    }
    mongo::BSONElement element = obj.getField(field_name);
    decode_element(t, element);
  }

  template <typename T>
  T decode_field(mongo::BSONObj const& obj, std::string const& field_name) {
    T t;
    decode_field(t, obj, field_name);
    return t;
  }

};

#endif

