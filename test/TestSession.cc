/* TestSession.cc
   Test that whole session class, yo.

   Jack Saalweachter
*/

#include "UnitTest++.h"

#include "mongoxx/mongoxx.hh"

#include <string>

using namespace mongoxx;

TEST(Session_connect) {
  Session session("localhost");
}


struct Person {
  std::string first_name;
  std::string last_name;
};


TEST(Session_inserter) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  Inserter<Person> inserter = session.inserter("test.person", &mapper);
}


TEST(Session_insert) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  Inserter<Person> inserter = session.inserter("test.person_insert", &mapper);

  Person person = { "Jack", "Saalweachter" };
  inserter.insert(person);
}

TEST(Session_insert_is_there) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.person_insert_is_there", &mapper).remove_all();

  CHECK_THROW(session.query("test.person_insert_is_there", &mapper).first(),
	      query_error);

  Inserter<Person> inserter = session.inserter("test.person_insert_is_there",
					       &mapper);

  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  Person person2 = session.query("test.person_insert_is_there", &mapper).first();

  CHECK_EQUAL(person1.first_name, person2.first_name);
  CHECK_EQUAL(person1.last_name, person2.last_name);
}


TEST(Session_remove_all) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  Inserter<Person> inserter = session.inserter("test.person_remove_all", &mapper);

  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  Person person2 = { "John", "Saalweachter" };
  inserter.insert(person2);


  Person person3 = session.query("test.person_remove_all", &mapper).first();

  // We are implicitly relying on the default ordering.
  CHECK_EQUAL(person1.first_name, person3.first_name);
  CHECK_EQUAL(person1.last_name, person3.last_name);

  session.query("test.person_remove_all", &mapper).remove_all();

  CHECK_THROW(session.query("test.person_remove_all", &mapper).first(),
	      query_error);
}


TEST(Session_query) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.person", &mapper);
}


TEST(Session_query_first) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  Inserter<Person> inserter = session.inserter("test.person_query_first", &mapper);

  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  Person person2 = session.query("test.person_query_first", &mapper).first();

  CHECK_EQUAL(person1.first_name, person2.first_name);
  CHECK_EQUAL(person1.last_name, person2.last_name);
}


TEST(Session_query_first_not_there) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.person_query_first_not_there", &mapper).remove_all();

  CHECK_THROW(session.query("test.person_query_first_not_there", &mapper).first(), query_error);
}


TEST(Session_query_one) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  Inserter<Person> inserter = session.inserter("test.person_query_one", &mapper);

  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  Person person2 = session.query("test.person_query_one", &mapper).one();

  CHECK_EQUAL(person1.first_name, person2.first_name);
  CHECK_EQUAL(person1.last_name, person2.last_name);
}


TEST(Session_query_one_not_there) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.person_query_one_not_there", &mapper).remove_all();

  CHECK_THROW(session.query("test.person_query_one_not_there", &mapper).one(), query_error);
}


TEST(Session_query_all) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.person_query_all", &mapper).remove_all();

  CHECK_EQUAL(0U, session.query("test.person_query_all", &mapper).all().size());

  Inserter<Person> inserter = session.inserter("test.person_query_all", &mapper);
  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);

  CHECK_EQUAL(1U, session.query("test.person_query_all", &mapper).all().size());

  Person person2 = { "John", "Saalweachter" };
  inserter.insert(person2);

  CHECK_EQUAL(2U, session.query("test.person_query_all", &mapper).all().size());

  Person person3 = { "John", "Saalwaechter" };
  inserter.insert(person3);

  CHECK_EQUAL(3U, session.query("test.person_query_all", &mapper).all().size());

  std::vector<Person> all = session.query("test.person_query_all", &mapper).all();

  CHECK_EQUAL(person1.first_name, all[0].first_name);
  CHECK_EQUAL(person1.last_name, all[0].last_name);
  CHECK_EQUAL(person2.first_name, all[1].first_name);
  CHECK_EQUAL(person2.last_name, all[1].last_name);
  CHECK_EQUAL(person3.first_name, all[2].first_name);
  CHECK_EQUAL(person3.last_name, all[2].last_name);
}

TEST(Session_query_remove_one) {
  Session session("localhost");

  Mapper<Person> mapper;
  mapper.add_field("first_name", &Person::first_name);
  mapper.add_field("last_name", &Person::last_name);

  session.query("test.person_query_remove_one", &mapper).remove_all();

  CHECK_EQUAL(0U, session.query("test.person_query_remove_one", &mapper).all().size());

  Inserter<Person> inserter = session.inserter("test.person_query_remove_one", &mapper);
  Person person1 = { "Jack", "Saalweachter" };
  inserter.insert(person1);
  Person person2 = { "John", "Saalweachter" };
  inserter.insert(person2);

  CHECK_EQUAL(2U, session.query("test.person_query_remove_one", &mapper).all().size());

  session.query("test.person_query_remove_one", &mapper).remove_one();

  CHECK_EQUAL(1U, session.query("test.person_query_remove_one", &mapper).all().size());
}


