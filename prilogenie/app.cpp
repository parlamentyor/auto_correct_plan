#include "app.h"

bool app::App::AvailableUser(const std::string &login) const {
    return users_.find(login) != users_.end();
}

bool app::App::IsCorrectPass(const std::string &login, const std::string &pass) const {
    return users_.find(login)->second.GetPass() == pass;
}

void app::App::SetActivUserName(const std::string &login) {
    active_user_name_ = login;
}

const std::string& app::App::GetActivUserName() const {
    return active_user_name_;
}

void app::App::AddUser(const std::string &login, const std::string &pass) {
    model::User new_user(login, pass);
    users_.emplace(login, model::User(login, pass));
}

void app::App::SetPathPlanMonth(const std::string &s_path) {
    doc_plan_month_.SetPath(s_path);
}

std::string app::App::GetPathPlanMonth() const {
    return doc_plan_month_.GetPath();
}
