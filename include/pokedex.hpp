#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

class BasicException {
protected:
    std::string message;

public:
    explicit BasicException(const char *_message) : message(_message) {}
    virtual const char *what() const {
        return message.c_str();
    }
};

class ArgumentException : public BasicException {
public:
    explicit ArgumentException(const char *_message) : BasicException(_message) {}
};

class IteratorException : public BasicException {
public:
    explicit IteratorException(const char *_message) : BasicException(_message) {}
};

struct Pokemon {
    char name[12];
    int id;
    std::vector<std::string> types;

    Pokemon() {
        memset(name, 0, sizeof(name));
        id = 0;
    }
};

class Pokedex {
private:
    std::string fileName;
    std::map<int, Pokemon> data;

    static bool isValidName(const char *name) {
        if (!name || name[0] == '\0') return false;
        for (int i = 0; name[i] != '\0'; ++i) {
            if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z'))) {
                return false;
            }
        }
        return true;
    }

    static bool isValidType(const std::string &type) {
        static const char *validTypes[] = {"water", "fire", "grass", "electric", "ground", "flying", "dragon"};
        for (const char *vt : validTypes) {
            if (type == vt) return true;
        }
        return false;
    }

    static float getMultiplier(const std::string &atk, const std::string &def) {
        if (atk == "water") {
            if (def == "fire" || def == "ground") return 2.0f;
            if (def == "water" || def == "grass" || def == "dragon") return 0.5f;
        } else if (atk == "fire") {
            if (def == "grass") return 2.0f;
            if (def == "water" || def == "fire" || def == "dragon") return 0.5f;
        } else if (atk == "grass") {
            if (def == "water" || def == "ground") return 2.0f;
            if (def == "fire" || def == "grass" || def == "flying" || def == "dragon") return 0.5f;
        } else if (atk == "electric") {
            if (def == "water" || def == "flying") return 2.0f;
            if (def == "grass" || def == "electric" || def == "dragon") return 0.5f;
            if (def == "ground") return 0.0f;
        } else if (atk == "ground") {
            if (def == "fire" || def == "electric") return 2.0f;
            if (def == "grass") return 0.5f;
            if (def == "flying") return 0.0f;
        } else if (atk == "flying") {
            if (def == "grass") return 2.0f;
            if (def == "electric") return 0.5f;
        } else if (atk == "dragon") {
            if (def == "dragon") return 2.0f;
        }
        return 1.0f;
    }

    void load() {
        std::ifstream ifs(fileName, std::ios::binary);
        if (!ifs) return;
        int size;
        if (!ifs.read(reinterpret_cast<char*>(&size), sizeof(size))) return;
        for (int i = 0; i < size; ++i) {
            Pokemon p;
            ifs.read(p.name, sizeof(p.name));
            ifs.read(reinterpret_cast<char*>(&p.id), sizeof(p.id));
            int typeCount;
            ifs.read(reinterpret_cast<char*>(&typeCount), sizeof(typeCount));
            for (int j = 0; j < typeCount; ++j) {
                int len;
                ifs.read(reinterpret_cast<char*>(&len), sizeof(len));
                char *buf = new char[len + 1];
                ifs.read(buf, len);
                buf[len] = '\0';
                p.types.push_back(std::string(buf));
                delete[] buf;
            }
            data[p.id] = p;
        }
    }

    void save() {
        std::ofstream ofs(fileName, std::ios::binary);
        if (!ofs) return;
        int size = data.size();
        ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (auto const& [id, p] : data) {
            ofs.write(p.name, sizeof(p.name));
            ofs.write(reinterpret_cast<const char*>(&p.id), sizeof(p.id));
            int typeCount = p.types.size();
            ofs.write(reinterpret_cast<const char*>(&typeCount), sizeof(typeCount));
            for (const auto &t : p.types) {
                int len = t.length();
                ofs.write(reinterpret_cast<const char*>(&len), sizeof(len));
                ofs.write(t.c_str(), len);
            }
        }
    }

