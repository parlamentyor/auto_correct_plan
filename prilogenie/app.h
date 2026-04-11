#ifndef APP_H
#define APP_H

#include"model.h"

#include<map>

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



    private:
        std::map<std::string, model::User> users_; // ключ имя; потом перейти на unordered_map; потом поменять на указатель на User; потом подтянуть базу данных с уже созданными users

        model::DocumentPlanMonth doc_plan_month_;
        std::string active_user_name_;
        std::vector<model::Contract> contracts_;
    };


}

#endif // APP_H
