#include "Websocket.h"

void SendAim(const std::string message) {
	zmq::message_t zmq_message(message.size());
	memcpy(zmq_message.data(), message.data(), message.size());
	web_socket.send(zmq_message, zmq::send_flags::dontwait);
}

void ConnectAim(const char* address) {
	web_socket.connect(address);
}

void SendOnScreen(const std::string message) {
	zmq::message_t zmq_message(message.size());
	memcpy(zmq_message.data(), message.data(), message.size());
	web_socket.send(zmq_message, zmq::send_flags::dontwait);
}

void ConnectOnScreen(const char* address) {
	web_socket.connect(address);
}