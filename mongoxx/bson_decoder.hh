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

  template <>
  class BSONDecoder<std::string> {
  public:
    std::string decode(mongo::BSONObj const& obj, std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      if (element.type() != mongo::String) {
	throw bson_error("Field '" + field_name + "' is not of string type.");
      }
      return element.str();
    }
  };

  template <>
  class BSONDecoder<int> {
  public:
    int decode(mongo::BSONObj const& obj, std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      if (element.type() != mongo::NumberInt) {
	throw bson_error("Field '" + field_name + "' is not of int type.");
      }
      return element.Int();
    }
  };

  template <>
  class BSONDecoder<unsigned int> {
  public:
    unsigned int decode(mongo::BSONObj const& obj, std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      if (element.type() != mongo::NumberInt) {
	throw bson_error("Field '" + field_name + "' is not of int type.");
      }
      return element.Int();
    }
  };

  template <>
  class BSONDecoder<long long> {
  public:
    long long decode(mongo::BSONObj const& obj, std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      if (element.type() != mongo::NumberLong) {
	throw bson_error("Field '" + field_name + "' is not of long type.");
      }
      return element.Long();
    }
  };

  template <>
  class BSONDecoder<bool> {
  public:
    bool decode(mongo::BSONObj const& obj, std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      if (element.type() != mongo::Bool) {
	throw bson_error("Field '" + field_name + "' is not of boolean type.");
      }
      return element.Bool();
    }
  };

  template <>
  class BSONDecoder<double> {
  public:
    double decode(mongo::BSONObj const& obj, std::string const& field_name) {
      if (not obj.hasField(field_name.c_str())) {
	throw bson_error("Field '" + field_name + "' is not in the BSON object.");
      }
      mongo::BSONElement element = obj.getField(field_name);
      if (element.type() != mongo::NumberDouble) {
	throw bson_error("Field '" + field_name + "' is not of double type.");
      }
      return element.Double();
    }
  };

};

#endif

