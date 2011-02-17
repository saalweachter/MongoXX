/* TestQuery.C
   Test that queries can query.  And all that.

*/

#include "UnitTest++.h"

#include "mongoxx/mongoxx.hh"

using namespace mongoxx;


struct Person {
  std::string first_name;
  std::string last_name;
};


TEST(Filter_equals) {
  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  mongo::BSONObj bson = (field(&Person::first_name) == std::string("Jack")).apply(&mapper);
  CHECK_EQUAL("{ \"first_name\" : \"Jack\" }", bson.jsonString());
}


TEST(Query_filter_equals) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.query_filter_equals", &mapper).remove_all();

  Inserter<Person> inserter = session.inserter("test.query_filter_equals", &mapper);

  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  Person person2 = { "John", "Saalweachter" };
  inserter.insert(person2);

  Person person3 = { "Jack", "Saalwaechter" };
  inserter.insert(person3);

  Person person4 = { "John", "Saalwaechter" };
  inserter.insert(person4);


  CHECK_EQUAL(4U, session.query("test.query_filter_equals", &mapper).all().size());

  CHECK_EQUAL(2U, session.query("test.query_filter_equals", &mapper).filter(field(&Person::first_name) == std::string("Jack")).all().size());

  CHECK_EQUAL(1U, session.query("test.query_filter_equals", &mapper).filter(field(&Person::first_name) == std::string("Jack")).filter(field(&Person::last_name) == std::string("Saalweachter")).all().size());

  CHECK_EQUAL(0U, session.query("test.query_filter_equals", &mapper).filter(field(&Person::first_name) == std::string("Joe")).all().size());

}


TEST(Query_filter_remove_all) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.query_filter_remove_all", &mapper).remove_all();

  Inserter<Person> inserter = session.inserter("test.query_filter_remove_all", &mapper);

  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  Person person2 = { "John", "Saalweachter" };
  inserter.insert(person2);

  Person person3 = { "Jack", "Saalwaechter" };
  inserter.insert(person3);

  Person person4 = { "John", "Saalwaechter" };
  inserter.insert(person4);


  CHECK_EQUAL(4U, session.query("test.query_filter_remove_all", &mapper).all().size());

  session.query("test.query_filter_remove_all", &mapper).filter(field(&Person::first_name) == std::string("Jack")).remove_all();

  CHECK_EQUAL(2U, session.query("test.query_filter_remove_all", &mapper).all().size());

}

