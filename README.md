# ShatterLock

## C-project for Semester-2


## Project Description

ShatterLock is a secure, local encryption system designed to protect user data on a computer. It uses fragmentation, multi-layered metadata encryption and distributed storage techniques to ensure encrypted files cannot be directly copied to a different system. 

Files once encrypted are split into packets, which are then scattered across the filesystem along with junk packets. The same process is done in reverse to decrypt and read files. The system supports multi-user access, allowing each user to maintain independently secured data spaces. The more users, the more secure the system. Hence, the security is based on both the encryption algorithm and its implementation.

The goal of the project is to provide users with a strong local encryption system which is secured using an encryption algorithm and an implementation that uses fragmentation and distributed storage. Designed to be used by users with minimal technical knowledge.

---

### Features

- The user has CRUD capabilities. I.e user can create, read, update/edit and delete files.
- The user is to enter their username and password while doing any of these actions.
- During signup, the user is to enter 2-3 Security questions to provide a more complex encryption system that implements TOR-like bouncing.

---

### Encryption and Workflow

- The files are encrypted using the username and password themselves. The locations of the files are also dependent on this. Decrypting files is impossible unless the username and password are correct. Therefore, there is no need for additional authentication.
- The files are encrypted based on a strong encryption algorithm. Then split up into many equally sized packets.
- These are then spread out into unique pseudo-random locations across the computer’s files.
    - The directories (or locations) are collected and n of them are picked in a pseudo-random method based on a key derived from the username and password. (n=number of packets)
    - Skipping the folders which has special permissions.
- When a user requests the data,
    - The username and password are used to generate a key, which will then be used to backtrack the location of the encrypted files.
    - The first layer (meta-data) is decrypted to ensure the right packets are decrypted and to order the packets in the right way, making sure they are all there.
    - The metadata is removed, and the rest of the contents are decrypted.
    - The user can then view the files. Similarly, other CRUD operations are defined.

---

## Tech Stack: C

---

### How to Run Current Stable Version

#### To run the current stable version: 
##### 1. Fork the repo
##### 2. Install C/C++ and CodeRunner extensions on your code editor. (optional but recommended)
https://marketplace.visualstudio.com/items?itemName=formulahendry.code-runner 
https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
##### 2. You can alternatively run 
```bash
gcc -E repo-hn.c
gcc -C repo-hn.c -lm
./a.out
```

---

## Maintainer in charge: *Suraj Acharya*, Github: *SA-10125*

---


## How to Contribute
### 1. Fork the repository on GitHub.
### 2. Clone your forked repository:
### 3. Create a new branch for your feature or fix:
### 4. Make your changes and commit them with clear, descriptive messages:
### 5. Push your changes to your fork:
### 6. Open a Pull Request (PR) from your branch to the main branch of the original repository, describing your changes in detail.

---

## Please refer to CONTRIBUTING.md

---

## This repository is open source and is protected under MIT License.

---
