#include "app.h"

bool app::App::AvailableUser(const std::string &login) const {
    return users_.find(login) != users_.end();
}

bool app::App::IsCorrectPass(const std::string &login, const std::string &pass) const {
    return users_.find(login)->second.GetPass() == pass;
}

void app::App::SetActivUserId(const std::string &login) {
    active_user_id_ = users_.find(login)->second.GetId();
}

void app::App::AddUser(const std::string &login, const std::string &pass)
{
    model::User new_user(login, pass);
    users_.emplace(login, model::User(login, pass));
}
