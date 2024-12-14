#include<boost/asio.hpp>
#include <nlohmann/json.hpp>
#include<iostream>
#include<string>

using boost::asio::ip::tcp;
using json = nlohmann::json;

void send_request(tcp::socket& socket,const json& response_json) {
	try {
		std::string body = response_json.dump();
		std::string response = 
		"HTTP/1.1 200 OK\r\n" "Content-Type: application/json\r\n"  
		"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;

		boost::asio::write(socket, boost::asio::buffer(response));
	} catch (const std::exception& e) {
		std::cerr << "Error + " << e.what() << std::endl;
	}
}

void request_handler(tcp::socket& socket) {
	try {
		char buffer[1024];
		std::string request;
		boost::system::error_code error;
		size_t length = socket.read_some(boost::asio::buffer(buffer), error);

		if(!error) {
			json reponse = {{"msg", "Hello world"}};
			request.append(buffer, length);
			std::cout << "Request recieved " << request << std::endl;
			send_request(socket, reponse);	
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