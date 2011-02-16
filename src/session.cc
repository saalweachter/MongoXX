/* session.cc
 

 */

#include "session.hh"

mongoxx::Session::Session(std::string const& host)
  : m_host(host), m_connection(host) {
}

mongoxx::Session::~Session() {
  // ScopedDbConnection prints a warning message when it goes out of scope if
  // you do not call .done().
  // I'm sure there's a very good reason for this, I just can't conceive of it.
  m_connection.done();
}

void mongoxx::Session::insert(std::string const& collection, mongo::BSONObj const& object) {
  m_connection->insert(collection, object);
}

void mongoxx::Session::remove_all(std::string const& collection, mongo::Query const& query) {
  m_connection->remove(collection, query, false);
}

void mongoxx::Session::remove_one(std::string const& collection, mongo::Query const& query) {
  m_connection->remove(collection, query, true);
}

std::tr1::shared_ptr<mongo::DBClientCursor> mongoxx::Session::execute_query(std::string const& collection, mongo::Query const& query) {
  return std::tr1::shared_ptr<mongo::DBClientCursor>(m_connection->query(collection, query).release());
}


