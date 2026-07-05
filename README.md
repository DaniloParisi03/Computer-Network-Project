# 🎮 Multithreaded TCP Trivia Game

> **Bachelor's Degree Project** — *Computer Networks Course (Reti Informatiche / Reti di Calcolatori)*

![Language](https://img.shields.io/badge/Language-C-blue.svg)
![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20POSIX-lightgrey.svg)
![Protocol](https://img.shields.io/badge/Protocol-TCP%2FIP-green.svg)
![Concurrency](https://img.shields.io/badge/Concurrency-POSIX%20Threads%20(pthreads)-orange.svg)

---

## 📖 Overview

This repository contains the implementation of a **Multithreaded Client-Server Trivia Quiz Game** written in **C** using **POSIX Sockets** and **POSIX Threads (`pthreads`)**. 

The project demonstrates fundamental networking and operating system concepts, including TCP/IP socket communication, concurrent client handling via thread pooling/spawning, dynamic memory allocation with linked lists, and thread synchronization using mutexes.

---

## ✨ Key Features

- **🌐 TCP Client-Server Architecture**: Reliable connection-oriented communication over TCP (default port `3456`).
- **🔀 Concurrent Multi-Client Handling**: Spawns a dedicated POSIX thread (`pthread_create`) for each incoming client connection, allowing multiple players to connect and play simultaneously without blocking the main server loop.
- **📂 Dynamic File-Based Trivia Loading**: Automatically loads quiz categories, questions, and answers from text files into dynamically allocated singly linked lists at server startup.
- **🔒 Thread-Safe State Management**: Utilizes POSIX mutexes (`pthread_mutex_t`) to prevent race conditions when accessing shared resources, such as global player lists and score leaderboards.
- **🏆 Player & Score Tracking**: Comprehensive data structures to track player sessions, active/completed themes, and dynamic leaderboard rankings.

---

## 📁 Project Structure

```text
Computer-Network-Project/
├── bin/                 # Compiled object files (*.o)
├── const/
│   └── const.h          # Global constants, macros, and configuration paths
├── include/
│   ├── all.h            # Standard library and POSIX networking headers
│   ├── client.h         # Client-specific definitions
│   ├── serverlib.h      # Data structures and server library prototypes
│   └── serverlib.c      # Implementation of file loading, linked lists, and game logic
├── src/
│   ├── client.c         # Client application entry point
│   └── server.c         # Server application entry point and connection listener
├── txt/                 # Quiz database (categories, questions, and answers)
│   ├── temi.txt         # List of available trivia themes
│   ├── domande1.txt     # Questions for Theme 1
│   ├── risposte1.txt    # Answers for Theme 1
│   ├── domande2.txt     # Questions for Theme 2
│   └── risposte2.txt    # Answers for Theme 2
├── makefile             # Build automation script
└── README.md            # Project documentation
```

---

## ⚙️ Architecture & Data Structures

The application relies on several core data structures defined in `include/serverlib.h`:

### 1. `THEME` (struct theme)
Represents a trivia category.
- **`name`**: Title of the category (loaded from `temi.txt`).
- **`NUM_DOMANDE`**: Number of questions in the category.
- **`lista_domande` & `lista_risposte`**: Singly linked lists (`NODE*`) containing the questions and corresponding answers.
- **`mutex`**: Dedicated mutex to ensure thread-safe score updates and leaderboard access for the theme.

### 2. `PLAYER` (struct player)
Represents a connected user session.
- **`name`**: Player username (up to `NAME_MAX` characters).
- **`active`**: Boolean flag indicating if the player is currently connected.
- **`temi_iniziati` & `temi_finiti`**: Boolean tracking arrays for theme progress.
- **`next`**: Pointer to the next player in the global connected players list.

### 3. `NODE` (struct node)
A dynamic linked list node used for storing variable-length quiz content in memory without hardcoded array limits.

---

## 🚀 Getting Started

### Prerequisites
To compile and run this project, you need a POSIX-compatible environment (Linux, macOS, or Windows via WSL/MSYS2) with:
- **GCC** (GNU Compiler Collection)
- **Make**
- **POSIX Threads & Sockets support**

### 🔨 Compilation

A `makefile` is provided for easy build automation. Open your terminal in the project root directory and run:

```bash
# Compile both server and client executables
make

# Or compile individually:
make server
make client
```

To clean up compiled binaries and object files:

```bash
make cls
```

---

## 🕹️ Running the Application

### 1. Start the Server
Start the server first. It will bind to `127.0.0.1:3456`, load the trivia questions from the `txt/` directory into memory, and begin listening for client connections:

```bash
./server
```

*Expected output:*
```text
Server in ascolto
```

### 2. Start the Client(s)
In a separate terminal window (or multiple windows for concurrent players), launch the client:

```bash
./client
```

---

## 🛠️ Current Status & Roadmap

This project is currently in active development as part of a university coursework submission:
- [x] **Server Core**: Socket creation, TCP binding, listening, and accept loop.
- [x] **Concurrency**: Worker thread creation per connected client with mutex initialization.
- [x] **Data Persistence & Parser**: Robust text file parser stripping whitespace and loading questions/answers into linked lists.
- [ ] **Game Loop & Protocol**: Implementing full client-server message exchange inside `th_client`.
- [ ] **Client CLI**: Interactive terminal user interface for selecting themes and answering questions.
- [ ] **Leaderboard System**: Finalizing score calculation and ranking algorithms (`ord` & `aggiornaPunteggio`).

---

## 👤 Author

**Danilo Parisi**
- GitHub: [@DaniloParisi03](https://github.com/DaniloParisi03)
