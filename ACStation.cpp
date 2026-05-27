#include "ACStation.h"
#include "User.h"
#include <iostream>
using namespace std;

ACStation::ACStation(const std::string& id, const std::string& locationName,
                     double latitude, double longitude, double powerRating,
                     int status, const std::string& supportedTiers,
                     int maxSlots)
    : Station(id, locationName, latitude, longitude, powerRating, status, supportedTiers),
      maxSlots(maxSlots) {
}

ACStation::~ACStation() {
}

int ACStation::getMaxSlots() const {
    return maxSlots;
}

double ACStation::calculatePricePerMin(const User* user) const {
    double baseRate = 0.15 * powerRating;
    double discount = 0.0;
    if (user != 0) {
        discount = user->calculateDiscount(baseRate);
    }
    return baseRate - discount;
}

std::string ACStation::getType() const {
    return "AC";
}

void ACStation::displayInfo() const {
    std::cout << "AC Station - " << stationID << " | "
              << locationName << " | "
              << "Power: " << powerRating << " kW" << " | "
              << "Slots: " << maxSlots << " | "
              << "Status: " << status << " | "
              << "Tiers: " << supportedTiers << std::endl;
}

void ACStation::saveToCsv(std::ostream& out) const {
    out << stationID << ",AC," << locationName << ","
        << latitude << "," << longitude << ","
        << powerRating << "," << status << ","
        << supportedTiers << "," << maxSlots << "\n";
}
