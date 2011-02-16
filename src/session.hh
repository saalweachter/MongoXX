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

  class Session {
  public:
    Session(std::string const& host);
    ~Session();

    template <typename T>
    Query<T> query(std::string const& collection, Mapper<T> *mapper) {
      return Query<T>(this, collection, mapper);
    }

    template <typename T>
    Inserter<T> inserter(std::string const& collection, Mapper<T> *mapper) {
      return Inserter<T>(this, collection, mapper);
    }

    template <typename T>
    QueryResult<T> execute_query(std::string const& collection,
				 mongo::Query const& query, Mapper<T> *mapper) {
      return QueryResult<T>(execute_query(collection, query), mapper);
    }

    void insert(std::string const& collection, mongo::BSONObj const& object);
    void remove_all(std::string const& collection, mongo::Query const& query);
    void remove_one(std::string const& collection, mongo::Query const& query);
    std::tr1::shared_ptr<mongo::DBClientCursor>
    execute_query(std::string const& collection, mongo::Query const& query);
 
  private:
    std::string m_host;
    mongo::ScopedDbConnection m_connection;
  };


  template <typename T>
  class Inserter {
  public:
    Inserter(Session *session, std::string const& collection, Mapper<T> *mapper)
      : m_session(session), m_collection(collection), m_mapper(mapper) { }

    void insert(T const& t) {
      mongo::BSONObj object;
      m_mapper->to_bson(t, object);
      m_session->insert(m_collection, object);
    }

  private:
    Session *m_session;
    std::string m_collection;
    Mapper<T> *m_mapper;
  };

};

#endif

