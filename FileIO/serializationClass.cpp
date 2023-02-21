#include <iostream>
#include <fstream>

class Serializable {
public:
    virtual ~Serializable() {}
    virtual void serialize(std::ostream& stream) = 0;
    virtual void deserialize(std::istream& stream) = 0;
};

class MyObject : public Serializable {
public:
    int x;
    double y;
    std::string name;

    void serialize(std::ostream& stream) override {
        stream << x << " " << y << " " << name << std::endl;
    }

    void deserialize(std::istream& stream) override {
        stream >> x >> y >> name;
    }
};




//example
int main() {
    MyObject obj;
    obj.x = 42;
    obj.y = 3.14;
    obj.name = "Hello, world!";

    std::ofstream file("data.txt");
    obj.serialize(file);
    file.close();

    std::ifstream infile("data.txt");
    MyObject new_obj;
    new_obj.deserialize(infile);

    std::cout << "x: " << new_obj.x << ", y: " << new_obj.y << ", name: " << new_obj.name << std::endl;

    return 0;
}
