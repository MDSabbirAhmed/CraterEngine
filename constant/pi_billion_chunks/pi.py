chunk_size = 20 * 1024 * 1024  # 20 MB chunk size

with open('pi-billion.txt', 'rb') as original_file:
    chunk_num = 1
    while True:
        chunk_data = original_file.read(chunk_size)
        if not chunk_data:
            break  # Exit the loop if the end of the file is reached
        with open(f'_py_billion___chunk_{chunk_num}.txt', 'wb') as chunk_file:
            chunk_file.write(chunk_data)
        chunk_num += 1