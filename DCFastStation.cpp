#include "DCFastStation.h"
#include "User.h"
#include <iostream>
using namespace std;

DCFastStation::DCFastStation(const std::string& id, const std::string& locationName,
                             double latitude, double longitude, double powerRating,
                             int status, const std::string& supportedTiers,
                             double maxPower)
    : Station(id, locationName, latitude, longitude, powerRating, status, supportedTiers),
      maxPower(maxPower) {
}

DCFastStation::~DCFastStation() {
}

double DCFastStation::getMaxPower() const {
    return maxPower;
}

double DCFastStation::calculatePricePerMin(const User* user) const {
    double baseRate = 0.30 * maxPower;
    double discount = 0.0;
    if (user != 0) {
        discount = user->calculateDiscount(baseRate);
    }
    return baseRate - discount;
}

std::string DCFastStation::getType() const {
    return "DCFast";
}

void DCFastStation::displayInfo() const {
    std::cout << "DC Fast Station - " << stationID << " | "
              << locationName << " | "
              << "Power: " << powerRating << " kW" << " | "
              << "Max Power: " << maxPower << " kW" << " | "
              << "Status: " << status << " | "
              << "Tiers: " << supportedTiers << std::endl;
}

void DCFastStation::saveToCsv(std::ostream& out) const {
    out << stationID << ",DCFast," << locationName << ","
        << latitude << "," << longitude << ","
        << powerRating << "," << status << ","
        << supportedTiers << "," << maxPower << "\n";
}
