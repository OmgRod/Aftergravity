#pragma once

#include <string>
#include "rapidjson/document.h"

class SaveManager {
public:
    static SaveManager& get();

    void load();
    void save();

    template <typename T>
    void SaveManager::set(const std::string& key, const T& value) {
        auto* val = findOrCreate(_doc, key);

        if (!val) return;

        if constexpr (std::is_same_v<T, int>) {
            val->SetInt(value);
        } else if constexpr (std::is_same_v<T, float>) {
            val->SetFloat(value);
        } else if constexpr (std::is_same_v<T, double>) {
            val->SetDouble(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            val->SetBool(value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            val->SetString(value.c_str(), _doc.GetAllocator());
        } else if constexpr (std::is_same_v<T, const char*>) {
            val->SetString(value, _doc.GetAllocator());
        }
    }

    template <typename T>
    T SaveManager::get(const std::string& key, const T& fallback) {
        const rapidjson::Value* v = getValue(_doc, key);

        if (!v) return fallback;

        if constexpr (std::is_same_v<T, int>) {
            return v->IsInt() ? v->GetInt() : fallback;
        } else if constexpr (std::is_same_v<T, float>) {
            return v->IsNumber() ? v->GetFloat() : fallback;
        } else if constexpr (std::is_same_v<T, double>) {
            return v->IsNumber() ? v->GetDouble() : fallback;
        } else if constexpr (std::is_same_v<T, bool>) {
            return v->IsBool() ? v->GetBool() : fallback;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v->IsString() ? std::string(v->GetString()) : fallback;
        }

        return fallback;
    }

    bool has(const std::string& key) const;

private:
    SaveManager();

    void ensurePath();

    std::string _path;
    rapidjson::Document _doc;

    static rapidjson::Value* findOrCreate(
        rapidjson::Document& doc,
        const std::string& key);

    static const rapidjson::Value* getValue(
        const rapidjson::Value& root,
        const std::string& key);
};