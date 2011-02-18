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
  T _decode(mongo::BSONElement const& element,
		  mongo::BSONType type, std::string const& type_name,
		  T (mongo::BSONElement::*result)() const) {
    if (element.type() != type) {
      throw bson_error("BSONElement is not of " + type_name + " type.");
    }
    return (element.*result)();
  }


  template <typename T>
  class BSONElementDecoder;

  template <>
  class BSONElementDecoder<std::string> {
  public:
    std::string decode(mongo::BSONElement const& elem) {
      return _decode(elem, mongo::String, "string", &mongo::BSONElement::str);
    }
  };

  template <>
  class BSONElementDecoder<int> {
  public:
    int decode(mongo::BSONElement const& elem) {
      return _decode(elem, mongo::NumberInt, "int", &mongo::BSONElement::Int);
    }
  };

  template <>
  class BSONElementDecoder<unsigned int> {
  public:
    unsigned int decode(mongo::BSONElement const& elem) {
      return _decode(elem, mongo::NumberInt, "int", &mongo::BSONElement::Int);
    }
  };

  template <>
  class BSONElementDecoder<long long> {
  public:
    long long decode(mongo::BSONElement const& e) {
      return _decode(e, mongo::NumberLong, "long", &mongo::BSONElement::Long);
    }
  };

  template <>
  class BSONElementDecoder<bool> {
  public:
    bool decode(mongo::BSONElement const& elem) {
      return _decode(elem, mongo::Bool, "boolean", &mongo::BSONElement::Bool);
    }
  };

  template <>
  class BSONElementDecoder<double> {
  public:
    double decode(mongo::BSONElement const& e) {
      return _decode(e, mongo::NumberDouble, "double", &mongo::BSONElement::Double);
    }
  };


  template <typename T>
  class BSONObjectDecoder {
  public:
    T decode(mongo::BSONObj const& obj, std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      return BSONElementDecoder<T>().decode(element);
    }
  };

};

#endif

