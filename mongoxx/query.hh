/* query.hh
   Queries!

*/

#ifndef MONGOXX_QUERY_HH
#define MONGOXX_QUERY_HH

#include "mapper.hh"
#include "session.hh"
#include "filter.hh"
#include "update.hh"

#include <string>
#include <stdexcept>
#include <tr1/memory>

namespace mongoxx {
  class Session;

  class query_error : public std::runtime_error {
  public:
    explicit query_error(std::string const &message) : runtime_error(message) { }
  };

  template <typename T>
  class QueryResult {
  public:
    QueryResult(std::tr1::shared_ptr<mongo::DBClientCursor> const& cursor, Mapper<T> const* mapper)
      : m_cursor(cursor), m_mapper(mapper) { }

    T first() const {
      if (m_cursor->more()) {
	mongo::BSONObj obj = m_cursor->next();
	return m_mapper->from_bson(obj);
      }
      throw query_error("Query returned no results; cannot return the first element.");
    }
    T one() const { return first(); }

    bool next(T &t) const {
      if (m_cursor->more()) {
	mongo::BSONObj obj = m_cursor->next();
	m_mapper->from_bson(obj, t);
	return true;
      }
      return false;
    }
    bool first(T &t) const {
      return next(t);
    }
    bool one(T &t) const {
      return next(t);
    }

    bool more() const {
      return m_cursor->more();
    }
    T next() const {
      if (m_cursor->more()) {
	mongo::BSONObj obj = m_cursor->next();
	return m_mapper->from_bson(obj);
      }
      throw query_error("Query results are empty; cannot return any more results.");
    }

    std::vector<T> all() const {
      std::vector<T> res;
      for (T t; next(t); res.push_back(t));
      return res;
    }

  private:
    std::tr1::shared_ptr<mongo::DBClientCursor> m_cursor;
    Mapper<T> const* m_mapper;
  };

  template <typename T>
  class Query {
  public:
    Query(Session *session, std::string const& collection, Mapper<T> const* mapper)
      : m_session(session), m_collection(collection), m_mapper(mapper),
	m_limit(0), m_skip(0), m_sort_direction(0) { }

    QueryResult<T> result() const {
      return m_session->execute_query(m_collection, query(), m_limit, m_skip,
				      m_mapper);
    }

    T first() const {
      return limit(1).result().first();
    }

    T one() const {
      return limit(1).result().one();
    }

    std::vector<T> all() const {
      return result().all();
    }

    void remove_all() const {
      m_session->remove_all(m_collection, query());
    }

    void remove_one() const {
      m_session->remove_one(m_collection, query());
    }

    Query filter(Filter const& by) const {
      return Query(m_session, m_collection, m_mapper,
		   (m_filters, by), m_limit, m_skip,
		   m_sort_by, m_sort_direction);
    }

    void update(Update const& update) const {
      return m_session->execute_update(m_collection, m_filters.to_bson(), update.to_bson());
    }

    void update(T const& t) const {
      return update(Update("$set", remove_id(m_mapper->to_bson(t))));
    }

    Query skip(unsigned int N) const {
      return Query(m_session, m_collection, m_mapper, m_filters, m_limit, N,
		   m_sort_by, m_sort_direction);
    }

    Query limit(unsigned int N) const {
      return Query(m_session, m_collection, m_mapper, m_filters, N, m_skip,
		   m_sort_by, m_sort_direction);
    }

    template <typename U>
    Query ascending(U T::*field) const {
      return Query(m_session, m_collection, m_mapper, m_filters, m_limit,
		   m_skip, m_mapper->lookup_field(field), 1);
    }

    template <typename U>
    Query descending(U T::*field) const {
      return Query(m_session, m_collection, m_mapper, m_filters, m_limit,
		   m_skip, m_mapper->lookup_field(field), -1);
    }

    template <typename U>
    Query ascending(U (T::*getter)()) const {
      return Query(m_session, m_collection, m_mapper, m_filters, m_limit,
		   m_skip, m_mapper->lookup_field(getter), 1);
    }

    template <typename U>
    Query descending(U (T::*getter)()) const {
      return Query(m_session, m_collection, m_mapper, m_filters, m_limit,
		   m_skip, m_mapper->lookup_field(getter), -1);
    }
		   

  private:
    Query(Session *session, std::string const& collection, Mapper<T> const* mapper,
	  Filter const& filters, unsigned int limit, unsigned int skip,
	  std::string sort_by, int sort_direction)
      : m_session(session), m_collection(collection), m_mapper(mapper),
	m_filters(filters), m_limit(limit), m_skip(skip),
	m_sort_by(sort_by), m_sort_direction(sort_direction) { }

    Session *m_session;
    std::string m_collection;
    Mapper<T> const* m_mapper;
    Filter m_filters;
    unsigned int m_limit;
    unsigned int m_skip;
    std::string m_sort_by;
    int m_sort_direction;

    mongo::Query query() const {
      mongo::Query query(m_filters.to_bson());
      if (m_sort_direction != 0) {
	query.sort(m_sort_by, m_sort_direction);
      }
      return query;
    }

    mongo::BSONObj remove_id(mongo::BSONObj const& base) const {
      mongo::BSONObjBuilder builder;
      mongo::BSONObj filter = m_filters.to_bson();
      std::set<std::string> fields;
      base.getFieldNames(fields);
      for (std::set<std::string>::const_iterator i = fields.begin(); i != fields.end(); ++i) {
	if (*i == "_id") continue;
	builder.append(base[*i]);
      }
      return builder.obj();
    }

  };

};

#endif

