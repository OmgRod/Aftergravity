#include "LanguageManager.h"
#include <sstream>
#include "SaveManager.h"

using namespace ax;
using namespace rapidjson;

LanguageManager::LanguageManager()
{
    _currentLanguage = aftergravity::Language::Count;
    _initialized = false;

    loadSavedLanguage();
}

LanguageManager& LanguageManager::get()
{
    static LanguageManager instance;
    return instance;
}

void LanguageManager::setLanguage(aftergravity::Language language)
{
    if (_initialized && _currentLanguage == language)
        return;

    _currentLanguage = language;
    loadLanguage(language);
    _initialized = true;
}

void LanguageManager::loadLanguage(aftergravity::Language language)
{
    std::string code = getLanguageInfo(language).code;

    std::string path = "lang/" + code + ".json";
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);

    _doc.SetObject();

    if (!FileUtils::getInstance()->isFileExist(fullPath))
    {
        AXLOG("Missing language file: %s", fullPath.c_str());
        return;
    }

    std::string jsonStr = FileUtils::getInstance()->getStringFromFile(fullPath);

    if (jsonStr.empty())
    {
        AXLOG("Empty language file: %s", fullPath.c_str());
        return;
    }

    _doc.Parse(jsonStr.c_str());

    if (_doc.HasParseError() || !_doc.IsObject())
    {
        AXLOG("Invalid JSON language file: %s", fullPath.c_str());
        _doc.SetObject();
        return;
    }
}

const rapidjson::Value* LanguageManager::findValue(const std::string& key)
{
    if (!_doc.IsObject())
        return nullptr;

    const rapidjson::Value* current = &_doc;

    if (key.find('.') == std::string::npos)
    {
        auto it = current->FindMember(key.c_str());
        if (it == current->MemberEnd())
            return nullptr;

        return &it->value;
    }

    std::stringstream ss(key);
    std::string segment;

    while (std::getline(ss, segment, '.'))
    {
        if (!current->IsObject())
            return nullptr;

        auto it = current->FindMember(segment.c_str());
        if (it == current->MemberEnd())
            return nullptr;

        current = &it->value;
    }

    return current;
}

std::string LanguageManager::getString(const std::string& key)
{
    const rapidjson::Value* val = findValue(key);

    if (!val)
    {
        AXLOG("Missing localization key: %s", key.c_str());
        return key;
    }

    if (!val->IsString())
    {
        AXLOG("Localization key not string: %s", key.c_str());
        return key;
    }

    return val->GetString();
}

aftergravity::Language LanguageManager::getLanguage() const
{
    return _currentLanguage;
}

std::string_view LanguageManager::getLanguageCode() const
{
    return aftergravity::getLanguageInfo(_currentLanguage).code;
}

std::string_view LanguageManager::getLanguageName() const
{
    return aftergravity::getLanguageInfo(_currentLanguage).displayName;
}

void LanguageManager::saveLanguage()
{
    SaveManager::get().set<std::string>(
        "language",
        std::string(getLanguageCode())
    );
}

void LanguageManager::loadSavedLanguage()
{
    auto code = SaveManager::get().get<std::string>(
        "language",
        "en"
    );

    setLanguage(languageFromCode(code));
}

std::string_view LanguageManager::codeFromLanguage(aftergravity::Language language)
{
    return aftergravity::getLanguageInfo(language).code;
}

aftergravity::Language LanguageManager::languageFromCode(std::string_view code)
{
    for (const auto& language : aftergravity::Languages)
    {
        if (language.code == code)
            return language.id;
    }

    return aftergravity::Language::English;
}

std::string LanguageManager::getString(
    const std::string& key,
    aftergravity::Language language)
{
    std::string code = getLanguageInfo(language).code;

    std::string path = "lang/" + code + ".json";
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);

    if (!FileUtils::getInstance()->isFileExist(fullPath))
        return key;

    std::string jsonStr = FileUtils::getInstance()->getStringFromFile(fullPath);

    if (jsonStr.empty())
        return key;

    rapidjson::Document tempDoc;
    tempDoc.Parse(jsonStr.c_str());

    if (tempDoc.HasParseError() || !tempDoc.IsObject())
        return key;

    const rapidjson::Value* current = &tempDoc;

    std::stringstream ss(key);
    std::string segment;

    while (std::getline(ss, segment, '.'))
    {
        if (!current->IsObject())
            return key;

        auto it = current->FindMember(segment.c_str());
        if (it == current->MemberEnd())
            return key;

        current = &it->value;
    }

    if (!current->IsString())
        return key;

    return current->GetString();
}

const aftergravity::LanguageInfo& LanguageManager::getLanguageInfo(aftergravity::Language language)
{
    for (const auto& lang : aftergravity::Languages)
    {
        if (lang.id == language)
            return lang;
    }

    static aftergravity::LanguageInfo fallback{ aftergravity::Language::English, "en", "English" };
    return fallback;
}