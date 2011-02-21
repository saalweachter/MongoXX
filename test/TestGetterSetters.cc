/* TestGetterSetters.cc
   Test that getter/setter fields work.

   Jack Saalweachter
*/

#include "UnitTest++.h"

#include "mongoxx/mongoxx.hh"


using namespace mongoxx;


class PersonG {
public:
  PersonG() { }
  PersonG(std::string const& first_name, std::string const& last_name, int age)
    : m_first_name(first_name), m_last_name(last_name), m_age(age) { }

  std::string const& first_name() const { return m_first_name; }
  std::string const& last_name() const { return m_last_name; }
  int age() const { return m_age; }

  void set_first_name(std::string const& first_name) {
    m_first_name = first_name;
  }
  void set_last_name(std::string const& last_name) {
    m_last_name = last_name;
  }
  void set_age(int age) {
    m_age = age;
  }

private:
  std::string m_first_name;
  std::string m_last_name;
  int m_age;
};


TEST(Basic_1) {
  Mapper<PersonG> mapper;
  mapper.add_field("first_name", &PersonG::first_name, &PersonG::set_first_name);
  mapper.add_field("last_name", &PersonG::last_name, &PersonG::set_last_name);
  mapper.add_field("age", &PersonG::age, &PersonG::set_age);

  PersonG person1("Jack", "Saalweachter", 28);

  mongo::BSONObj bson = mapper.to_bson(person1);

  CHECK_EQUAL("{ \"first_name\" : \"Jack\", \"last_name\" : \"Saalweachter\", \"age\" : 28 }", bson.jsonString());

  PersonG person2 = mapper.from_bson(bson);

  CHECK_EQUAL(person1.first_name(), person2.first_name());
  CHECK_EQUAL(person1.last_name(), person2.last_name());
  CHECK_EQUAL(person1.age(), person2.age());
}


