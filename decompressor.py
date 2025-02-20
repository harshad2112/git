import struct

def read_git_index(index_file):
    try:
        with open(index_file, 'rb') as f:
            # Read the header (12 bytes)
            header = f.read(12)
            signature, version, num_entries = struct.unpack('>4sII', header)

            if signature != b'DIRC':
                raise ValueError("Invalid Git index file signature")

            print(f"Signature: {signature.decode('ascii')}")
            print(f"Version: {version}")
            print(f"Number of entries: {num_entries}")

            # Read the entries
            entries = []
            for _ in range(num_entries):
                # Each entry is at least 62 bytes + variable-length file path
                entry_data = f.read(62)
                ctime_s, ctime_ns, mtime_s, mtime_ns, dev, ino, mode, uid, gid, size, sha1, flags = struct.unpack(
                    '>IIIIIIIIII20sH', entry_data
                )

                # Read the variable-length file path (null-terminated)
                path = b""
                while True:
                    byte = f.read(1)
                    if byte == b'\x00':
                        break
                    path += byte

                # Align to the next multiple of 8
                padding = (8 - (len(entry_data) + len(path) + 1) % 8) % 8
                f.read(padding)

                entries.append({
                    "ctime": (ctime_s, ctime_ns),
                    "mtime": (mtime_s, mtime_ns),
                    "dev": dev,
                    "ino": ino,
                    "mode": mode,
                    "uid": uid,
                    "gid": gid,
                    "size": size,
                    "sha1": sha1.hex(),
                    "flags": flags,
                    "path": path.decode('utf-8')
                })

            return entries
    except Exception as e:
        print(f"An error occurred: {e}")
        return None


if __name__ == "__main__":
    index_file = ".git/index"
    entries = read_git_index(index_file)
    if entries:
        for entry in entries:
            print(entry)

