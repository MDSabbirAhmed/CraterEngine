//python code
//
//import pickle
//
//# create an object to be serialized
//data = {'name': 'Alice', 'age': 25, 'email': 'alice@example.com'}
//
//# serialize the data and write it to a file
//with open('data.pkl', 'wb') as f:
//    pickle.dump(data, f)



#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// define a struct to hold the deserialized data
struct Person {
    std::string name;
    int age;
    std::string email;
};

int main() {
    // read the serialized data from a file
    std::ifstream f("data.pkl", std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    // deserialize the data using the pickle library
    Py_Initialize();
    PyObject* module = PyImport_ImportModule("pickle");
    PyObject* load = PyObject_GetAttrString(module, "loads");
    PyObject* arg = PyBytes_FromStringAndSize(&buffer[0], buffer.size());
    PyObject* obj = PyObject_CallFunctionObjArgs(load, arg, NULL);

    // convert the deserialized data to a C++ struct
    Person p;
    PyObject* py_name = PyObject_GetAttrString(obj, "name");
    p.name = PyUnicode_AsUTF8(py_name);
    p.age = PyLong_AsLong(PyObject_GetAttrString(obj, "age"));
    PyObject* py_email = PyObject_GetAttrString(obj, "email");
    p.email = PyUnicode_AsUTF8(py_email);

    // print the deserialized data
    std::cout << "Name: " << p.name << std::endl;
    std::cout << "Age: " << p.age << std::endl;
    std::cout << "Email: " << p.email << std::endl;

    // clean up the Python objects
    Py_DECREF(module);
    Py_DECREF(load);
    Py_DECREF(arg);
    Py_DECREF(obj);
    Py_DECREF(py_name);
    Py_DECREF(py_email);

    return 0;
}
