#pragma once
#ifndef LOCKFREEMAP_H
#define LOCKFREEMAP_H
#include <mutex>
#include <unordered_map>
#include <string>
/*
* Utility class
*
* Map that makes individual entrys blocking while allowing multiple threads to access other entries at the same time
*/
template<typename KeyType, typename ValueType>
class LockFreeMap {
private:
	std::unordered_map<KeyType, ValueType> map_;
	std::unordered_map<KeyType, std::mutex> entryMutexes_;
public:
	void write(const KeyType& key, const ValueType& value) {
		std::lock_guard<std::mutex> lock(entryMutexes_[key]);
		map_[key] = value;
	}
	ValueType read(const KeyType& key) {
		std::lock_guard<std::mutex> lock(entryMutexes_[key]);
		ValueType value = map_[key];
		return value;
	}

	ValueType& get(const KeyType& key) {
		std::lock_guard<std::mutex> lock(entryMutexes_[key]);
		return map_.at(key);
	}
};
#endif