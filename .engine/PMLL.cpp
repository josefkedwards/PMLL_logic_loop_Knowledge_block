#include "PMLL.h"
#include <iostream>

int main() {
    // Initialize PMLL with a memory file
    PMLL pmll("memory_data.txt");

    // Add or update memory entries
    pmll.addMemory("username", "Josef");
    pmll.addMemory("project", "Unified PMLL Logic Loop");

    // Retrieve and print memory entries
    std::cout << "Username: " << pmll.getMemory("username") << "\n";
    std::cout << "Project: " << pmll.getMemory("project") << "\n";

    // Display all memory
    pmll.displayMemory();

    // Clear memory
    pmll.clearMemory();
    std::cout << "Memory cleared.\n";

    return 0;
}


// src/PersistentMemory.cpp

#include "PersistentMemory.h"
#include <fstream>
#include <thread>
#include <future>

// LRUCache Implementation
template<typename Key, typename Value>
LRUCache<Key, Value>::LRUCache(size_t capacity) : capacity_(capacity) {}

template<typename Key, typename Value>
bool LRUCache<Key, Value>::get(const Key& key, Value& value) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = cache_map_.find(key);
    if (it == cache_map_.end()) {
        return false;
    }
    // Move the accessed item to the front of the list
    cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
    value = it->second->second;
    return true;
}

template<typename Key, typename Value>
void LRUCache<Key, Value>::put(const Key& key, const Value& value) {
    std::lock_guard<std::mutex> lock(cache_mutex_);
    auto it = cache_map_.find(key);
    if (it != cache_map_.end()) {
        // Update existing item and move to front
        it->second->second = value;
        cache_list_.splice(cache_list_.begin(), cache_list_, it->second);
        return;
    }

    // Insert new item at the front
    cache_list_.emplace_front(key, value);
    cache_map_[key] = cache_list_.begin();

    // Evict least recently used item if capacity is exceeded
    if (cache_map_.size() > capacity_) {
        auto last = cache_list_.end();
        last--;
        cache_map_.erase(last->first);
        cache_list_.pop_back();
    }
}

// Explicit template instantiation to avoid linker errors
template class LRUCache<std::string, json>;

// PersistentMemory Implementation
PersistentMemory::PersistentMemory(const std::string& memory_file, size_t cache_capacity)
    : memory_file_(memory_file),
      cache_(cache_capacity),
      logger_(spdlog::basic_logger_mt("PMLL_logger", "pml_log.txt")) {
    logger_->set_level(spdlog::level::info);
    loadMemory();
}

PersistentMemory::~PersistentMemory() {
    // Optional: Flush logs before destruction
    spdlog::drop("PMLL_logger");
}

void PersistentMemory::addMemory(const std::string& key, const json& value) {
    {
        std::lock_guard<std::mutex> lock(memory_mutex_);
        memory_data_[key] = value;
        cache_.put(key, value);
        logger_->info("Added/Updated memory for key: {}", key);
    }
    saveMemoryAsync();
}

json PersistentMemory::getMemory(const std::string& key, const json& default_value) {
    json value;
    if (cache_.get(key, value)) {
        logger_->info("Cache hit for key: {}", key);
        return value;
    }

    {
        std::lock_guard<std::mutex> lock(memory_mutex_);
        auto it = memory_data_.find(key);
        if (it != memory_data_.end()) {
            cache_.put(key, it->second);
            logger_->info("Cache miss for key: {}. Loaded from storage.", key);
            return it->second;
        }
    }
    logger_->warn("Key not found: {}", key);
    return default_value;
}

void PersistentMemory::clearMemory() {
    {
        std::lock_guard<std::mutex> lock(memory_mutex_);
        memory_data_.clear();
        cache_ = LRUCache<std::string, json>(cache_.capacity()); // Reset cache
        logger_->info("Cleared all memory.");
    }
    saveMemoryAsync();
}

void PersistentMemory::addMemoryVersion(const std::string& key, const json& value) {
    {
        std::lock_guard<std::mutex> lock(memory_mutex_);
        memory_versions_[key].push_back(value);
        memory_data_[key] = value;
        logger_->info("Added memory version for key: {}", key);
    }
    saveMemoryAsync();
}

json PersistentMemory::getMemoryVersion(const std::string& key, size_t version) {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    if (memory_versions_.find(key) != memory_versions_.end() &&
        version < memory_versions_[key].size()) {
        logger_->info("Retrieved version {} for key: {}", version, key);
        return memory_versions_[key][version];
    }
    logger_->warn("Version {} for key: {} not found.", version, key);
    return nullptr;
}

void PersistentMemory::loadMemory() {
    std::ifstream infile(memory_file_);
    if (infile.is_open()) {
        try {
            json j;
            infile >> j;
            std::lock_guard<std::mutex> lock(memory_mutex_);
            if (j.contains("memory_data")) {
                memory_data_ = j.at("memory_data").get<std::unordered_map<std::string, json>>();
            }
            if (j.contains("memory_versions")) {
                memory_versions_ = j.at("memory_versions").get<std::unordered_map<std::string, std::vector<json>>>();
            }
            logger_->info("Memory loaded from file: {}", memory_file_);
        } catch (const json::parse_error& e) {
            logger_->error("Error parsing memory file: {}", e.what());
        } catch (const json::out_of_range& e) {
            logger_->error("Missing keys in memory file: {}", e.what());
        }
        infile.close();
    } else {
        logger_->warn("Memory file not found. Starting with empty memory.");
    }
}

void PersistentMemory::saveMemory() {
    std::lock_guard<std::mutex> lock(memory_mutex_);
    std::ofstream outfile(memory_file_);
    if (outfile.is_open()) {
        try {
            json j;
            j["memory_data"] = memory_data_;
            j["memory_versions"] = memory_versions_;
            outfile << j.dump(4);
            logger_->info("Memory saved to file: {}", memory_file_);
        } catch (const json::type_error& e) {
            logger_->error("Error serializing memory data: {}", e.what());
        }
        outfile.close();
    } else {
        logger_->error("Failed to open memory file for writing: {}", memory_file_);
    }
}

void PersistentMemory::saveMemoryAsync() {
    std::future<void> fut = std::async(std::launch::async, &PersistentMemory::saveMemory, this);
    // Optionally, store futures if you need to manage their lifetimes
}
