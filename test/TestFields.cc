/* TestFields.cc
   Test the basic fields.

*/

#include "UnitTest++.h"

#include "mongoxx/mongoxx.hh"

#include <string>

struct Person {
  std::string first_name;
  std::string last_name;
  int age;
  long long random_long;
  bool alive;
  double weight;
};


using namespace mongoxx;


TEST(BasicTest_1) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);
  mapper.add_field("age", &Person::age);
  mapper.add_field("random_long", &Person::random_long);
  mapper.add_field("alive", &Person::alive);
  mapper.add_field("weight", &Person::weight);

  Person person = { "Jack", "Saalweachter", 28, 137, true, 210.8 };

  CHECK_EQUAL("{ \"first_name\" : \"Jack\", \"last_name\" : \"Saalweachter\", \"age\" : 28, \"random_long\" : 137, \"alive\" : true, \"weight\" : 210.8 }",
	      mapper.to_json(person));
}


TEST(Mapper_name_lookup_1) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);
  mapper.add_field("age", &Person::age);
  mapper.add_field("alive", &Person::alive);
  mapper.add_field("weight", &Person::weight);

  CHECK_EQUAL("first_name", mapper.lookup_field(&Person::first_name));
  CHECK_EQUAL("last_name", mapper.lookup_field(&Person::last_name));
  CHECK_EQUAL("age", mapper.lookup_field(&Person::age));
  CHECK_EQUAL("alive", mapper.lookup_field(&Person::alive));
  CHECK_EQUAL("weight", mapper.lookup_field(&Person::weight));

  CHECK_THROW(mapper.lookup_field(&Person::random_long), std::invalid_argument);
}


TEST(StringField_encode_decode) {
  // This tests string field encoding/decoding with no exceptions.
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  Person person1 = { "Jack", "Saalweachter", 28, 137 };

  mongo::BSONObj bson = mapper.to_bson(person1);

  Person person2 = mapper.from_bson(bson);

  CHECK_EQUAL(person1.first_name, person2.first_name);
  CHECK_EQUAL(person1.last_name, person2.last_name);
}


TEST(StringField_missing_field) {
  // This tests the exception thrown when a field is missing.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("first_name", &Person::first_name);

  Person person1 = { "Jack", "Saalweachter", 28, 137 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(StringField_mistyped_field) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("first_name", &Person::first_name);
  deficient_mapper.add_field("last_name", &Person::age); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(IntField_encode_decode) {
  // This tests string field encoding/decoding with no exceptions.
  Mapper<Person> mapper;
  mapper.add_field("age", &Person::age);

  Person person1 = { "Jack", "Saalweachter", 28, 137 };

  mongo::BSONObj bson = mapper.to_bson(person1);

  Person person2 = mapper.from_bson(bson);

  CHECK_EQUAL(person1.age, person2.age);
}


TEST(IntField_missing_field) {
  // This tests the exception thrown when a field is missing.
  Mapper<Person> deficient_mapper;

  Person person1 = { "Jack", "Saalweachter", 28, 137 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("age", &Person::age);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(IntField_mistyped_field) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("age", &Person::first_name); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("age", &Person::age);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(LongField_encode_decode) {
  // This tests string field encoding/decoding with no exceptions.
  Mapper<Person> mapper;
  mapper.add_field("random_long", &Person::random_long);

  Person person1 = { "Jack", "Saalweachter", 28, 137 };

  mongo::BSONObj bson = mapper.to_bson(person1);

  Person person2 = mapper.from_bson(bson);

  CHECK_EQUAL(person1.random_long, person2.random_long);
}


TEST(LongField_missing_field) {
  // This tests the exception thrown when a field is missing.
  Mapper<Person> deficient_mapper;

  Person person1 = { "Jack", "Saalweachter", 28, 137 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("random_long", &Person::random_long);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(LongField_mistyped_field_1) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("random_long", &Person::first_name); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("random_long", &Person::random_long);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(LongField_mistyped_field_2) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("random_long", &Person::age); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137, true };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("random_long", &Person::random_long);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(BoolField_encode_decode) {
  // This tests string field encoding/decoding with no exceptions.
  Mapper<Person> mapper;
  mapper.add_field("alive", &Person::alive);

  Person person1 = { "Jack", "Saalweachter", 28, 137, true };

  mongo::BSONObj bson = mapper.to_bson(person1);

  Person person2 = mapper.from_bson(bson);

  CHECK_EQUAL(person1.alive, person2.alive);
}


TEST(BoolField_missing_field) {
  // This tests the exception thrown when a field is missing.
  Mapper<Person> deficient_mapper;

  Person person1 = { "Jack", "Saalweachter", 28, 137, true };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("alive", &Person::alive);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(BoolField_mistyped_field_1) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("alive", &Person::first_name); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137, true };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("alive", &Person::alive);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(BoolField_mistyped_field_2) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("alive", &Person::age); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137, true };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("alive", &Person::alive);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(DoubleField_encode_decode) {
  // This tests string field encoding/decoding with no exceptions.
  Mapper<Person> mapper;
  mapper.add_field("weight", &Person::weight);

  Person person1 = { "Jack", "Saalweachter", 28, 137, true, 210.8 };

  mongo::BSONObj bson = mapper.to_bson(person1);

  Person person2 = mapper.from_bson(bson);

  CHECK_CLOSE(person1.weight, person2.weight, 0.01);
}


TEST(DoubleField_missing_field) {
  // This tests the exception thrown when a field is missing.
  Mapper<Person> deficient_mapper;

  Person person1 = { "Jack", "Saalweachter", 28, 137, true, 210.8 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("weight", &Person::weight);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(DoubleField_mistyped_field_1) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("weight", &Person::first_name); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137, true, 210.8 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("weight", &Person::weight);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


TEST(DoubleField_mistyped_field_2) {
  // This tests the exception thrown when a field is not the type we expect.
  Mapper<Person> deficient_mapper;
  deficient_mapper.add_field("weight", &Person::age); // whoops!

  Person person1 = { "Jack", "Saalweachter", 28, 137, true, 210.8 };
  mongo::BSONObj bson = deficient_mapper.to_bson(person1);

  Mapper<Person> mapper;
  mapper.add_field("weight", &Person::weight);

  CHECK_THROW(mapper.from_bson(bson), bson_error);
}


