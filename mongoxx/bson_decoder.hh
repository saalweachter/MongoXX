/* bson_decoder.hh
   Decodes bson-fields in a template-friendly way.

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
  class BSONDecoder;

  template <typename T>
  T _super_decode(mongo::BSONObj const& obj, std::string const& field_name,
		  mongo::BSONType type, std::string const& type_name,
		  T (mongo::BSONElement::*result)() const) {
    if (not obj.hasField(field_name.c_str())) {
      throw bson_error("Field '" + field_name + "' is not in the BSON object.");
    }
    mongo::BSONElement element = obj.getField(field_name);
    if (element.type() != type) {
      throw bson_error("Field '" + field_name + "' is not of " + type_name + " type.");
    }
    return (element.*result)();
  }

  template <>
  class BSONDecoder<std::string> {
  public:
    std::string decode(mongo::BSONObj const& obj, std::string const& field_name) {
      return _super_decode(obj, field_name, mongo::String, "string",
			   &mongo::BSONElement::str);
    }
  };

  template <>
  class BSONDecoder<int> {
  public:
    int decode(mongo::BSONObj const& obj, std::string const& field_name) {
      return _super_decode(obj, field_name, mongo::NumberInt, "int",
			   &mongo::BSONElement::Int);
    }
  };

  template <>
  class BSONDecoder<unsigned int> {
  public:
    unsigned int decode(mongo::BSONObj const& obj, std::string const& field_name) {
      return _super_decode(obj, field_name, mongo::NumberInt, "int",
			   &mongo::BSONElement::Int);
    }
  };

  template <>
  class BSONDecoder<long long> {
  public:
    long long decode(mongo::BSONObj const& obj, std::string const& field_name) {
      return _super_decode(obj, field_name, mongo::NumberLong, "long",
			   &mongo::BSONElement::Long);
    }
  };

  template <>
  class BSONDecoder<bool> {
  public:
    bool decode(mongo::BSONObj const& obj, std::string const& field_name) {
      return _super_decode(obj, field_name, mongo::Bool, "boolean",
			   &mongo::BSONElement::Bool);
    }
  };

  template <>
  class BSONDecoder<double> {
  public:
    double decode(mongo::BSONObj const& obj, std::string const& field_name) {
      return _super_decode(obj, field_name, mongo::NumberDouble, "double",
			   &mongo::BSONElement::Double);
    }
  };

};

#endif

