#include "ClientHandler.hpp"
#include <iostream>
#include <csignal>
#include <thread>

ClientHandler* g_client = nullptr;

void signalHandler(int signum) {
    if (g_client) {
        g_client->stop();
    }
    std::cout << "\nClient shutdown gracefully" << std::endl;
    exit(signum);
}

void printUpdate(const std::string& filename, const nlohmann::json& content) {
    std::cout << "Update received for " << filename << ":\n"
              << content.dump(4) << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }

    const std::string socketPath = "/tmp/ipc_socket.sock";
    const std::string filename = argv[1];

    try {
        ClientHandler client(socketPath);
        g_client = &client;
        
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        client.subscribe(filename);
        client.startListening(printUpdate);

        std::cout << "Listening for updates on " << filename 
                 << ". Press Ctrl+C to exit..." << std::endl;
        
        while(true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
