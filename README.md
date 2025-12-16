# Unix Shell Implementation (Minishell)

![Language](https://img.shields.io/badge/Language-C-00599C?style=flat-square&logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Linux-FCC624?style=flat-square&logo=linux&logoColor=black)
![Score](https://img.shields.io/badge/Grade-9.4%2F10-success?style=flat-square)

## 📖 Overview
A fully functional **Unix Shell** developed in **C**, interacting directly with the Linux Kernel API.
This project was part of the Operating Systems course at **Universidade da Coruña (FIC)**, focusing on low-level system programming, memory management, and process concurrency.

The implementation includes a command parser, historical record of commands, and direct manipulation of system resources.

## 🏆 Project Performance
This project achieved a near-perfect score of **94%** (2.35 / 2.50) based on rigorous automated testing and code quality review.

| Module | Focus Area | Score | Status |
| :--- | :--- | :--- | :--- |
| **P1** | File System & Syscalls | **100%** (0.80/0.80) | ✅ Perfect |
| **P2** | Memory Management | **90%** (0.77/0.85) | ✅ Optimized |
| **P3** | Process & Concurrency | **92%** (0.78/0.85) | ✅ Optimized |
| **Total** | **Final Assessment** | **94%** (2.35/2.50) | **Distinction** |

> *Note: All minor bugs from the initial submission have been patched ("Errores corregidos").*

## ⚙️ Key Features

### 1. File System Operations (P1)
- Custom implementation of core commands using system calls (`open`, `read`, `write`, `stat`).
- Directory navigation and file inspection without relying on external binaries.

### 2. Memory Management (P2)
- **Manual Allocator:** CLI commands to allocate/deallocate memory blocks (`malloc`, `mmap`, `shared_mem`).
- **Memory Dump:** Tools to inspect memory content at specific addresses (Hex/ASCII view) to debug segmentation faults.
- Recursive functions to analyze stack growth and memory layout.

### 3. Process Management (P3)
- **Background Execution:** Full support for `&` operator to run processes in the background.
- **Priority Handling:** Management of process priority (`nice`) and environment variables.
- **Process List:** Dynamic linked list tracking active, stopped, and terminated processes (PIDs).
- **Signal Handling:** Custom handlers for IPC signals.

## 💻 Installation & Usage

1. **Clone the repository:**
   ```bash
   git clone [https://github.com/tiago-volta/SO-UDC.git](https://github.com/tiago-volta/SO-UDC.git)
   cd SO-UDC

2. **Compile:**
```bash
   make 
   
3. **Run the Shell:**
```bash
./shell   
