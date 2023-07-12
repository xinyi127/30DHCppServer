server: src/util.cpp server.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp client.cpp src/Channel.cpp src/Server.cpp src/EventLoop.cpp \
	src/Acceptor.cpp src/Connection.cpp src/Buffer.cpp
	g++ src/util.cpp server.cpp src/Epoll.cpp src/InetAddress.cpp src/Socket.cpp src/Channel.cpp src/Server.cpp src/EventLoop.cpp \
		src/Buffer.cpp src/Acceptor.cpp src/Connection.cpp -o server
	g++ src/Buffer.cpp src/InetAddress.cpp src/Socket.cpp src/util.cpp client.cpp -o client
clean:
	rm server && rm client
