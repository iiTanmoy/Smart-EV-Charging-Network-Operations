#include "RegularUser.h"
#include <iostream>
using namespace std;

RegularUser::RegularUser(const std::string& id, const std::string& name,
                         const std::string& contact, double walletBalance,
                         int totalSessions)
    : User(id, name, contact, walletBalance, totalSessions) {
}

RegularUser::~RegularUser() {
}

std::string RegularUser::getTier() const {
    return "Regular";
}

double RegularUser::calculateDiscount(double /* basePrice */) const {
    return 0.0;
}

void RegularUser::displayInfo() const {
    User::displayInfo();
}

void RegularUser::saveToCsv(std::ostream& out) const {
    User::saveToCsv(out);
}
