#ifndef umc_CommandInfo_hpp
#define umc_CommandInfo_hpp

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/algorithm/string.hpp>
#include "commands.h"

namespace ums {

/**
 * Meta data for command.
 * Parsing and output generation.
 */
class CommandInfo
{
public:
	virtual ~CommandInfo() {}

	const std::string &name() const { return name_; }
	std::string lowerName() const { return boost::to_lower_copy(name_); }
	uint8_t id() const { return id_; }

	/**
	 * Compile a string version of the command into a binary version suitable for transmission to the device.
	 */
	virtual MaxCmdBuff compile(const std::string &input) const = 0;

	static const CommandInfo *findByName(std::string name);
	static const CommandInfo *findById(uint8_t id);

	/**
	 * Compile a single line into binary for transmission to the device.
	 * Takes a single line string.
	 * Ignores comments starting with '#'
	 * Finds the correct CommandInfo subclass by extracting the command name from
	 * line and searching the registry for it.
	 */
	static std::auto_ptr<MaxCmdBuff> parseLine(const std::string &line);

protected:
	std::string name_;
	uint8_t id_;

	struct Name {};
	struct Id {};

	typedef boost::multi_index_container <CommandInfo*,
		boost::multi_index::indexed_by<
		boost::multi_index::hashed_unique <boost::multi_index::tag<Id>, boost::multi_index::member <CommandInfo, uint8_t, &CommandInfo::id_> >,
		boost::multi_index::hashed_unique < boost::multi_index::tag<Name>, BOOST_MULTI_INDEX_CONST_MEM_FUN(CommandInfo, std::string, lowerName) >
		>
	> CommandInfo_set;

	template <typename T> void fillCmdBuff(MaxCmdBuff &buff, const T &cmd) const {
		uint8_t *pbuff = &buff.cmdId;
		std::fill(pbuff, pbuff+sizeof(MaxCmdBuff), 0);
		const uint8_t *pcmd = &cmd.cmdId;
		std::copy(pcmd, pcmd+sizeof(T), pbuff);

	}

	void addToRegistry();

private:
	static CommandInfo_set &registry();
};

}

#endif
