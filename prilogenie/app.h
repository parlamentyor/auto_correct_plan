#ifndef APP_H
#define APP_H

#include"model.h"

#include <map>
#include <set>

namespace app {

    class App {
    public:
        bool AvailableUser(const std::string& login) const;
        bool IsCorrectPass(const std::string& login, const std::string& pass) const;
        void SetActivUserName(const std::string &login);
        const std::string& GetActivUserName() const;
        void AddUser(const std::string& login, const std::string& pass);
        const std::map<std::string, model::User>& GetUsers() const;
        void AddContract(model::Contract&& contract);
        const std::vector<model::Contract>& GetContracts() const;

        void SetPathPlanMonth(const std::string& s_path);
        std::string GetPathPlanMonth() const;
        bool HasValuePathPlanMonth() const;

        void AddBaseWork(const std::string& new_work);
        const std::set<std::string>& GetBaseWork() const;

        void AddBaseEmployee(const std::string& new_employee);
        const std::set<std::string>& GetBaseEmployee() const;

        void AddBaseExpenses(const std::string& new_expenses);
        const std::set<std::string>& GetBaseExpenses() const;

    private:
        std::map<std::string, model::User> users_; // ключ имя; потом перейти на unordered_map; потом поменять на указатель на User; потом подтянуть базу данных с уже созданными users
        model::DocumentPlanMonth doc_plan_month_;
        std::string active_user_name_;
        std::vector<model::Contract> contracts_;

        std::set<std::string> base_works_;
        std::set<std::string> base_employees_;
        std::set<std::string> base_expenses_;
    };


}

#endif // APP_H
