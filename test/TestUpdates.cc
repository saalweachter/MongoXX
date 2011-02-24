/* TestUpdates.cc
   Test the our 'update' commands work.

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


TEST(UpdateTest_equal) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);
  mapper.add_field("age", &Person::age);
  mapper.add_field("random_long", &Person::random_long);
  mapper.add_field("alive", &Person::alive);
  mapper.add_field("weight", &Person::weight);

  CHECK_EQUAL("{ \"$set\" : { \"first_name\" : \"Jack\" } }", (mapper[&Person::first_name] = "Jack").to_bson().jsonString());
  CHECK_EQUAL("{ \"$set\" : { \"last_name\" : \"Saalweachter\" } }", (mapper[&Person::last_name] = "Saalweachter").to_bson().jsonString());
  CHECK_EQUAL("{ \"$set\" : { \"age\" : 28 } }", (mapper[&Person::age] = 28).to_bson().jsonString());
  CHECK_EQUAL("{ \"$set\" : { \"random_long\" : 42424242 } }", (mapper[&Person::random_long] = 42424242).to_bson().jsonString());
  CHECK_EQUAL("{ \"$set\" : { \"alive\" : true } }", (mapper[&Person::alive] = true).to_bson().jsonString());
  CHECK_EQUAL("{ \"$set\" : { \"weight\" : 210.5 } }", (mapper[&Person::weight] = 210.5).to_bson().jsonString());

}

