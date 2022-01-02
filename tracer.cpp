#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// fopen
#include <stdio.h>

// the DLL loader helper
#include <dlfcn.h>

// read/write/connect
#include <unistd.h>

// inet_ntoa
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// Stats
#include <map>
#include <string>
#include <iostream>

// Keeps stats
static std::map<int, ssize_t> sent;
static std::map<int, ssize_t> received;
static std::map<int, std::string> ips;

#ifdef __cplusplus
extern "C" {
#endif

/// Hook fopen to track open files.
/// Doesn't track anything yet.
FILE *fopen(const char *path, const char *mode) {
    FILE *(*original_fopen)(const char*, const char*);
    original_fopen = (FILE* (*)(const char*, const char*))dlsym(RTLD_NEXT, "fopen");
    return (*original_fopen)(path, mode);
}

/// Keep track of bytes written to sockets (or just file descriptors).
ssize_t write(int fd, const void *buf, size_t count) {
	// Only track bytes written for a socket created with connect()
	if (sent.find(fd) != sent.end()) {
		sent[fd] += count;
	}

	ssize_t (*original_write)(int, const void*, size_t);
	original_write = (ssize_t (*)(int, const void*, size_t))dlsym(RTLD_NEXT, "write");
	return (*original_write)(fd, buf, count);
}

/// TCP connect() hook.
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	// Get a human-readable IP.
	struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
	std::string ip(inet_ntoa(addr_in->sin_addr));

	// Initialize stats.
	ips[sockfd] = ip;
	sent[sockfd] = 0;
	received[sockfd] = 0;


	int (*original_connect)(int, const struct sockaddr*, socklen_t);
	original_connect = (int (*)(int, const sockaddr*, socklen_t))dlsym(RTLD_NEXT, "connect");

	return (*original_connect)(sockfd, addr, addrlen);
}

/// Keep track of bytes read from sockets (or just file descriptors).
ssize_t read(int fd, void *buf, size_t count) {
	ssize_t (*original_read)(int, void*, size_t);
	original_read = (ssize_t (*)(int, void*, size_t))dlsym(RTLD_NEXT, "read");

	ssize_t bytes_read = (*original_read)(fd, buf, count);

	// Only track bytes read for sockets created with connect()
	if (received.find(fd) != received.end()) {
		received[fd] += bytes_read;
	}

	return bytes_read;
}

/// Show stats for socket on close.
/// They can be uploaded to a tracker here (maybe with UDP if it's fast enough).
int close(int fd) {
	int (*original_close)(int);
	original_close = (int (*)(int))dlsym(RTLD_NEXT, "close");

	if (ips.find(fd) != ips.end() && sent.find(fd) != sent.end() && received.find(fd) != received.end()) {
		std::cout << std::endl;
		std::cout << "=== Tracer stats ===" << std::endl;
		std::cout << "IP " << ips[fd] << " received a total of " << sent[fd] << " bytes." << std::endl;
		std::cout << "IP " << ips[fd] << " sent a total of " << received[fd] << " bytes." << std::endl;
		ips.erase(ips.find(fd));
		sent.erase(sent.find(fd));
		received.erase(received.find(fd));
	}

	return (*original_close)(fd);
}

#ifdef __cplusplus
}
#endif