public:
    explicit Pokedex(const char *_fileName) : fileName(_fileName) {
        load();
    }

    ~Pokedex() {
        save();
    }

    bool pokeAdd(const char *name, int id, const char *types) {
        if (!isValidName(name)) {
            std::string msg = "Argument Error: PM Name Invalid (";
            msg += name;
            msg += ")";
            throw ArgumentException(msg.c_str());
        }
        std::vector<std::string> tList;
        std::string ts(types);
        size_t pos = 0;
        while ((pos = ts.find('#')) != std::string::npos) {
            std::string t = ts.substr(0, pos);
            if (!isValidType(t)) {
                std::string msg = "Argument Error: PM Type Invalid (";
                msg += t;
                msg += ")";
                throw ArgumentException(msg.c_str());
            }
            tList.push_back(t);
            ts.erase(0, pos + 1);
        }
        if (!isValidType(ts)) {
            std::string msg = "Argument Error: PM Type Invalid (";
            msg += ts;
            msg += ")";
            throw ArgumentException(msg.c_str());
        }
        tList.push_back(ts);

        if (data.count(id)) return false;
        for (auto const& [did, dp] : data) {
            if (strcmp(dp.name, name) == 0) return false;
        }

        Pokemon p;
        strncpy(p.name, name, 11);
        p.name[11] = '\0';
        p.id = id;
        p.types = tList;
        data[id] = p;
        return true;
    }

    bool pokeDel(int id) {
        return data.erase(id) > 0;
    }

    std::string pokeFind(int id) const {
        auto it = data.find(id);
        if (it != data.end()) return it->second.name;
        return "None";
    }

    std::string typeFind(const char *types) const {
        std::vector<std::string> queryTypes;
        std::string ts(types);
        size_t pos = 0;
        while ((pos = ts.find('#')) != std::string::npos) {
            std::string t = ts.substr(0, pos);
            if (!isValidType(t)) {
                std::string msg = "Argument Error: PM Type Invalid (";
                msg += t;
                msg += ")";
                throw ArgumentException(msg.c_str());
            }
            queryTypes.push_back(t);
            ts.erase(0, pos + 1);
        }
        if (!isValidType(ts)) {
            std::string msg = "Argument Error: PM Type Invalid (";
            msg += ts;
            msg += ")";
            throw ArgumentException(msg.c_str());
        }
        queryTypes.push_back(ts);

        std::vector<std::string> results;
        for (auto const& [id, p] : data) {
            bool allMatch = true;
            for (const auto &qt : queryTypes) {
                bool match = false;
                for (const auto &pt : p.types) {
                    if (pt == qt) {
                        match = true;
                        break;
                    }
                }
                if (!match) {
                    allMatch = false;
                    break;
                }
            }
            if (allMatch) results.push_back(p.name);
        }

        if (results.empty()) return "None";
        std::string res = std::to_string(results.size());
        for (const auto &name : results) {
            res += "\n" + name;
        }
        return res;
    }

    float attack(const char *type, int id) const {
        if (!isValidType(type)) {
            std::string msg = "Argument Error: PM Type Invalid (";
            msg += type;
            msg += ")";
            throw ArgumentException(msg.c_str());
        }
        auto it = data.find(id);
        if (it == data.end()) return -1.0f;

        float totalMult = 1.0f;
        for (const auto &pType : it->second.types) {
            totalMult *= getMultiplier(type, pType);
        }
        return totalMult;
    }

    int catchTry() const {
        if (data.empty()) return 0;

        std::set<int> owned;
        owned.insert(data.begin()->first);

        bool changed = true;
        while (changed) {
            changed = false;
            for (auto const& [id, p] : data) {
                if (owned.count(id)) continue;

                bool canCatch = false;
                for (int oid : owned) {
                    const Pokemon &op = data.at(oid);
                    for (const auto &atkType : op.types) {
                        float mult = 1.0f;
                        for (const auto &defType : p.types) {
                            mult *= getMultiplier(atkType, defType);
                        }
                        if (mult >= 2.0f) {
                            canCatch = true;
                            break;
                        }
                    }
                    if (canCatch) break;
                }

                if (canCatch) {
                    owned.insert(id);
                    changed = true;
                }
            }
        }
        return owned.size();
    }

    struct iterator {
        std::map<int, Pokemon>::iterator it;
        std::map<int, Pokemon> *container;

        iterator() : container(nullptr) {}
        iterator(std::map<int, Pokemon>::iterator _it, std::map<int, Pokemon> *_container)
            : it(_it), container(_container) {}

        iterator &operator++() {
            if (it == container->end()) throw IteratorException("Iterator Error: Increment Beyond End");
            ++it;
            return *this;
        }
        iterator &operator--() {
            if (it == container->begin()) throw IteratorException("Iterator Error: Decrement Before Begin");
            --it;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        iterator &operator=(const iterator &rhs) {
            it = rhs.it;
            container = rhs.container;
            return *this;
        }
        bool operator==(const iterator &rhs) const {
            return it == rhs.it;
        }
        bool operator!=(const iterator &rhs) const {
            return it != rhs.it;
        }
        Pokemon &operator*() const {
            if (it == container->end()) throw IteratorException("Iterator Error: Dereference End Iterator");
            return it->second;
        }
        Pokemon *operator->() const {
            if (it == container->end()) throw IteratorException("Iterator Error: Dereference End Iterator");
            return &(it->second);
        }
    };

    iterator begin() {
        return iterator(data.begin(), &data);
    }

    iterator end() {
        return iterator(data.end(), &data);
    }
};

#endif
