/* TestFilters.cc
   Test that filters work as intended.

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


TEST(FilterTest_equals) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);
  mapper.add_field("age", &Person::age);
  mapper.add_field("random_long", &Person::random_long);
  mapper.add_field("alive", &Person::alive);
  mapper.add_field("weight", &Person::weight);

  CHECK_EQUAL("{ \"first_name\" : \"Jack\" }", (mapper[&Person::first_name] == "Jack").to_bson().jsonString());
  CHECK_EQUAL("{ \"last_name\" : \"Saalweachter\" }", (mapper[&Person::last_name] == "Saalweachter").to_bson().jsonString());
  CHECK_EQUAL("{ \"age\" : 28 }", (mapper[&Person::age] == 28).to_bson().jsonString());
  CHECK_EQUAL("{ \"random_long\" : 42424242 }", (mapper[&Person::random_long] == 42424242).to_bson().jsonString());
  CHECK_EQUAL("{ \"alive\" : true }", (mapper[&Person::alive] == true).to_bson().jsonString());
  CHECK_EQUAL("{ \"weight\" : 210.5 }", (mapper[&Person::weight] == 210.5).to_bson().jsonString());

}


TEST(FilterTest_less) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);
  mapper.add_field("age", &Person::age);
  mapper.add_field("random_long", &Person::random_long);
  mapper.add_field("alive", &Person::alive);
  mapper.add_field("weight", &Person::weight);

  CHECK_EQUAL("{ \"first_name\" : { \"$lt\" : \"Jack\" } }", (mapper[&Person::first_name] < "Jack").to_bson().jsonString());
  CHECK_EQUAL("{ \"last_name\" : { \"$lt\" : \"Saalweachter\" } }", (mapper[&Person::last_name] < "Saalweachter").to_bson().jsonString());
  CHECK_EQUAL("{ \"age\" : { \"$lt\" : 28 } }", (mapper[&Person::age] < 28).to_bson().jsonString());
  CHECK_EQUAL("{ \"random_long\" : { \"$lt\" : 42424242 } }", (mapper[&Person::random_long] < 42424242).to_bson().jsonString());
  CHECK_EQUAL("{ \"weight\" : { \"$lt\" : 210.5 } }", (mapper[&Person::weight] < 210.5).to_bson().jsonString());

}


TEST(FilterTest_greater) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);
  mapper.add_field("age", &Person::age);
  mapper.add_field("random_long", &Person::random_long);
  mapper.add_field("alive", &Person::alive);
  mapper.add_field("weight", &Person::weight);

  CHECK_EQUAL("{ \"first_name\" : { \"$gt\" : \"Jack\" } }", (mapper[&Person::first_name] > "Jack").to_bson().jsonString());
  CHECK_EQUAL("{ \"last_name\" : { \"$gt\" : \"Saalweachter\" } }", (mapper[&Person::last_name] > "Saalweachter").to_bson().jsonString());
  CHECK_EQUAL("{ \"age\" : { \"$gt\" : 28 } }", (mapper[&Person::age] > 28).to_bson().jsonString());
  CHECK_EQUAL("{ \"random_long\" : { \"$gt\" : 42424242 } }", (mapper[&Person::random_long] > 42424242).to_bson().jsonString());
  CHECK_EQUAL("{ \"weight\" : { \"$gt\" : 210.5 } }", (mapper[&Person::weight] > 210.5).to_bson().jsonString());

}


TEST(FilterTest_less_and_greater) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);
  mapper.add_field("age", &Person::age);
  mapper.add_field("random_long", &Person::random_long);
  mapper.add_field("alive", &Person::alive);
  mapper.add_field("weight", &Person::weight);

  CHECK_EQUAL("{ \"age\" : { \"$gt\" : 25 } , \"$lt\" : 30 }", (mapper[&Person::age] > 25, mapper[&Person::age] < 30).to_bson().jsonString());
  CHECK_EQUAL("{ \"random_long\" : { \"$gt\" : 42424242 } }", (mapper[&Person::random_long] > 42424242).to_bson().jsonString());
  CHECK_EQUAL("{ \"weight\" : { \"$gt\" : 210.5 } }", (mapper[&Person::weight] > 210.5).to_bson().jsonString());

}

