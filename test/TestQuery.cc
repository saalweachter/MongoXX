/* TestQuery.C
   Test that queries can query.  And all that.

*/

#include "UnitTest++.h"

#include "mongoxx/mongoxx.hh"

using namespace mongoxx;


struct PersonQ {
  std::string first_name;
  std::string last_name;
  unsigned int age;
};


TEST(Filter_equals) {
  Mapper<PersonQ> mapper;
  mapper.add_field("first_name", &PersonQ::first_name);
  mapper.add_field("last_name", &PersonQ::last_name);

  mongo::BSONObj bson = (field(&PersonQ::first_name) == "Jack").apply(&mapper);
  CHECK_EQUAL("{ \"first_name\" : \"Jack\" }", bson.jsonString());
}


TEST(Query_filter_equals) {
  Session session("localhost");

  Mapper<PersonQ> mapper;
  mapper.add_field("first_name", &PersonQ::first_name);
  mapper.add_field("last_name", &PersonQ::last_name);

  session.query("test.query_filter_equals", &mapper).remove_all();

  Inserter<PersonQ> inserter = session.inserter("test.query_filter_equals", &mapper);

  PersonQ person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  PersonQ person2 = { "John", "Saalweachter" };
  inserter.insert(person2);

  PersonQ person3 = { "Jack", "Saalwaechter" };
  inserter.insert(person3);

  PersonQ person4 = { "John", "Saalwaechter" };
  inserter.insert(person4);


  CHECK_EQUAL(4U, session.query("test.query_filter_equals", &mapper).all().size());

  CHECK_EQUAL(2U, session.query("test.query_filter_equals", &mapper).filter(field(&PersonQ::first_name) == "Jack").all().size());

  CHECK_EQUAL(1U, session.query("test.query_filter_equals", &mapper).filter(field(&PersonQ::first_name) == "Jack").filter(field(&PersonQ::last_name) == "Saalweachter").all().size());

  CHECK_EQUAL(0U, session.query("test.query_filter_equals", &mapper).filter(field(&PersonQ::first_name) == "Joe").all().size());

}


TEST(Query_filter_remove_all) {
  Session session("localhost");

  Mapper<PersonQ> mapper;
  mapper.add_field("first_name", &PersonQ::first_name);
  mapper.add_field("last_name", &PersonQ::last_name);

  session.query("test.query_filter_remove_all", &mapper).remove_all();

  Inserter<PersonQ> inserter = session.inserter("test.query_filter_remove_all", &mapper);

  PersonQ person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  PersonQ person2 = { "John", "Saalweachter" };
  inserter.insert(person2);

  PersonQ person3 = { "Jack", "Saalwaechter" };
  inserter.insert(person3);

  PersonQ person4 = { "John", "Saalwaechter" };
  inserter.insert(person4);


  CHECK_EQUAL(4U, session.query("test.query_filter_remove_all", &mapper).all().size());

  session.query("test.query_filter_remove_all", &mapper).filter(field(&PersonQ::first_name) == "Jack").remove_all();

  CHECK_EQUAL(2U, session.query("test.query_filter_remove_all", &mapper).all().size());

}


TEST(Query_filter_remove_all_table) {
  Session session("localhost");

  Table<PersonQ> table("test.query_filter_remove_all_table");
  table.add_field("first_name", &PersonQ::first_name);
  table.add_field("last_name", &PersonQ::last_name);

  session.query(table).remove_all();

  Inserter<PersonQ> inserter = session.inserter(table);

  PersonQ person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  PersonQ person2 = { "John", "Saalweachter" };
  inserter.insert(person2);

  PersonQ person3 = { "Jack", "Saalwaechter" };
  inserter.insert(person3);

  PersonQ person4 = { "John", "Saalwaechter" };
  inserter.insert(person4);


  CHECK_EQUAL(4U, session.query(table).all().size());

  session.query(table).filter(table[&PersonQ::first_name] == "Jack").remove_all();

  CHECK_EQUAL(2U, session.query(table).all().size());

}



TEST(Query_filter_greater_than) {
  Session session("localhost");

  Table<PersonQ> table("test.query_filter_greater_than");
  table.add_field("first_name", &PersonQ::first_name);
  table.add_field("last_name", &PersonQ::last_name);
  table.add_field("age", &PersonQ::age);

  session.query(table).remove_all();

  Inserter<PersonQ> inserter = session.inserter(table);

  PersonQ person1 = { "Jack", "Saalweachter", 25 };
  inserter.insert(person1);

  PersonQ person2 = { "John", "Saalweachter", 26 };
  inserter.insert(person2);

  PersonQ person3 = { "Jack", "Saalwaechter", 27 };
  inserter.insert(person3);

  PersonQ person4 = { "John", "Saalwaechter", 28 };
  inserter.insert(person4);

  PersonQ person5 = { "John", "Saalwachter", 29 };
  inserter.insert(person5);


  CHECK_EQUAL(5U, session.query(table).all().size());

  CHECK_EQUAL(2U, session.query(table).filter(table[&PersonQ::age] > 27).all().size());
}


