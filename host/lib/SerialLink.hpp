#ifndef ums_SerialLink_hpp
#define ums_SerialLink_hpp

#include "ILink.hpp"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>

namespace ums {

class SerialLink : public ILink
{
public:
	SerialLink(const std::string &portName);
	virtual ~SerialLink();

	virtual void write(std::vector<uint8_t> &bytes);
	virtual void write(void *bytes, size_t nBytes);
	virtual std::deque<uint8_t> read();
	virtual boost::optional<uint8_t> readByte();

	bool open() const { return open_; }
	const boost::system::error_code &error();

private:
	typedef boost::function<void(const boost::system::error_code &ec, size_t count)> asio_handler_type;
	static const int READ_BUFFER_SIZE = 1024;

	std::string portName_;
	boost::asio::io_service ioSvc_;
	boost::asio::serial_port port_;
	bool open_;
	boost::system::error_code error_;
	boost::thread background_;
	std::deque<uint8_t> bytesRead_;
	std::deque<uint8_t> writeQueue_;
	boost::mutex mutex_;
	boost::array<uint8_t, READ_BUFFER_SIZE> readBuff_;
	std::vector<uint8_t> writeBuff_;

	void writeImpl();
	void writeHandler(const boost::system::error_code &ec, size_t count);
	void readHandler(const boost::system::error_code &ec, size_t count);
	void initiateRead();
	void error(const boost::system::error_code &val);
	void close();
	void closeHandler();

};

}

#endif
