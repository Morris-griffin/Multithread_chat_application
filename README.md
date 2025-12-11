Multithreaded Chat Application
README
Authors

Morris Griffin – 02616740

Louis Canning – 02572277

Overview

This project implements a multithreaded client–server chat application using UDP sockets, POSIX threads, custom synchronization, and extended features such as history replay and inactive-client removal. The application also includes an ncurses-based user interface.

Core Implementations
Client-Side Implementation

UDP socket binding (unique ports + admin port 6666)

Threading model with sender and listener threads

All 8 request types implemented (conn, say, sayto, mute, unmute, rename, disconn, kick)

Session key authentication

ncurses-based UI with split chat/input windows

Server-Side Implementation

Server binds to port 12000

Listener thread dispatches requests to handler threads

Linked list storing IP, port, username, timestamps

All 8 request types processed in response_thread()

Thread-per-request model using pthread_create and pthread_detach

Synchronization

Custom reader–writer locks using semaphores

Writer priority mechanism to prevent starvation

Read locks for lookups/broadcasts

Write locks for data modification

Separate locks for client list, history, and heaps

Proposed Extensions
PE 1: Message History at Connection

Circular buffer of last 15 messages

Thread-safe history locking

Sent to new clients upon connection

Updated on broadcasts and announcements

PE 2: Remove Inactive Clients

Min-heap tracking clients by last active time

Ping after 30 seconds of inactivity

Clients moved to “pong heap” awaiting ret-ping

Auto-removal after 5 seconds without response

Heap operations include add_to_heap, move_down, remove_from_heap

Further Enhancements

Reliable communication layer using acknowledgments.

Known Issues

Communication over LAN was problematic due to WSL IP address differences. WSL’s assigned IP does not match the device’s actual network interface, making local-network testing impractical.
