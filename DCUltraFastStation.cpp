#include "DCUltraFastStation.h"
#include "User.h"
#include <iostream>

DCUltraFastStation::DCUltraFastStation(const std::string& id,
                                       const std::string& locationName,
                                       double latitude, double longitude,
                                       double powerRating, int status,
                                       const std::string& supportedTiers,
                                       double ultraPower)
    : Station(id, locationName, latitude, longitude, powerRating, status, supportedTiers),
      ultraPower(ultraPower) {
}

DCUltraFastStation::~DCUltraFastStation() {
}

double DCUltraFastStation::getUltraPower() const {
    return ultraPower;
}

double DCUltraFastStation::calculatePricePerMin(const User* user) const {
    double baseRate = 0.45 * ultraPower;
    double discount = 0.0;
    if (user != 0) {
        discount = user->calculateDiscount(baseRate);
    }
    return baseRate - discount;
}

void DCUltraFastStation::displayInfo() const {
    std::cout << "DC Ultra Fast Station - " << stationID << " | "
              << locationName << " | "
              << "Power: " << powerRating << " kW" << " | "
              << "Ultra: " << ultraPower << " kW" << " | "
              << "Status: " << status << " | "
              << "Tiers: " << supportedTiers << std::endl;
}

void DCUltraFastStation::saveToCsv(std::ostream& out) const {
    out << stationID << ",DCUltraFast," << locationName << ","
        << latitude << "," << longitude << ","
        << powerRating << "," << status << ","
        << supportedTiers << "," << ultraPower << "\n";
}
