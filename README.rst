Mongo++ is a library for using a Mongo database from C++.  It is open sourced under the MIT license; see the LICENSE file for details.

First things first, a quick getting started:

1. Mongo++ is header only.  Just copy the mongoxx/ directory to a convenient include directory, or add the repository to your include path.
2. ``#include <mongoxx/mongoxx.hh>``
3. When compiling, you'll need to link against all of the libraries required by Mongo: ``-lmongoclient -lboost_system -lboost_thread-mt -lboost_filesystem -lboost_program_options``


Now that that's out of the way, let's get down to the example code, which is also answers the question of "Why would I want to use Mongo++?".

Let's start with one of the great programming examples, the student grade database::

 struct Student {
   std::string first_name;
   std::string last_name;
   std::vector<unsigned int> grades;
 };

When using Mongo++, the first thing you'll do is define a Mapper for your datastructure(s)::

 using namespace mongoxx;
 
 Mapper<Student> mapper;
 mapper.add_field(&Student::first_name, "first_name"); 
 mapper.add_field(&Student::last_name, "last_name"); 
 mapper.add_field(&Student::grades, "grades"); 

Next, connect to a database::

 Session session("localhost");

Now the fun starts.  Let's imagine we need to insert students into the database::

 Inserter<Student> inserter = session.inserter("students", &mapper);

 Student student1, student2, student3;

 inserter.insert(student1);
 inserter.insert(student2);
 inserter.insert(student3);

Let's query!

Get all results from the database::

 std::vector<Student> students = session.query("students", &mapper).all();

Get one result from the database::

 Student student = session.query("students", &mapper).one();

By now, you might be getting tired of typing *"students", &mapper* all over the place.  The solution for this is to use a ``Table<>`` object::

 Table<Student> table("localhost", mapper);

 std::vector<Student> students = session.query(table).all();

 Student student = session.query(table).one();

The ``Table<>`` object make filtering a query easier::

 Student student = session.query(table).filter(table[&Student::first_name] == "John").filter(table[&Student::last_name] == "Doe").one();

