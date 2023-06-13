import hashlib

chunk_size = 20 * 1024 * 1024  # 20 MB chunk size

# Calculate hash of the original file
original_hash = hashlib.md5()
with open('pi-billion.txt', 'rb') as original_file:
    while True:
        data = original_file.read(chunk_size)
        if not data:
            break
        original_hash.update(data)
original_file_hash = original_hash.hexdigest()

# Read the hash from the hash file
with open('pi-billion.md5', 'r') as hash_file:
    expected_hash = hash_file.read().strip()

# Compare the expected hash with the combined hash of chunk files
chunk_num = 1
combined_hash = hashlib.md5()
while True:
    try:
        with open(f'_py_billion___chunk_{chunk_num}.txt', 'rb') as chunk_file:
            chunk_data = chunk_file.read(chunk_size)
            if not chunk_data:
                break
            combined_hash.update(chunk_data)
    except FileNotFoundError:
        break
    chunk_num += 1

if combined_hash.hexdigest() == expected_hash:
    print("Hash of chunk files matches the hash in pi-billion.md5 file.")
else:
    print("Hash of chunk files does not match the hash in pi-billion.md5 file.")
