import pickle

class MyObject:
    def __init__(self, x, y, name):
        self.x = x
        self.y = y
        self.name = name

obj = MyObject(42, 3.14, "Hello, world!")

# Serialize the object to a byte stream
bytes = pickle.dumps(obj)

# Write the byte stream to a file
with open("data.pkl", "wb") as file:
    file.write(bytes)

# Read the byte stream from the file and deserialize it
with open("data.pkl", "rb") as file:
    bytes = file.read()
    new_obj = pickle.loads(bytes)

print(f"x: {new_obj.x}, y: {new_obj.y}, name: {new_obj.name}")
