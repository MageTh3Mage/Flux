#pragma once
#include <zmq.hpp>

inline bool enableWebSocketAim = true;

inline zmq::context_t context(1);
inline zmq::socket_t web_socket(context, ZMQ_PUSH);

void SendAim(const std::string message);

void ConnectAim(const char* address);

void SendOnScreen(const std::string message);

void ConnectOnScreen(const char* address);