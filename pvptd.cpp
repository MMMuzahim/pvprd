#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <limits>
#include <cctype>

using namespace std;

struct Object {
private:
    string ident;
    string type;
    int level;
    string rarity;
    double damege = 0;
    double protection = 0;
    double speed = 0;

public:
    string GetIdent() const { return ident; }
    string GetType() const { return type; }
    int GetLevel() const { return level; }
    string GetRarity() const { return rarity; }
    double GetDamege() const { return damege; }
    double GetProtection() const { return protection; }
    double GetSpeed() const { return speed; }

    void SetDamege(double new_value) { damege = new_value; }
    void SetProtection(double new_value) { protection = new_value; }
    void SetSpeed(double new_value) { speed = new_value; }

    Object (string s) {
        string temp = "";
        for (size_t i = 0; i < s.size(); i++) {
            if (set<char>{'{', '}', ',' , ':', '\"'}.count(s[i]) == 1) {
                temp = temp + " ";
            } else {
                temp = temp + s[i];
            }
        }
        
        stringstream ss(temp);
        string parameter;
        while (ss >> parameter) {
            if (parameter == "ident") {
                ss >> ident;
                continue;
            }
            if (parameter == "type") {
                ss >> type;
                continue;
            }
            if (parameter == "level") {
                ss >> level;
                continue;
            }
            if (parameter == "rarity") {
                ss >> rarity;
                continue;
            }
            if (parameter == "damage") {
                ss >> damege;
                continue;
            }
            if (parameter == "protection") {
                ss >> protection;
                continue;
            }
            if (parameter == "speed") {
                ss >> speed;
                continue;
            }
        }
    }
};

ostream& operator<<(ostream& stream, const Object& object) {
    stream << "{ident: \"" << object.GetIdent() << "\", type: " << object.GetType()
           << ", level: " << object.GetLevel() << ", rarity: " << object.GetRarity();
    if (object.GetDamege() != 0) {
        stream << ", damage: " << object.GetDamege() << ", speed: " << object.GetSpeed() << " }";
    }
    if (object.GetProtection() != 0) {
        stream << ", protection: " << object.GetProtection() << " }";
    }

    return stream;
}

bool operator< (const Object& lhs, const Object& rhs) {
    return lhs.GetIdent() < rhs.GetIdent();
}

struct Filter {
private:
    string name;
    string sign = "";
    string value_str = "";
    int value_int = 0;

public:
    string GetName() const { return name; }
    string GetSign() const { return sign; }
    string GetValue_str() const { return value_str; }
    int GetValue_int() const { return value_int; }
    Filter (string name, string sign = "", string value_str = "", int value_int = 0) {
        this->name = name;
        this->sign = sign;
        this->value_str = value_str;
        this->value_int = value_int;
    }
};

struct Buff {
private:
    string ident;
    vector<Filter> filters;
    string type;
    double value;
    bool Use(Object& item) const {
        if (type == "DamageBuff" && item.GetDamege() != 0) {
            item.SetDamege(item.GetDamege() + value);
            return true;
        }
        if (type == "ProtectionBuff" && item.GetProtection() != 0) {
            item.SetProtection(item.GetProtection() + value);
            return true;
        }
        if (type == "SpeedBuff" && item.GetSpeed() != 0) {
            item.SetSpeed(item.GetSpeed() + value);
            return true;
        }
        return false;
    }

public:
    string GetIdent() const { return ident; }
    vector<Filter> GetFilters() const { return filters; }
    string GetType() const { return type; }
    double GetValue() const { return value; }

    Buff(string s) {
        string temp = "";
        for (size_t i = 0; i < s.size(); i++) {
            if (set<char>{'{', '}', ',' , ':', '\"'}.count(s[i]) == 1) {
                temp = temp + ' ';
            } else {
                temp = temp + s[i];
            }
        }

        stringstream ss(temp);
        string parameter;
        while (ss >> parameter) {
            if (parameter == "ident") {
                ss >> ident;
                continue;
            }
            if (parameter == "filters") {
                string name;
                ss >> name >> name;
                while (name != "]") {
                    string sign = "";
                    int value_int = 0;
                    string value_str = "";
                    if (name == "level") {
                        ss >> sign >> value_int;
                    } else {
                        ss >> value_str;
                    }
                    filters.push_back({name, sign, value_str, value_int});
                    ss >> name;
                }
                continue;
            }
            if (parameter == "type") {
                ss >> type;
                continue;
            }
            if (parameter == "value") {
                ss >> value;
                continue;
            }
        }
    }

