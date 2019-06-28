#include <iostream>
#include <memory>
#include <thread>
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

class Connection : public std::enable_shared_from_this<Connection> {
public:
    Connection(boost::asio::io_context &io_context) :
        _socket(io_context) {
    }

    ~Connection() {
        std::cout << "Connection is closed" << std::endl;
    }

    tcp::socket& socket() {
        return _socket;
    }

    void start() {
        auto ptr = shared_from_this();
        // Start receiving loop in a separate thread.
        // Capture shared pointer to itself in the lambda to prevent own deletion.
        std::thread thread([ptr]() {
            try {
                while (true) {
                    const auto msg = read_message(ptr->socket());
                    std::cout << "Received: " << msg << std::endl;
                    if (msg == "exit") {
                        break;
                    }
                }
            }
            catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
        });
        thread.detach();
    }

private:
    tcp::socket _socket;
};

class Server  {
public:
    Server(boost::asio::io_context& io_context, unsigned short port) :
        _acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    }

    void startAccept() {
        auto connection = std::make_shared<Connection>(_acceptor.get_executor().context());

        _acceptor.async_accept(connection->socket(), [this, connection](const boost::system::error_code &error) {
            this->handleAccept(connection, error);
        });
    }

private:
    void handleAccept(std::shared_ptr<Connection> new_connection, const boost::system::error_code &error) {
        if (!error) {
            std::cout << "Got connection" << std::endl;
            new_connection->start();
        }
        startAccept();
    }

private:
    tcp::acceptor _acceptor;
};

int main(int argc, char **argv) {
    try {
        const auto port = (argc > 1 ? std::stoi(argv[1]) : 1234);

        boost::asio::io_context io_context;

        Server server(io_context, static_cast<unsigned short>(port));

        std::cout << "Server is listening on " << port << std::endl;

        server.startAccept();
        io_context.run();
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
