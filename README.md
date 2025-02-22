# Build your own GIT
This is a CPP implementation of Git.

## Description

Git-cpp currently supports these commands:
 - Init
 - cat-file
 - hash-object
 - ls-tree
 - ls-files
 - write-tree

## Usage
- `./git init` -> This command will initialize a git repo.
  * create a `.git`, `.git/objects`, `.git/refs` folders.
  * Create a `.git/HEAD` file, to keep track of the head.
- `./git cat-file -p` ->
- `./git hash-object` -> compresses the file and creates an SHA1 of that compressed data, and then writes that data in a file with the same SHA hash in .git/objects folder.
- `./git ls-tree` -> shows all the file present in that commit id.
- `./git ls-files` -> 
- `./git write-tree` -> 
- `./git add .` -> This commmands adds all the modified/created files to index.
  
