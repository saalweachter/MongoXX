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

  
  template <typename T>
  void _decode(T &t, mongo::BSONElement const& element,
	       mongo::BSONType type, std::string const& type_name,
	       T (mongo::BSONElement::*result)() const) {
    if (element.type() != type) {
      throw bson_error("BSONElement is not of " + type_name + " type.");
    }
    t = (element.*result)();
  }


  template <typename T>
  class BSONDecoderBackend;

  template <>
  class BSONDecoderBackend<std::string> {
  public:
    static void decode(std::string &s, mongo::BSONElement const& elem) {
      _decode(s, elem, mongo::String, "string", &mongo::BSONElement::str);
    }
  };

  template <>
  class BSONDecoderBackend<int> {
  public:
    static void decode(int &i, mongo::BSONElement const& elem) {
      _decode(i, elem, mongo::NumberInt, "int", &mongo::BSONElement::Int);
    }
  };

  template <>
  class BSONDecoderBackend<unsigned int> {
  public:
    static void decode(unsigned int &ui, mongo::BSONElement const& elem) {
      int i = 0;
      _decode(i, elem, mongo::NumberInt, "int", &mongo::BSONElement::Int);
      ui = i;
    }
  };

  template <>
  class BSONDecoderBackend<long long> {
  public:
    static void decode(long long &l, mongo::BSONElement const& e) {
      _decode(l, e, mongo::NumberLong, "long", &mongo::BSONElement::Long);
    }
  };

  template <>
  class BSONDecoderBackend<bool> {
  public:
    static void decode(bool &b, mongo::BSONElement const& elem) {
      _decode(b, elem, mongo::Bool, "boolean", &mongo::BSONElement::Bool);
    }
  };

  template <>
  class BSONDecoderBackend<double> {
  public:
    static void decode(double &d, mongo::BSONElement const& e) {
      _decode(d, e, mongo::NumberDouble, "double", &mongo::BSONElement::Double);
    }
  };


  template <typename T>
  class BSONDecoder {
  public:
    static T decode_field(mongo::BSONObj const& obj,
			  std::string const& field_name) {
      T t;
      decode_field(t, obj, field_name);
      return t;
    }

    static void decode_field(T &t, mongo::BSONObj const& obj,
			     std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      decode_element(t, element);
    }

    static T decode_element(mongo::BSONElement const& element) {
      T t;
      decode_element(t, element);
      return t;
    }

    static void decode_element(T &t, mongo::BSONElement const element) {
      BSONDecoderBackend<T>::decode(t, element);
    }
  };

};

#endif