    set<Object> UseBuff(vector<Object>& inventory) const {
        set<string> types;
        set<string> rarities;
        int min_level = 0;
        int max_level = numeric_limits<int>::max();
        for (const Filter& filter : filters) {
            if (filter.GetName() == "type") {
                types.insert(filter.GetValue_str());
                continue;
            } else if (filter.GetName() == "rarity") {
                rarities.insert(filter.GetValue_str());
                continue;
            } else if (filter.GetName() == "level") {
                if (filter.GetSign() == ">" && min_level < filter.GetValue_int()) {
                    min_level = filter.GetValue_int() + 1;
                }
                
                if (filter.GetSign() == "<" && max_level > filter.GetValue_int()) {
                    max_level = filter.GetValue_int() - 1;
                }

                if (filter.GetSign() == ">=" && min_level <= filter.GetValue_int()) {
                    min_level = filter.GetValue_int();
                }

                if (filter.GetSign() == "<=" && max_level >= filter.GetValue_int()) {
                    max_level = filter.GetValue_int();
                }

                if (filter.GetSign() == "==" && min_level != filter.GetValue_int() && max_level != filter.GetValue_int()) {
                    min_level = filter.GetValue_int();
                    max_level = filter.GetValue_int();
                }
            } else {
                cout << "Incorrect filter!" << endl;
            }
        }
        
        set<Object> result;
        for (Object& item : inventory) {
            if ((types.count(item.GetType()) == 1 || types.size() == 0) && (rarities.count(item.GetRarity()) == 1 || rarities.size() == 0) && item.GetLevel() >= min_level && item.GetLevel() <= max_level) {
                if (Use(item)) {
                    result.insert(item);
                }
            }
        }
        return result;
    }
};

bool operator< (const Buff& lhs, const Buff& rhs) {
    return lhs.GetIdent() < rhs.GetIdent();
}

int main()
{
    vector<Object> inventory;
    vector<Buff> buffs;
    cout << "Для добавления предмета в инвентарь введите команду \"ADD_Obj\"" << endl 
         << "Для добавления нескольких предметов в инвентарь введите команду \"ADD_Obj_list\" (концом ввода является пустая строка)" << endl 
         << "Для добавления модификатора введите команду \"ADD_Buff\"" << endl
         << "Для добавления нескольких модификаторов введите команду \"ADD_Buff_list\" (концом ввода является пустая строка)" << endl
         << "Для завершения программы введите команду \"EXIT\"" << endl;
    string command;
    while (getline(cin, command)) {
        for (size_t i = 0; i < command.size(); ++i) {
            command[i] = (char)tolower(command[i]);
        }

        if (command == "add_obj") {
            cout << "Введите предмет инвентаря:" << endl;
            string s;
            getline(cin, s);
            inventory.push_back(Object(s));
            continue;
        } else if (command == "add_obj_list") {
            cout << "Введите предметы инвентаря:" << endl;
            string s;
            while (getline(cin, s)) {
                if (s == "")
                    break;
                inventory.push_back(Object(s));
            }
            continue;
        } else if (command == "add_buff") {
            cout << "Введите модификатор:" << endl;
            string s;
            getline(cin, s);
            buffs.push_back(Buff(s));

            cout << "Модифицированные предметы:" << endl;
            for (const Object& item : buffs[buffs.size() - 1].UseBuff(inventory)) {
                cout << item << endl;
            }
            cout << endl;
            continue;
        } else if (command == "add_buff_list") {
            cout << "Введите модификаторы:" << endl;
            string s;
            vector<Buff> temp_buff_vector;
            while (getline(cin, s)) {
                if (s == "")
                    break;
                Buff b = Buff(s);
                buffs.push_back(b);
                temp_buff_vector.push_back(b);
            }

            cout << "Модифицированные предметы:" << endl;
            set<Object> temp_item;
            for (size_t i = 0; i < temp_buff_vector.size(); ++i) {
                for (const Object& item : temp_buff_vector[i].UseBuff(inventory)) {
                    cout << item << endl;
                }
            }
            cout << endl;
            continue;
        } else if (command == "exit") {
            break;
        } else {
            cout << "Wrong command" << endl;
        }
    }
    
    return 0;
}
