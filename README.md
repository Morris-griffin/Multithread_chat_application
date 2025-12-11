Multithreaded Chat Application

A fully concurrent client–server chat system implemented in C using UDP sockets, POSIX threads, custom reader/writer synchronization, and an ncurses-based UI.
The project also features extended functionality including message history replay, inactive-client removal, and a lightweight reliability layer.

🚀 Features
Client

UDP socket binding with unique ports

Admin port: 6666

Two-thread model:

client_speak (sending messages)

client_listen (receiving messages)

Implements all 8 request types:
conn, say, sayto, mute, unmute, rename, disconn, kick

Session key authentication (added security)

Ncurses interface with split chat + input panels

Server

Binds to port 12000

Listener thread dispatches every request to a dedicated worker thread

Maintains client list using a linked list (IP, port, username, timestamps)

All 8 request types handled in response_thread()

Thread-per-request model using pthread_create() + pthread_detach()

🔒 Synchronization System

Custom reader/writer lock implementation using semaphores:

Writer priority to prevent starvation

Read locks for:

Lookups

Broadcast operations

Write locks for:

Client list modifications

History updates

Heap operations

Separate lock types for:

Client list

Message history

Inactivity heaps

🧩 Project Extensions
PE 1 — Message History on Connection

Circular buffer storing the last 15 messages

Thread-safe access with history_lock() / history_unlock()

Automatically sent to new clients when they connect

Updated on:

Broadcast messages

Server announcements

PE 2 — Automatic Removal of Inactive Clients

Min-heap tracks clients by last_active_time

Server sends $ping$ after 30 seconds of silence

Clients moved to a "pong heap" while awaiting ret-ping$

Auto-removal after 5 seconds with no response

Heap operations implemented:

add_to_heap()

move_down()

remove_from_heap()

⚙️ Additional Enhancements

Lightweight reliability layer using ACKs
→ Helps compensate for UDP’s lack of reliability

🧠 Known Issues & Development Challenges

Due to the use of WSL, the virtual network interface created by WSL does not share the same IP as the host device.
This caused major issues when attempting LAN communication between devices, and TAs confirmed that WSL makes this setup impractical.

👥 Authors

Morris Griffin — 02616740

Louis Canning — 02572277
