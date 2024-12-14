#include<boost/asio.hpp>
#include<nlohmann/json.hpp>
#include<iostream>
#include<sstream>
#include<string>

using boost::asio::ip::tcp;
using json = nlohmann::json;

std::string response_format(const json& response_json) {
	std::string body = response_json.dump();
	std::string response = "HTTP/1.1 200 OK\r\n" "Content-Type: Application/json\r\n"  
		"Content-Length: " + std::to_string(body.size()) + "\r\n\r\n" + body;

	return response;
}

void get_request(tcp::socket& socket, const json& response_json) {
	try {
		std::string response = response_format(response_json);
		boost::asio::write(socket, boost::asio::buffer(response));
	} catch (const std::exception& e) {
		std::cerr << "Error + " << e.what() << std::endl;
	}
}

void post_request(tcp::socket& socket, const std::string& request) {
	size_t start_json = request.find("\r\n\r\n");
	try {
		if(start_json != std::string::npos) {
			std::string response = request.substr(start_json + 4);
			json body = json::parse(response);
			std::string whole_response = response_format(body);
			boost::asio::write(socket, boost::asio::buffer(whole_response));
		} else {
			std::cerr << "Error: " << std::endl;
		}
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
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

			std::istringstream request_identifier(request);
			std::string method;
			request_identifier >> method;

			if(method == "GET") {
				std::cout << "GET Request: " << request << std::endl;
				get_request(socket, reponse);	
			}
		
			if(method == "POST") {
				std::cout << "POST request: " << request << std::endl;
				post_request(socket, request);
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