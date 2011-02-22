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
	m_limit(0), m_skip(0) { }

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

    Query filter(Filter<T> const& by) const {
      return Query(m_session, m_collection, m_mapper,
		   merge(m_filters, by.apply(m_mapper)), m_limit, m_skip);
    }

    void update(Update<T> const& update) const {
      return m_session->execute_update(m_collection, m_filters, update.apply(m_mapper));
    }

    Query skip(unsigned int N) const {
      return Query(m_session, m_collection, m_mapper, m_filters, m_limit, N);
    }

    Query limit(unsigned int N) const {
      return Query(m_session, m_collection, m_mapper, m_filters, N, m_skip);
    }

  private:
    Query(Session *session, std::string const& collection, Mapper<T> const* mapper,
	  mongo::BSONObj const& filters, unsigned int limit, unsigned int skip)
      : m_session(session), m_collection(collection), m_mapper(mapper),
	m_filters(filters), m_limit(limit), m_skip(skip) { }

    Session *m_session;
    std::string m_collection;
    Mapper<T> const* m_mapper;
    mongo::BSONObj m_filters;
    unsigned int m_limit;
    unsigned int m_skip;

    mongo::Query query() const {
      mongo::Query query(m_filters);
      return query;
    }

    static mongo::BSONObj merge(mongo::BSONObj const& a, mongo::BSONObj const& b) {
      std::set<std::string> fields;

      a.getFieldNames(fields);
      b.getFieldNames(fields);

      mongo::BSONObjBuilder builder;
      for (std::set<std::string>::const_iterator i = fields.begin(); i != fields.end(); ++i) {
	if (a.hasField(i->c_str()) and b.hasField(i->c_str())) {
	  // In both!
	  mongo::BSONElement a_elem = a.getField(*i);
	  mongo::BSONElement b_elem = b.getField(*i);
	  if (a_elem.type() != mongo::Object or a_elem.type() != mongo::Object)
	    throw invalid_argument("Multiple equality filters applied to the same field.");
	  mongo::BSONObjBuilder builder2;
	  builder2.appendElements(a_elem.Obj());
	  builder2.appendElements(b_elem.Obj());
	  builder.append(i->c_str(), builder2.obj());
	} else if (a.hasField(i->c_str())) {
	  // In a.
	  builder.append(a.getField(*i));
	} else {
	  // In b.
	  builder.append(b.getField(*i));
	}
      }
      return builder.obj();
    }

  };

};

#endif

