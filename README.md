# ShatterLock
C-project for semester-2

(will format the readme better later, pls forgive)

to run current stable version: stableversionfordemo.c
create storage folder where you are running, also create subfolders in that and leave them empty.
can alternatively just download the whole repo.

windows: create "storage" folder in the folder where you downloaded the stableversionfordemo.exe file in. then run that stableversionfordemo.exe in the terminal.


linux: download .c file, make "storage" folder in that directory and compile the stableversionfordemo.c using:
terminal:
  gcc -E stableversionfordemo.c
  gcc -C stableversionfordemo.c -lm
  ./a.out
  
**ShatterLock** is a secure, local encryption system designed to protect user data on a
computer. It uses fragmentation, multi-layered metadata encryption and distributed storage
techniques to ensure encrypted files cannot be directly copied to a different system. Files
once encrypted are split into packets, which are then scattered across the filesystem along
with junk packets. The same process is done in reverse to decrypt and read files. The system
supports multi-user access, allowing each user to maintain independently secured data
spaces. The more users, the more secure the system. Hence, the security is based on both
the encryption algorithm and its implementation

The goal of the project is to provide users with a strong local encryption system
which is secured using an encryption algorithm and an implementation that uses
fragmentation and distributed storage. Designed to be used by users with minimal technical
knowledge.

● The user has CRUD capabilities. I.e user can create, read, update/edit and delete
files.
● The user is to enter their username and password while doing any of these actions.
● During signup, the user is to enter 2-3 Security questions to provide a more complex
encryption system that implements TOR-like bouncing

The files are encrypted using the username and password themselves. The locations
of the files are also dependent on this. Decrypting files is impossible unless the
username and password are correct. Therefore, there is no need for additional
authentication.
● The files are encrypted based on a strong encryption algorithm. Then split up into
many equally sized packets.
● These are then spread out into unique pseudo-random locations across the
computer’s files.
a. The directories (or locations) are collected and n of them are picked in a
pseudo-random method based on a key derived from the username and
password. (n=number of packets)
b. Skipping the folders which has special permissions.
● When a user requests the data,
a. The username and password are used to generate a key, which will then be
used to backtrack the location of the encrypted files.
b. The first layer (meta-data) is decrypted to ensure the right packers are
decrypted and to order the packets in the right way, making sure they are all
there.
c. The metadata is removed, and the rest of the contents are decrypted.
d. The user can then view the files. Similarly, other CRUD operations are
defined.
