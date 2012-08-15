#ifndef ums_MessageInfo_hpp
#define ums_MessageInfo_hpp

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/algorithm/string.hpp>
#include "messages.h"
#include "ILink.hpp"
#include "UMS_DLL.h"

namespace ums {

/**
 * Meta data for message.
 * Reception and translation.
 */
class UMS_API MessageInfo
{
public:
	typedef std::vector<uint8_t> buffer_t;

	virtual ~MessageInfo() {}

	const std::string &name() const { return name_; }
	std::string lowerName() const { return boost::to_lower_copy(name_); }
	uint8_t id() const { return id_; }
	size_t size() const { return size_; }

	/**
	 * Translate a binary message into a human readable string.
	 */
	virtual std::string translate(const buffer_t &b) const = 0;

	static const MessageInfo *findByName(const std::string &name);
	static const MessageInfo *findById(uint8_t id);
	static buffer_t receiveMessage(ILink *);
	static std::string toString(const buffer_t &b);

protected:
	std::string name_;
	uint8_t id_;
	size_t size_;

	struct Name {};
	struct Id {};

	typedef boost::multi_index_container <class MessageInfo*,
		boost::multi_index::indexed_by<
		boost::multi_index::hashed_unique <boost::multi_index::tag<Id>, BOOST_MULTI_INDEX_CONST_MEM_FUN(MessageInfo, uint8_t, id) >,
		boost::multi_index::hashed_unique < boost::multi_index::tag<Name>, BOOST_MULTI_INDEX_CONST_MEM_FUN(MessageInfo, std::string, lowerName) >
		>
	> MessageInfo_set;

	void addToRegistry();

private:
	static MessageInfo_set &registry();
};

}

#endif
