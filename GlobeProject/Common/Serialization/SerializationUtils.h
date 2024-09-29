#pragma once
#

#include <iostream>
#include <vector>
#include <cstring> // For memcpy
#include <cstddef> // For std::byte
#include <fstream>
#include <functional>
#include <string>

#define TYPE_NAME(type) \
    std::string typeName() { return #type; } \
    uint32_t typeId() { return hash(typeName()); } \
    static uint32_t classId() { return hash(#type); } \
    static uint32_t hash(const std::string& str) { return std::hash<std::string>()(str); }

struct PacketHeader {
    uint32_t type;
    uint32_t packetBodySize;
};

struct PacketBody {
    std::vector<std::byte> data;
};


struct CodecPacket {
    PacketHeader header;
    PacketBody body;
};



// Template functions for serialization/deserialization
template <typename T>
inline void serializePrimitive(std::vector<std::byte>& buffer, const T& value) {
    buffer.insert(buffer.end(),
        reinterpret_cast<const std::byte*>(&value),
        reinterpret_cast<const std::byte*>(&value) + sizeof(T));
}

template <typename T>
inline void serializeVector(std::vector<std::byte>& buffer, const std::vector<T>& vec) {
    size_t vecSize = vec.size();
    serializePrimitive(buffer, vecSize); // Serialize size of the vector
    buffer.insert(buffer.end(),
        reinterpret_cast<const std::byte*>(vec.data()),
        reinterpret_cast<const std::byte*>(vec.data()) + vecSize * sizeof(T));
}

template <typename T>
inline void deserializePrimitive(const std::vector<std::byte>& buffer, size_t& offset, T& value) {
    std::memcpy(&value, &buffer[offset], sizeof(T));
    offset += sizeof(T);
}

template <typename T>
inline void deserializeVector(const std::vector<std::byte>& buffer, size_t& offset, std::vector<T>& vec) {
    size_t vecSize;
    deserializePrimitive(buffer, offset, vecSize); // Deserialize size of the vector
    vec.resize(vecSize);
    std::memcpy(vec.data(), &buffer[offset], vecSize * sizeof(T));
    offset += vecSize * sizeof(T);
}




class Writer {

    std::ofstream outFile;

public:
    Writer(const char* path) {
        outFile = std::ofstream(path, std::ios::binary);
    }

    //When calling this function MAKE SURE that object passed in has a 'serialize' function defined
    template <typename T>
    void write(T dataStruct, int id) {
        CodecPacket tempPacket;

        std::vector<std::byte> serializedData;
        dataStruct.serialize(serializedData);

        tempPacket.body.data = serializedData;
        tempPacket.header.type = id;
        tempPacket.header.packetBodySize = tempPacket.body.data.size();

        write(reinterpret_cast<const char*>(&tempPacket.header), sizeof(tempPacket.header));
        write(reinterpret_cast<const char*>(tempPacket.body.data.data()), tempPacket.body.data.size());

    }

    void write(const char* str, std::streamsize count) {
        outFile.write(str, count);
    }

    void close() {
        outFile.close();
    }


};


class Reader {

    std::ifstream inFile;

public:
    Reader(const char* path) {
        inFile = std::ifstream(path, std::ios::binary);
    }

    bool reachedEndOfFile() {
        if (inFile.eof()) {
            return true;
        }
        else {
            return false;
        }
    }

    CodecPacket getNextPacket() {

        CodecPacket inPacket;

        read(reinterpret_cast<char*>(&inPacket.header), sizeof(inPacket.header));
        inPacket.body.data.resize(inPacket.header.packetBodySize);
        read(reinterpret_cast<char*>(inPacket.body.data.data()), inPacket.body.data.size());

        return inPacket;
    }


    void read(char* str, std::streamsize count) {
        inFile.read(str, count);
    }

    void close() {
        inFile.close();
    }


};

// Registry class that maps type IDs to deserialization functions
class Registry {
public:
    static Registry& getInstance() {
        static Registry instance; // Guaranteed to be destroyed and instantiated on first use
        return instance;
    }

    // Register a type with its ID and deserialization function
    template<typename T>
    void registerType() {
        uint32_t id = T::classId();
        auto deserializeFunc = [](const std::vector<std::byte>& buffer) -> std::shared_ptr<void> {
            return std::make_shared<T>(T::deserialize(buffer));
            };
        std::cout << "Adding type: " << id << " to the registry" << std::endl;
        typeMap[id] = deserializeFunc;
    }

    // Deserialize an object based on the type ID
    std::shared_ptr<void> deserialize(uint32_t typeId, const std::vector<std::byte>& buffer) {
        auto it = typeMap.find(typeId);
        if (it != typeMap.end()) {
            return it->second(buffer); // Call the deserialization function
        }
        throw std::runtime_error("Type ID not registered");
    }

private:
    Registry() = default; // Private constructor
    Registry(const Registry&) = delete; // Prevent copy
    Registry& operator=(const Registry&) = delete; // Prevent assignment

    std::unordered_map<uint32_t, std::function<std::shared_ptr<void>(const std::vector<std::byte>&)>> typeMap;
};

template<typename T>
class AutoRegister {
public:
    AutoRegister() {
        Registry::getInstance().registerType<T>();
    }
};