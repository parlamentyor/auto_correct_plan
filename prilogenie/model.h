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
        std::optional<std::string> info_;
    };

    struct Payment {
        Price price_;
        Date date_;
    };

    struct Expenses {
        Price price_;
        std::string name_;
    };

    struct Stage {
        int number_;
        std::optional<std::string> name_organization_;
        std::optional<std::string> name_short_;
        std::optional<std::string> name_full_;
        Date date_deadline_;
        std::optional<std::string> name_responsible_employee_;
        Price price_;
        Price price_other_department_;
        bool with_nds_;
        int stavka_nds_;
        TypeContract type_;
        std::vector<SeparateWork> pool_work_;
        std::optional<std::string> info_;
        bool is_complet_;
        bool is_paid_;
        std::optional<std::vector<Payment>> payments_;
        std::optional<std::vector<Expenses>> expenses_;
        std::string status_payment_;
    };

    struct Contract {

        // Конструктор по умолчанию
        Contract() = default;

        // Пользовательский конструктор для создания из данных
        Contract(
            std::optional<std::string> number,
            Date date,
            std::optional<std::string> name_organization,
            std::optional<std::string> name_short,
            std::optional<std::string> name_full,
            Date date_deadline,
            std::optional<std::string> name_responsible_employee,
            Price price,
            Price price_other_department,
            bool with_nds,
            int stavka_nds,
            TypeContract type,
            bool with_stage,
            std::vector<SeparateWork> pool_work,
            std::optional<std::string> info,
            std::optional<std::vector<Stage>> pool_stage,
            bool is_complet,
            bool is_paid,
            std::optional<std::vector<Payment>> payments,
            std::optional<std::vector<Expenses>> expenses,
            std::string status_payment
            )
            : number_(std::move(number))
            , date_(date)
            , name_organization_(std::move(name_organization))
            , name_short_(std::move(name_short))
            , name_full_(std::move(name_full))
            , date_deadline_(date_deadline)
            , name_responsible_employee_(std::move(name_responsible_employee))
            , price_(price)
            , price_other_department_(price_other_department)
            , with_nds_(with_nds)
            , stavka_nds_(stavka_nds)
            , type_(type)
            , with_stage_(with_stage)
            , pool_work(std::move(pool_work))
            , info_(std::move(info))
            , pool_stage_(std::move(pool_stage))
            , is_complet_(is_complet)
            , is_paid_(is_paid)
            , payments_(std::move(payments))
            , expenses_(std::move(expenses))
            , status_payment_(std::move(status_payment)) {
        }

        // Разрешаем перемещение
        Contract(Contract&& other) noexcept
            : number_(std::move(other.number_))
            , date_(other.date_)
            , name_organization_(std::move(other.name_organization_))
            , name_short_(std::move(other.name_short_))
            , name_full_(std::move(other.name_full_))
            , date_deadline_(other.date_deadline_)
            , name_responsible_employee_(std::move(other.name_responsible_employee_))
            , price_(other.price_)
            , price_other_department_(other.price_other_department_)
            , with_nds_(other.with_nds_)
            , stavka_nds_(other.stavka_nds_)
            , type_(other.type_)
            , with_stage_(other.with_stage_)
            , pool_work(std::move(other.pool_work))
            , id_(other.id_)
            , info_(std::move(other.info_))
            , pool_stage_(std::move(other.pool_stage_))
            , is_complet_(other.is_complet_)
            , is_paid_(other.is_paid_)
            , payments_(std::move(other.payments_))
            , expenses_(std::move(other.expenses_))
            , status_payment_(std::move(other.status_payment_)) {
        }

        Contract& operator=(Contract&& other) noexcept {
            if (this != &other) {
                number_ = std::move(other.number_);
                date_ = other.date_;
                name_organization_ = std::move(other.name_organization_);
                name_short_ = std::move(other.name_short_);
                name_full_ = std::move(other.name_full_);
                date_deadline_ = other.date_deadline_;
                name_responsible_employee_ = std::move(other.name_responsible_employee_);
                price_ = other.price_;
                price_other_department_ = other.price_other_department_;
                with_nds_ = other.with_nds_;
                stavka_nds_ = other.stavka_nds_;
                type_ = other.type_;
                with_stage_ = other.with_stage_;
                pool_work = std::move(other.pool_work);
                id_ = other.id_;
                info_ = std::move(other.info_);
                pool_stage_ = std::move(other.pool_stage_);
                is_complet_ = other.is_complet_;
                is_paid_ = other.is_paid_;
                payments_ = std::move(other.payments_);
                expenses_ = std::move(other.expenses_);
                status_payment_ = std::move(other.status_payment_);
            }
            return *this;
        }

        // запрещаем использовать конструктор копирования и оператор присваивания
        Contract(const Contract&) = delete;
        Contract& operator=(const Contract&) = delete;

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
        std::optional<std::string> info_;

        std::optional<std::vector<Stage>> pool_stage_;
        bool is_complet_;
        bool is_paid_;
        std::optional<std::vector<Payment>> payments_;
        std::optional<std::vector<Expenses>> expenses_;
        std::string status_payment_;

        static int id_counter_;
    };

    class User {
    public:
        User(const std::string& login, const std::string& pass)
            : id_(++id_counter_)
            , login_(login)
            , pass_(pass) {
        }

        // Конструктор для восстановления из JSON
        User(int id, const std::string& login, const std::string& pass)
            : id_(id)
            , login_(login)
            , pass_(pass) {
            ++id_counter_;
        }

        // Конструктор перемещения
        User(User&& other) noexcept
            : id_(other.id_)
            , login_(std::move(other.login_))
            , pass_(std::move(other.pass_)) {
        }

        // запрещаем использовать конструктор копирования и оператор присваивания
        User(const User&) = delete;
        User& operator=(const User&) = delete;


        // очень не безопасно, но это временно для сериализации, потом подумать как избавиться или как минимуи хранить в зашифрованном виде
        const std::string& GetPass() const {
            return pass_;
        }

        const std::string& GetLogin() const {
            return login_;
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
