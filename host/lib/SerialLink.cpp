#include "SerialLink.hpp"
#include <boost/function.hpp>

// finish implementation from http://gitorious.org/serial-port/serial-port/blobs/master/3_async/AsyncSerial.cpp

namespace asio = boost::asio;

namespace ums {

SerialLink::SerialLink(const std::string &portName) :
		portName_(portName), ioSvc_(), port_(ioSvc_), open_(false),
		error_(0, boost::system::system_category())
{
	writeBuff_.clear();
	port_.open(portName_);

	// hard code to 8N1, no flow control, 115200 bps
	port_.set_option(asio::serial_port_base::baud_rate(115200));

	// give the io service something to do so it doesn't stop immediately.
	ioSvc_.post(boost::bind(&SerialLink::initiateRead, this));

	// start background thread
	boost::thread t(boost::bind(&asio::io_service::run, &ioSvc_));
	background_.swap(t);
	open_ = true;
	error(boost::system::error_code());

}

SerialLink::~SerialLink()
{
	if (open()) {
		try {
			close();
		} catch(...) {}
	}
}

void
SerialLink::write(std::vector<uint8_t> &bytes)
{
	boost::lock_guard<boost::mutex> guard(mutex_);
	writeQueue_.insert(writeQueue_.end(), bytes.begin(), bytes.end());
	writeImpl();

}

void
SerialLink::write(void *bytes, size_t nBytes)
{
	uint8_t *pBytes = reinterpret_cast<uint8_t*>(bytes);
	boost::lock_guard<boost::mutex> guard(mutex_);
	writeQueue_.insert(writeQueue_.end(), pBytes, pBytes+nBytes);
	writeImpl();
}

void SerialLink::writeImpl() {
	// don't do anything else if a write operation is in progress
	if (writeBuff_.size() == 0) {
		writeBuff_.resize(writeQueue_.size());
		std::copy(writeQueue_.begin(), writeQueue_.end(), writeBuff_.begin());
		writeQueue_.clear();

		asio_handler_type handler = boost::bind(&SerialLink::writeHandler, this,
				asio::placeholders::error, asio::placeholders::bytes_transferred);
		asio::async_write(port_, asio::buffer(writeBuff_), handler);
	}
}


void
SerialLink::writeHandler(const boost::system::error_code &ec, size_t count)
{
	if (ec) {
		error(ec);
		if (open()) {
			closeHandler();
		}
	} else {
		boost::lock_guard<boost::mutex> guard(mutex_);
		if (writeQueue_.empty()) {
			writeBuff_.clear();
		} else {
			writeBuff_.resize(writeQueue_.size());
			std::copy(writeQueue_.begin(), writeQueue_.end(), writeBuff_.begin());
			writeQueue_.clear();
			asio_handler_type handler = boost::bind(&SerialLink::writeHandler, this,
					asio::placeholders::error, asio::placeholders::bytes_transferred);
			asio::async_write(port_, asio::buffer(writeBuff_), handler);
		}
	}
}

std::deque<uint8_t>
SerialLink::read()
{
	boost::lock_guard<boost::mutex> guard(mutex_);
	std::deque<uint8_t> ret;
	ret.swap(bytesRead_);
	return ret;
}

boost::optional<uint8_t>
SerialLink::readByte()
{
	boost::optional<uint8_t> ret;
	return ret;
}

void
SerialLink::initiateRead()
{
	asio_handler_type handler = boost::bind(&SerialLink::readHandler, this,
			asio::placeholders::error, asio::placeholders::bytes_transferred);
	port_.async_read_some(asio::buffer(readBuff_), handler);
}

void
SerialLink::readHandler(const boost::system::error_code &ec, size_t count)
{
	if (ec) {
		error(ec);
		if (open()) {
			closeHandler();
		}
	} else {
		boost::lock_guard<boost::mutex> guard(mutex_);
		for (size_t i=0; i<count; i++) {
			bytesRead_.push_back(readBuff_[i]);
		}
		initiateRead();
	}
}

void
SerialLink::closeHandler()
{
	boost::system::error_code ec;
	port_.cancel(ec);
	if (ec) error(ec);
	port_.close(ec);
	if (ec) error(ec);
}

const boost::system::error_code &
SerialLink::error()
{
	boost::lock_guard<boost::mutex> guard(mutex_);
	return error_;
}

void
SerialLink::error(const boost::system::error_code &val)
{
	boost::lock_guard<boost::mutex> guard(mutex_);
	error_ = val;
}

void
SerialLink::close()
{
	if (!open_) return;

	open_ = false;
	ioSvc_.post(boost::bind(&SerialLink::closeHandler, this));
	background_.join();
	ioSvc_.reset();
}


}
