#include "FleetUser.h"
#include <iostream>
using namespace std;

FleetUser::FleetUser(const std::string& id, const std::string& name,
                     const std::string& contact, double walletBalance,
                     int totalSessions, const std::string& fleetID,
                     const std::string& companyName)
    : User(id, name, contact, walletBalance, totalSessions),
      fleetID(fleetID), companyName(companyName) {
}

FleetUser::~FleetUser() {
}

std::string FleetUser::getTier() const {
    return "Fleet";
}

double FleetUser::calculateDiscount(double basePrice) const {
    return basePrice * 0.15;
}

void FleetUser::displayInfo() const {
    User::displayInfo();
    std::cout << "  Fleet ID: " << fleetID << " | "
              << "Company: " << companyName << std::endl;
}

void FleetUser::saveToCsv(std::ostream& out) const {
    out << userID << "," << name << "," << contact << ","
        << getTier() << "," << walletBalance << ","
        << totalSessions << "," << fleetID << "," << companyName << "\n";
}
