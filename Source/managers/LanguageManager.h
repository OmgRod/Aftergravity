#pragma once

#pragma once

#include <string>

#include "rapidjson/document.h"
#include <axmol.h>
#include <string>
#include <string_view>
#include <regex>
#include <sstream>
#include <utility>

namespace aftergravity {

enum class Language {
    English,
    AmericanEnglish,
    Spanish,
    German,
    French,
    Polish,
    Portuguese,

    Count // not an actual language lmfao
};

struct LanguageInfo {
    Language id;
    std::string_view code;
    std::string_view displayName;
};

constexpr std::array<LanguageInfo, static_cast<size_t>(Language::Count)> Languages{{
    { Language::English,    "en-GB",    "English (United Kingdom)" },
    { Language::AmericanEnglish,    "en-US",    "English (United States)" },
    { Language::Spanish,    "es",    "Español" },
    { Language::German,     "de",    "Deutsch" },
    { Language::French,     "fr",    "Français" },
    { Language::Polish,     "pl",    "Polski" },
    { Language::Portuguese, "pt-PT",    "Português (Portugal)" }
}};

constexpr const LanguageInfo& getLanguageInfo(Language lang)
{
    return Languages[static_cast<size_t>(lang)];
}

}

class LanguageManager {
public:
    static LanguageManager& get();

    void setLanguage(aftergravity::Language language);
    aftergravity::Language getLanguage() const;

    std::string_view getLanguageCode() const;
    std::string_view getLanguageName() const;

    std::string getString(const std::string& key);

    std::string getString(const std::string& key, aftergravity::Language language);

    template<typename... Args>
    std::string getString(const std::string& key, Args&&... args)
    {
        std::string result = getString(key);
        (replaceNextPlaceholder(result, toString(std::forward<Args>(args))), ...);
        return result;
    }

    template<typename... Args>
    std::string getString(const std::string& key, aftergravity::Language language, Args&&... args)
    {
        std::string result = getString(key, language);
        (replaceNextPlaceholder(result, toString(std::forward<Args>(args))), ...);
        return result;
    }

    void saveLanguage();
    void loadSavedLanguage();

    static aftergravity::Language languageFromCode(std::string_view code);
    static std::string_view codeFromLanguage(aftergravity::Language language);

    const aftergravity::LanguageInfo& getLanguageInfo(aftergravity::Language language);

private:
    LanguageManager();

    void loadLanguage(aftergravity::Language language);
    const rapidjson::Value* findValue(const std::string& key);

    rapidjson::Document _doc;
    aftergravity::Language _currentLanguage = aftergravity::Language::English;
    bool _initialized = false;

    static void replaceNextPlaceholder(std::string& text, const std::string& value);

    template<typename T>
    static std::string toString(const T& value)
    {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }
};