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
    try {
        const auto port = (argc > 1 ? std::stoi(argv[1]) : 1234);

        boost::asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), static_cast<unsigned short>(port)));

        std::cout << "Server is listening on " << port << std::endl;

        for (;;) {

            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Got connection" << std::endl;

            while (true) {
                const auto msg = read_message(socket);
                std::cout << "Received: " << msg << std::endl;
                if (msg == "exit") {
                    break;
                }
            }

            std::cout << "Disconnecting" << std::endl;
        }

        std::cout << "Exiting..." << std::endl;
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
