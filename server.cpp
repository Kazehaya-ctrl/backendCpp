#include<boost/asio.hpp>
#include<iostream>
#include<string>

using boost::asio::ip::tcp;

const std::string HTTP_RESPONSE = "HTTP/1.1 200 OK\r\n"
	"Content-Type: Application/json\r\n"
	"Content-Length: 13\r\n"
	"\r\n"
	"{msg: Hello, World!'}";
  
void request_handler(tcp::socket& socket) {
	try {
		char buffer[1024];
		std::string request;
		boost::system::error_code error;
		size_t length = socket.read_some(boost::asio::buffer(buffer), error);

		if(!error) {
			request.append(buffer, length);
			std::cout << "Request recieved " << request << std::endl;
			boost::asio::write(socket, boost::asio::buffer(HTTP_RESPONSE), error);
			if(error) {
				std::cerr << "Error: " << error << std::endl;
			}
		} else {
			std::cerr << "Request can't be read: " << std::endl;
		}
	} catch(const std::exception& e) {
		std::cerr << "Error + " << e.what() << std::endl;
	}
}

int main() {

	try {
		boost::asio::io_context io_context;
		tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

		std::cout << "HTTP connection on port 8080" << std::endl;
		
		while(true) {
			tcp::socket socket(io_context);
			acceptor.accept(socket);
			std::cout << "Connection established: " << std::endl;
			request_handler(socket);
			socket.close();
		}
	} catch (const std::exception& e) {
		std::cerr << "Error + " << e.what() << std::endl;
	}
	return 0;
}


