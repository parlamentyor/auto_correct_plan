#include "model.h"

int model::User::id_counter_ = 0;
int model::Contract::id_counter_ = 0;
int model::Contractor::id_counter_ = 0;
int model::Customer::id_counter_ = 0;

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


model::Price& model::Price::operator+=(const Price &other_price) {
    ruble_ += other_price.ruble_;
    kop_ += other_price.kop_;

    // Простая нормализация
    if (kop_ >= 100) {
        ruble_ += kop_ / 100;
        kop_ %= 100;
    }
    return *this;
}

model::Price model::operator+(const Price &lhs, const Price &rhs) {
    model::Price result = lhs;
    result += rhs;
    return result;
}
