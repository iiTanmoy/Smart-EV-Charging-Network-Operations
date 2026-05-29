#include "PremiumUser.h"
#include <iostream>
using namespace std;

PremiumUser::PremiumUser(const std::string& id, const std::string& name,
                         const std::string& contact, double walletBalance,
                         int totalSessions, int freeMinutes)
    : User(id, name, contact, walletBalance, totalSessions),
      freeMinutes(freeMinutes) {
}

PremiumUser::~PremiumUser() {
}

std::string PremiumUser::getTier() const {
    return "Premium";
}

double PremiumUser::calculateDiscount(double basePrice) const {
    return 0.0;
}

void PremiumUser::displayInfo() const {
    User::displayInfo();
    std::cout << "  Free minutes: " << freeMinutes << std::endl;
}

void PremiumUser::saveToCsv(std::ostream& out) const {
    out << userID << "," << name << "," << contact << ","
        << getTier() << "," << walletBalance << ","
        << totalSessions << "," << freeMinutes << "\n";
}
