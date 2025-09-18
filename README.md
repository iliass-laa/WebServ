# WebServ (42 Project)

## Overview

**WebServ** is a minimalist HTTP server developed as part of the 42 school curriculum. The goal of this project is to help students understand the inner workings of web servers by building one from scratch in C++98, without relying on high-level libraries. It is an educational project focusing on low-level network programming, event-driven server architecture, and the HTTP protocol.

## Key Concepts

- **Socket Programming:** Learn to create, bind, listen, and manage sockets for network communication.
- **Event-driven Design:** Handle multiple simultaneous client connections efficiently using system calls like `poll` or `select`.
- **Non-blocking I/O:** Implement non-blocking behavior to ensure the server can scale and respond to many clients at once.
- **HTTP Protocol:** Parse and respond to HTTP requests, laying the foundation for understanding web communication.
- **Resource Management:** Gain experience managing memory, file descriptors, and system resources in C++.

## Learning Objectives

- Grasp how real-world web servers accept and serve multiple client requests.
- Understand the flow of an HTTP request and response at a low level.
- Explore event loops and multiplexing techniques for scalable network applications.
- Improve C++ skills, especially with respect to resource management and object-oriented design.

## Typical Features (to be implemented by students)

- Accept multiple simultaneous client connections.
- Parse HTTP requests and generate basic HTTP responses.
- Support for different HTTP methods (GET, POST, DELETE, etc.).
- Configurable server behavior (ports, error pages, routes, etc.).
- (Optional) Support for CGI scripts and dynamic content.

## Getting Started

To use this project:
1. Clone the repository.
2. Build using `make` (or your preferred C++98 build process).
3. Run the resulting server executable.
4. Connect to the server using a browser or tools like `curl` to test HTTP requests.

## Why This Project?

Building a web server from the ground up teaches the essential elements of modern networking and backend development. It demystifies what happens when you enter a URL in your browser and provides valuable experience in writing robust, efficient, and maintainable system software.

## Credits

Project created as part of the 42 school curriculum by students for educational purposes.
