/* session.hh
   Manages the actual connection to the flippin' server.

   Jack Saalweachter
*/

#ifndef MONGOXX_SESSION_HH
#define MONGOXX_SESSION_HH

#include "mongo/client/dbclient.h"
#include "mongo/client/connpool.h"

#include <string>
#include <tr1/memory>

namespace mongoxx {
  template <typename T> class Mapper;
  template <typename T> class Inserter;
  template <typename T> class Query;
  template <typename T> class QueryResult;
  template <typename T> class Table;

  class Session {
  public:
    Session(std::string const& host) : m_host(host), m_connection(host) { }
    ~Session() {
      // ScopedDbConnection prints a warning message when it goes out of scope
      // if you do not call .done().
      // I'm sure there's a very good reason for this, I just can't conceive of
      // it.
      m_connection.done();
    }


    template <typename T>
    Query<T> query(std::string const& collection, Mapper<T> const* mapper) {
      return Query<T>(this, collection, mapper);
    }

    template <typename T>
    Query<T> query(Table<T> const& table) {
      return Query<T>(this, table.collection(), table.mapper());
    }

    template <typename T>
    Inserter<T> inserter(std::string const& collection, Mapper<T> const* mapper) {
      return Inserter<T>(this, collection, mapper);
    }

    template <typename T>
    Inserter<T> inserter(Table<T> const& table) {
      return Inserter<T>(this, table.collection(), table.mapper());
    }

    template <typename T>
    QueryResult<T> execute_query(std::string const& collection,
				 mongo::Query const& query,
				 unsigned int limit, unsigned int skip,
				 Mapper<T> const* mapper) {
      return QueryResult<T>(execute_query(collection, query, limit, skip),
			    mapper);
    }

    void insert(std::string const& collection, mongo::BSONObj const& object) {
      m_connection->insert(collection, object);
    }

    void remove_all(std::string const& collection, mongo::Query const& query) {
      m_connection->remove(collection, query, false);
    }

    void remove_one(std::string const& collection, mongo::Query const& query) {
      m_connection->remove(collection, query, true);
    }

    std::tr1::shared_ptr<mongo::DBClientCursor>
    execute_query(std::string const& collection, mongo::Query const& query, unsigned int limit, unsigned int skip) {
      return std::tr1::shared_ptr<mongo::DBClientCursor>(m_connection->query(collection, query, limit, skip).release());
    }

    void execute_update(std::string const& collection, mongo::Query const& query, mongo::BSONObj const& update) {
      m_connection->update(collection, query, update);
    }
 
  private:
    std::string m_host;
    mongo::ScopedDbConnection m_connection;
  };


  template <typename T>
  class Inserter {
  public:
    Inserter(Session *session, std::string const& collection, Mapper<T> const* mapper)
      : m_session(session), m_collection(collection), m_mapper(mapper) { }

    Inserter& insert(T const& t) {
      mongo::BSONObj object;
      m_mapper->to_bson(t, object);
      m_session->insert(m_collection, object);
      return *this;
    }

  private:
    Session *m_session;
    std::string m_collection;
    Mapper<T> const* m_mapper;
  };

};

#endif

