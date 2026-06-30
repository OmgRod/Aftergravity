#include "SaveManager.h"
#include "axmol.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

#include <fstream>
#include <sstream>

using namespace ax;
using namespace rapidjson;

const rapidjson::Value* SaveManager::getValue(const rapidjson::Value& root, const std::string& key) {
    std::stringstream ss(key);
    std::string segment;

    const rapidjson::Value* current = &root;

    while (std::getline(ss, segment, '.')) {
        if (!current->IsObject()) return nullptr;

        auto it = current->FindMember(segment.c_str());
        if (it == current->MemberEnd()) return nullptr;

        current = &it->value;
    }

    return current;
}

SaveManager& SaveManager::get() {
    static SaveManager instance;
    return instance;
}

SaveManager::SaveManager() {
    _path = FileUtils::getInstance()->getWritablePath() + "save.json";
    load();
}

void SaveManager::load() {
    auto* fs = ax::FileUtils::getInstance();

    _path = fs->getWritablePath() + "save.json";

    if (!fs->isFileExist(_path)) {
        _doc.SetObject();

        save();
        return;
    }

    std::string data = fs->getStringFromFile(_path);

    if (data.empty()) {
        _doc.SetObject();
        save();
        return;
    }

    _doc.Parse(data.c_str());

    if (!_doc.IsObject()) {
        _doc.SetObject();
        save();
    }
}

void SaveManager::save() {
    if (!_doc.IsObject()) return;

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    _doc.Accept(writer);

    std::ofstream file(_path);
    file << buffer.GetString();
}

rapidjson::Value* SaveManager::findOrCreate(rapidjson::Document& doc, const std::string& key) {
    std::stringstream ss(key);
    std::string segment;

    rapidjson::Value* current = &doc;

    while (std::getline(ss, segment, '.')) {
        if (!current->IsObject()) current->SetObject();

        auto it = current->FindMember(segment.c_str());

        if (it == current->MemberEnd()) {
            rapidjson::Value k(segment.c_str(), doc.GetAllocator());
            rapidjson::Value v(rapidjson::kObjectType);

            current->AddMember(k, v, doc.GetAllocator());
            it = current->FindMember(segment.c_str());
        }

        current = &it->value;
    }

    return current;
}

bool SaveManager::has(const std::string& key) const {
    const rapidjson::Value* current = getValue(_doc, key);

    return current != nullptr;
}

template void SaveManager::set<int>(const std::string&, const int&);
template void SaveManager::set<float>(const std::string&, const float&);
template void SaveManager::set<double>(const std::string&, const double&);
template void SaveManager::set<bool>(const std::string&, const bool&);
template void SaveManager::set<std::string>(const std::string&, const std::string&);

template int SaveManager::get<int>(const std::string&, const int&);
template float SaveManager::get<float>(const std::string&, const float&);
template double SaveManager::get<double>(const std::string&, const double&);
template bool SaveManager::get<bool>(const std::string&, const bool&);
template std::string SaveManager::get<std::string>(const std::string&, const std::string&);