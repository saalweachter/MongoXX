/* filter.hh
   This is really about filters, actually.

*/

#ifndef MONGOXX_FILTER_HH
#define MONGOXX_FILTER_HH

#include "mongo/client/dbclient.h"

namespace mongoxx {

  class Filter {
  public:

    Filter() { }
    Filter(mongo::BSONObj const& filter) : m_filter(filter) { }

    Filter(Filter const& a, Filter const& b) {
      std::set<std::string> fields;

      a.m_filter.getFieldNames(fields);
      b.m_filter.getFieldNames(fields);

      mongo::BSONObjBuilder builder;
      for (std::set<std::string>::const_iterator i = fields.begin(); i != fields.end(); ++i) {
	if (a.m_filter.hasField(i->c_str()) and b.m_filter.hasField(i->c_str())) {
	  // In both!
	  mongo::BSONElement a_elem = a.m_filter.getField(*i);
	  mongo::BSONElement b_elem = b.m_filter.getField(*i);
	  if (a_elem.type() != mongo::Object or a_elem.type() != mongo::Object)
	    throw invalid_argument("Multiple equality filters applied to the same field.");
	  mongo::BSONObjBuilder builder2;
	  builder2.appendElements(a_elem.Obj());
	  builder2.appendElements(b_elem.Obj());
	  builder.append(i->c_str(), builder2.obj());
	} else if (a.m_filter.hasField(i->c_str())) {
	  // In a.
	  builder.append(a.m_filter.getField(*i));
	} else {
	  // In b.
	  builder.append(b.m_filter.getField(*i));
	}
      }
      m_filter = builder.obj();
    }

    mongo::BSONObj to_bson() const {
      return m_filter;
    }

  private:
    mongo::BSONObj m_filter;
  };

  inline Filter operator , (Filter const& a, Filter const& b) {
    return Filter(a, b);
  }


};

#endif

