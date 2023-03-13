#include "http/server.h"

#include <netinet/in.h>

namespace http {

void Server::Stop() {
  stopped = true;
}

void Server::Run() {
  // Creating the server socket
  server_sd_ = socket(AF_INET, SOCK_STREAM, 0);
  if(server_sd_ < 0) {
    throw std::runtime_error("Error establishing the server socket");
  }

  // Binding server socket to intended address
  int bind_status = bind(server_sd_, (struct sockaddr*) &server_addr_, sizeof(server_addr_));
  if (bind_status < 0) {
    throw std::runtime_error("Error binding the server socket");
  }

  LOG(INFO) << "Server starts listening on port " << opts_.port;
  while (!stopped) {
    listen(server_sd_, opts_.backlog);

    // Received a new connection
    sockaddr_in conn_socket_addr;
    socklen_t conn_socket_addr_size = sizeof(conn_socket_addr);

    // Accept the connection
    int conn_sd = accept(server_sd_, (sockaddr *)&conn_socket_addr, &conn_socket_addr_size);
    if (conn_sd < 0) {
      throw std::runtime_error("Error accepting request from client");
    }

    // Handle the connection
    Handle(conn_sd);
  }
}

void Server::Initialize() {
  bzero((char*)&server_addr_, sizeof(server_addr_));
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr_.sin_port = htons(opts_.port);
}

} // namespace http
