/* update.hh
   How we specify updates.

   Jack Saalweachter
*/

#ifndef MONGOXX_UPDATE_HH
#define MONGOXX_UPDATE_HH

#include "mongo/client/dbclient.h"

namespace mongoxx {

  /**
   * An Update hold the fields of a document to be updated.
   */
  class Update {
  public:

    /**
     * Basic constructor, the parameters and how they should be changed.
     * @param operation the operation to perform: $set, $inc
     * @param parameters the fields of the document to update
     */
    Update(std::string const& operation, mongo::BSONObj parameters) {
      m_updates[operation] = parameters;
    }

    /**
     * Composition constructor.  Takes two updates, combines them.
     * @param a first update
     * @param b second update
     */
    Update(Update const& a, Update const& b) : m_updates(a.m_updates) {
      for (std::map<std::string, mongo::BSONObj>::const_iterator i = b.m_updates.begin(); i != b.m_updates.end(); ++i) {
	if (m_updates.find(i->first) == m_updates.end()) {
	  m_updates[i->first] = i->second;
	} else {
	  mongo::BSONObjBuilder builder;
	  builder.appendElements(m_updates[i->first]);
	  builder.appendElements(i->second);
	  m_updates[i->first] = builder.obj();
	}
      }
    }

    /**
     * Generate a BSON object in the format expected by the update function.
     * @return a BSONObj encoding the update
     */
    mongo::BSONObj to_bson() const {
      mongo::BSONObjBuilder builder;
      for (std::map<std::string, mongo::BSONObj>::const_iterator i = m_updates.begin(); i != m_updates.end(); ++i) {
	builder.append(i->first, i->second);
      }
      return builder.obj();
    }

  private:
    std::map<std::string, mongo::BSONObj> m_updates;
  };

  /**
   * Compose an Update out of two other Updates.
   * @param a first update
   * @param b second update
   * @return an Update containing both sets of Updates
   */
  inline Update operator , (Update const& a, Update const& b) {
    return Update(a, b);
  }

};

#endif

