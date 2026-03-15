#include "model.h"

int model::User::id_counter_ = 0;
int model::Contract::id_counter_ = 0;


void model::DocumentImpl::SetPath(std::string s_path)
{
    path_ = s_path;
}

std::string model::DocumentImpl::GetPath() const
{
//    return path_.string(); // это если у нас path_ является объектом fs::path
    return path_.value();
}

bool model::DocumentImpl::HasValuePath() const
{
    return path_.has_value();
}
