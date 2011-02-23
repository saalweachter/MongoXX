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


struct Student {
  std::string first_name;
  std::string last_name;
  std::vector<int> grades;
};

TEST(VectorIntField_standard) {
  Mapper<Student> mapper;
  mapper.add_field("first_name", &Student::first_name);
  mapper.add_field("last_name", &Student::last_name);
  mapper.add_field("grades", &Student::grades);

  Student student1;
  student1.first_name = "Jack";
  student1.last_name = "Saalweachter";
  student1.grades.push_back(100);
  student1.grades.push_back(90);
  student1.grades.push_back(80);
  student1.grades.push_back(70);

  mongo::BSONObj bson = mapper.to_bson(student1);
  CHECK_EQUAL("{ \"first_name\" : \"Jack\", \"last_name\" : \"Saalweachter\", \"grades\" : [ 100, 90, 80, 70 ] }", bson.jsonString());

  Student student2 = mapper.from_bson(bson);

  CHECK_EQUAL(student1.first_name, student2.first_name);
  CHECK_EQUAL(student1.last_name, student2.last_name);
  CHECK_EQUAL(student1.grades.size(), student2.grades.size());
  CHECK_EQUAL(student1.grades[0], student2.grades[0]);
  CHECK_EQUAL(student1.grades[1], student2.grades[1]);
  CHECK_EQUAL(student1.grades[2], student2.grades[2]);
  CHECK_EQUAL(student1.grades[3], student2.grades[3]);

}


struct Student2 {
  std::string first_name;
  std::string last_name;
  std::vector<double> grades;
};

TEST(VectorIntField_field_mismatch_1) {
  Mapper<Student> mapper1;
  mapper1.add_field("first_name", &Student::first_name);
  mapper1.add_field("last_name", &Student::last_name);
  mapper1.add_field("grades", &Student::grades);

  Student student1;
  student1.first_name = "Jack";
  student1.last_name = "Saalweachter";
  student1.grades.push_back(100);
  student1.grades.push_back(90);
  student1.grades.push_back(80);
  student1.grades.push_back(70);

  mongo::BSONObj bson = mapper1.to_bson(student1);

  Mapper<Student2> mapper2;
  mapper2.add_field("first_name", &Student2::first_name);
  mapper2.add_field("last_name", &Student2::last_name);
  mapper2.add_field("grades", &Student2::grades);

  CHECK_THROW(mapper2.from_bson(bson), bson_error);

}


TEST(VectorIntField_field_mismatch_2) {
  Mapper<Student2> mapper1;
  mapper1.add_field("first_name", &Student2::first_name);
  mapper1.add_field("last_name", &Student2::last_name);
  mapper1.add_field("grades", &Student2::grades);

  Student2 student1;
  student1.first_name = "Jack";
  student1.last_name = "Saalweachter";
  student1.grades.push_back(100.5);
  student1.grades.push_back(90.5);
  student1.grades.push_back(80.5);
  student1.grades.push_back(70.5);

  mongo::BSONObj bson = mapper1.to_bson(student1);

  Mapper<Student> mapper2;
  mapper2.add_field("first_name", &Student::first_name);
  mapper2.add_field("last_name", &Student::last_name);
  mapper2.add_field("grades", &Student::grades);

  CHECK_THROW(mapper2.from_bson(bson), bson_error);

}


struct FriendlessPerson {
  std::string first_name;
  std::string last_name;
};

struct FriendfulPerson {
  std::string first_name;
  std::string last_name;
  std::vector<FriendlessPerson> friends;
};

TEST(MultiLevelPeople_1) {
  Mapper<FriendlessPerson> less_mapper;
  less_mapper.add_field("first_name", &FriendlessPerson::first_name);
  less_mapper.add_field("last_name", &FriendlessPerson::last_name);

  Mapper<FriendfulPerson> ful_mapper;
  ful_mapper.add_field("first_name", &FriendfulPerson::first_name);
  ful_mapper.add_field("last_name", &FriendfulPerson::last_name);
  ful_mapper.add_field("friends", &FriendfulPerson::friends, less_mapper);
  
  FriendfulPerson person1;
  person1.first_name = "Jack";
  person1.last_name = "Saalweachter";

  FriendlessPerson person2 = { "John", "Saalweachter" };
  FriendlessPerson person3 = { "Jack", "Saalwaechter" };

  person1.friends.push_back(person2);
  person1.friends.push_back(person3);

  CHECK_EQUAL("{ \"first_name\" : \"Jack\", \"last_name\" : \"Saalweachter\", \"friends\" : [ { \"first_name\" : \"John\", \"last_name\" : \"Saalweachter\" }, { \"first_name\" : \"Jack\", \"last_name\" : \"Saalwaechter\" } ] }", ful_mapper.to_json(person1));
}

