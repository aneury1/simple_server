#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <string>
#include <memory>

using namespace boost::asio;
using ip::tcp;

class SMTP_Session : public std::enable_shared_from_this<SMTP_Session> {
public:
    SMTP_Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        std::cout << "Client connected.\n";
        send_welcome();
    }

private:
    tcp::socket socket_;
    char data_[4096] = {0};

    void send_welcome() {
        auto self(shared_from_this());
        std::string welcome_message = "220 Simple SMTP Server Ready\r\n";
        async_write(socket_, buffer(welcome_message),
            [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    read_client();
                }
            });
    }

    void read_client() {
        auto self(shared_from_this());
        socket_.async_read_some(buffer(data_, 4096),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string client_request(data_, length);
                    std::cout << "Client said: " << client_request << "\n";

                    if (client_request.find("HELO") != std::string::npos||client_request.find("helo") != std::string::npos) {
                        send_response("250 Hello, pleased to meet you\r\n");
                    }
                    else if (client_request.find("TO:") != std::string::npos||client_request.find("to:") != std::string::npos) {
                        send_response("250 OK\r\n");
                    } 
                    else if (client_request.find("ehlo") != std::string::npos||client_request.find("EHLO") != std::string::npos) {
                        send_response("250 Hello, pleased to meet you\r\n");
                    } 
                    else if (client_request.find("QUIT") != std::string::npos||client_request.find("rset") != std::string::npos) {
                        send_response("221 Bye\r\n", true);
                    } 
                     else if (client_request.find("FROM") != std::string::npos) {
                        std::string response = "250 OK <" + std::string("<from@example.com>") + ">, Sender accepted\r\n";
                        send_response("250 OK\r\n");
                    } 
                    else if (client_request.find("RCPT") != std::string::npos||client_request.find("rcpt") != std::string::npos) {
                        std::string response = "250 OK <" + std::string("<to@example.com>") + ">, Recipient accepted\r\n";
                        send_response("250 OK\r\n");
                    } 
                    else if (client_request.find("DATA") != std::string::npos||client_request.find("data") != std::string::npos) {
                        std::string response = "354 Start mail input;\r\n\r\n";
                        send_response(response);
                    }
                    else {
                        read_client(); // Continue listening for more data
                    }
                }
            });
    }

    void send_response(const std::string& message, bool close_after = false) {
        auto self(shared_from_this());
        async_write(socket_, buffer(message),
            [this, self, close_after](boost::system::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    if (close_after) {
                        socket_.close();
                        std::cout << "Client disconnected.\n";
                    } else {
                        read_client();
                    }
                }
            });
    }
};

class SMTP_Server {
public:
    SMTP_Server(io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
    }

private:
    tcp::acceptor acceptor_;

    void start_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<SMTP_Session>(std::move(socket))->start();
                }
                start_accept(); // Accept the next connection
            });
    }
};

int main() {
    try {
        const int THREAD_POOL_SIZE = 4; // Number of threads in the pool
        io_context io_context;

        SMTP_Server server(io_context, 2525);
        std::cout << "SMTP Server started on port 2525\n";

        // Thread Pool
        std::vector<std::thread> thread_pool;
        for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
            thread_pool.emplace_back([&io_context]() { io_context.run(); });
        }

        for (auto& thread : thread_pool) {
            thread.join();
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
