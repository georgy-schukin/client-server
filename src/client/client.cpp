#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

std::string read_message(tcp::socket &socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    auto message = std::string(boost::asio::buffers_begin(buf.data()),
                               boost::asio::buffers_begin(buf.data()) + buf.size());
    message.erase(message.find_last_not_of(" \n\r\t") + 1);
    return message;
}

void write_message(tcp::socket &socket, const std::string &message) {
    boost::asio::write(socket, boost::asio::buffer(message + "\n"));
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl;
        return -1;
    }

    try {
        const auto host = std::string(argv[1]);
        const auto port = (argc > 2 ? std::stoi(argv[2]) : 1234);

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);

        auto endpoints = resolver.resolve(host, std::to_string(port));

        std::cout << "Connecting to " << host << ":" << port << "..." << std::endl;

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        std::cout << "Connected" << std::endl;
        std::cout << "Input 'exit' to exit" << std::endl;

        while (true) {
            std::cout << "Input message: ";
            std::string message;
            std::getline(std::cin, message);
            write_message(socket, message);
            if (message == "exit") {
                break;
            }
        }

        std::cout << "Exiting..." << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
