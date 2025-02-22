# Build your own GIT

This project is a minimal implementation of Git in C++, replicating core Git functionalities such as object storage, indexing, and tree management. The goal is to understand Git's internals by building a working model of its key commands.

## Features

The current implementation supports the following Git commands:

- `git init` – Initializes a new Git repository.
- `git cat-file -p <object>` – Displays the content of a Git object.
- `git hash-object <file>` – Computes the SHA-1 hash of a file.
- `git hash-object -w <file>` – Computes the SHA-1 hash and writes the object to the `.git/objects` directory.
- `git ls-tree <tree>` – Lists the contents of a tree object.
- `git ls-tree -r <tree>` – Recursively lists the contents of a tree object.
- `git ls-tree -l <tree>` – Lists tree contents with object size.
- `git ls-files` – Shows the tracked files in the index.
- `git write-tree` – Converts the index into a tree object.
- `git add .` – Adds files to the index.

## Installation

To build and run the project, ensure you have a C++ compiler installed (GCC/Clang/MSVC) and `CMake` for build management.

```sh
cmake --debug-output -B build -S .
cmake --build build
```

## Usage

Initialize a repository:

```sh
./git-cpp init
```

Add files to the index:

```sh
./git-cpp add .
```

Write the index to a tree:

```sh
./git-cpp write-tree
```

List files in the index:

```sh
./git-cpp ls-files
```

Inspect a tree:

```sh
./git-cpp ls-tree <Tree-hash>
```

Compute and store a file’s hash:

```sh
./git-cpp hash-object -w myfile.txt
```

Retrieve an object’s content:

```sh
./git-cpp cat-file -p <object-hash>
```

## Project Structure

```
.git-cpp/
├── src/           # Source code
├── include/       # Header files
├── build/         # Compiled binaries
├── .git/          # Git metadata when initialized
└── README.md      # Project documentation
```

## Roadmap

- Implement `git commit`
- Implement `git checkout`
- Optimize object storage
- Add unit tests

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests to improve the project.

