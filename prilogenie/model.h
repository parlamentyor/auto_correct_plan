#ifndef MODEL_H
#define MODEL_H

#include<filesystem>
#include<optional>
#include<string>
#include<unordered_map>

namespace model {

    namespace fs = std::filesystem;

    struct Date {
        int day_;
        int month_;
        int year_;
    };

    struct Price {
        int ruble_;
        int kop_;
    };

    enum TypeContract {
        GOZ,
        ATT,
        BEK,
        SI
    };

    struct SeparateWork {
        std::string name_;
        std::vector<std::string> names_responsible_employees_;
        Date date_deadline_;
    };

    struct Contract {
        std::optional<std::string> number_; // подумать (протестировать) как будет работать с русским алфавитом
        Date date_;
//        std::unordered_map<int, std::string> id_number_; //на будущее для осуществления быстрого поиска
//        std::unordered_map<std::string, int> number_id; //на будущее для осуществления быстрого поиска
        std::optional<std::string> name_organization_; // подумать (протестировать) как будет работать с русским алфавитом
        std::optional<std::string> name_short_;
        std::optional<std::string> name_full_;
        Date date_deadline_;
        std::optional<std::string> name_responsible_employee_;
        Price price_;
        Price price_other_department_;
        bool with_nds_;
        int stavka_nds_;
        TypeContract type_;
        bool with_stage_;
        std::vector<SeparateWork> pool_work;
        int id_ = ++id_counter_;

        static int id_counter_;
    };

    class User {
    public:
        User(const std::string& login, const std::string& pass)
            : id_(++id_counter_)
            , login_(login)
            , pass_(pass) {
        }

        const std::string& GetPass() const {
            return pass_;
        }

        int GetId() const {
            return id_;
        }

    private:
        int id_;
        std::string login_;
        std::string pass_;
        static int id_counter_;
    };

    class DocumentImpl {
    public:
        bool AddContract(Contract entered_data_contract);
        bool EreseContract (std::string name_contract_);
        void SetPath(std::string s_path);
        std::string GetPath() const;
        bool HasValuePath() const;

    private:
//        fs::path path_; // нужно протестировать как он работатет с путями win, русскими буквами, пробелами в имени  --- херово работает
        std::optional<std::string> path_;
    };

    class DocumentPlanMonth : public DocumentImpl {};


    class TableBudget : public DocumentImpl {};


    class DocumentPlanWeek {};


    class DocumentСontractWeek {};


} // namespace model

#endif // MODEL_H
