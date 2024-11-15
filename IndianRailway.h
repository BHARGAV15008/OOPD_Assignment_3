#ifndef INDIAN_RAILWAY_H
#define INDIAN_RAILWAY_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <sstream>
#include <type_traits>
#include <cassert>
#include <regex>

namespace Style {
    const std::string GREEN = "\033[1;32m";
    const std::string RED = "\033[1;31m";
    const std::string YELLOW = "\033[1;33m";
    const std::string RESET = "\033[0m";
    
    const std::string TOP_LEFT = "||";
    const std::string TOP_RIGHT = "||";
    const std::string BOTTOM_LEFT = "||";
    const std::string BOTTOM_RIGHT = "||";
    const std::string HORIZONTAL = "=";
    const std::string VERTICAL = "||";
}

class RwyException : public std::runtime_error {
public:
    explicit RwyException(const std::string& msg) : std::runtime_error(msg) {}
};

class Validator {
public:
    static bool isValidName(const std::string& name) {
        std::regex nameRegex("^[A-Za-z][A-Za-z0-9 ]{1,49}$");
        return std::regex_match(name, nameRegex);
    }

    static bool isValidCode(const std::string& code) {
        std::regex codeRegex("^[A-Z]{1,3}[0-9]{1,3}$");
        return std::regex_match(code, codeRegex);
    }

    static bool isValidFreq(int freq) {
        return freq >= 10 && freq <= 30;
    }
};

template <typename T>
std::string toString(const T& val) {
    if constexpr (std::is_same_v<T, std::string>) {
        return val;
    } else {
        return std::to_string(val);
    }
}

// Forward declarations
template <typename T> class Station;
template <typename T> class Line;
template <typename T> class Platform;

template <typename T>
class BaseEntity {
protected:
    T id;
    std::string desc;

public:
    BaseEntity(T entityId, const std::string& d = "") : id(entityId), desc(d) {}
    virtual ~BaseEntity() = default;

    T getId() const { return id; }
    std::string getDesc() const { return desc; }

    virtual void validate() const = 0;
    virtual void show() const = 0;

    std::string makeBox(const std::string& content, const std::string& color) const {
        std::string horizontalLine;
        for(size_t i = 0; i < content.length() + 2; ++i) {
            horizontalLine += Style::HORIZONTAL;
        }
        
        return color + 
               Style::TOP_LEFT + horizontalLine + Style::TOP_RIGHT + "\n" +
               Style::VERTICAL + " " + content + " " + Style::VERTICAL + "\n" +
               Style::BOTTOM_LEFT + horizontalLine + Style::BOTTOM_RIGHT + 
               Style::RESET + "\n";
    }
};

template <typename T>
class Line : public BaseEntity<T> {
private:
    std::string type;
    std::unordered_map<T, std::shared_ptr<Station<T>>> stations;

public:
    Line(T id, const std::string& t)
        : BaseEntity<T>(id, "Line"), type(t) {
        validate();
    }

    void validate() const override {
        if (type != "Express" && type != "Passenger" && type != "Freight") {
            throw RwyException("Invalid Line Type: " + type);
        }
    }

    void addStn(std::shared_ptr<Station<T>> station) {
        if (stations.find(station->getId()) == stations.end()) {
            stations[station->getId()] = station;
        }
    }

    void show() const override {
        std::cout << this->makeBox(
            "Line ID: " + toString(this->getId()) + 
            " (Type: " + type + ")",
            Style::YELLOW
        );
        
        std::cout << "Stations:\n";
        for (const auto& s : stations) {
            std::cout << "  - " << toString(s.second->getId()) << "\n";
        }
    }

    const std::string& getType() const { return type; }
    const auto& getStations() const { return stations; }
};

template <typename T>
class Platform : public BaseEntity<T> {
private:
    int freq;
    bool access;

public:
    Platform(T id, int f, bool a = true) 
        : BaseEntity<T>(id, "Platform"), freq(f), access(a) {
        validate();
    }

    void validate() const override {
        if (!Validator::isValidFreq(freq)) {
            throw RwyException("Invalid Frequency: " + std::to_string(freq));
        }
    }

    void show() const override {
        std::cout << this->makeBox(
            "Platform ID: " + toString(this->getId()) + 
            " (Freq: " + std::to_string(freq) + "min, " +
            (access ? "Access" : "No Access") + ")",
            freq == 30 ? Style::GREEN : Style::RED
        );
    }

    int getFreq() const { return freq; }
    bool hasAccess() const { return access; }
};

template <typename T>
class Station : public BaseEntity<T> {
private:
    std::unordered_map<T, std::shared_ptr<Line<T>>> lines;
    std::unordered_map<T, std::shared_ptr<Platform<T>>> platforms;

public:
    Station(T id) : BaseEntity<T>(id, "Station") {}

    void validate() const override {
        if (lines.empty()) {
            throw RwyException("Station needs at least one line.");
        }
    }

    void addLine(std::shared_ptr<Line<T>> line) {
        if (lines.find(line->getId()) == lines.end()) {
            lines[line->getId()] = line;
        }
    }

    void addPlatform(std::shared_ptr<Platform<T>> platform) {
        if (platforms.find(platform->getId()) == platforms.end()) {
            platforms[platform->getId()] = platform;
        }
    }

    void show() const override {
        std::cout << this->makeBox("Station ID: " + toString(this->getId()), Style::GREEN);
        
        std::cout << "Lines:\n";
        for (const auto& l : lines) {
            std::cout << "  - " << toString(l.second->getId()) << "\n";
        }
        
        std::cout << "Platforms:\n";
        for (const auto& p : platforms) {
            std::cout << "  - " << toString(p.second->getId()) 
                     << " (Freq: " << p.second->getFreq() << "min, Access: " 
                     << (p.second->hasAccess() ? "Yes" : "No") << ")\n";
        }
    }

    const auto& getLines() const { return lines; }
    const auto& getPlatforms() const { return platforms; }
};

#endif // INDIAN_RAILWAY_H
