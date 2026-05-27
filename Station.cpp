#include "Station.h"
#include "User.h"
#include <iostream>

Station::Station(const std::string& id, const std::string& locationName,
                 double latitude, double longitude, double powerRating,
                 int status, const std::string& supportedTiers)
    : stationID(id), locationName(locationName), latitude(latitude),
      longitude(longitude), powerRating(powerRating), status(status),
      supportedTiers(supportedTiers) {
}

Station::~Station() {
}

const std::string& Station::getStationID() const {
    return stationID;
}

const std::string& Station::getLocationName() const {
    return locationName;
}

double Station::getLatitude() const {
    return latitude;
}

double Station::getLongitude() const {
    return longitude;
}

double Station::getPowerRating() const {
    return powerRating;
}

int Station::getStatus() const {
    return status;
}

const std::string& Station::getSupportedTiers() const {
    return supportedTiers;
}

void Station::setStatus(int newStatus) {
    status = newStatus;
}

bool Station::isAvailable() const {
    return status == Available;
}

void Station::displayInfo() const {
    std::cout << "ID: " << stationID << " | "
              << "Location: " << locationName << " | "
              << "Type: " << "Station" << " | "
              << "Power: " << powerRating << " kW" << " | "
              << "Status: " << status << " | "
              << "Supported: " << supportedTiers << std::endl;
}

void Station::saveToCsv(std::ostream& out) const {
    out << stationID << "," << locationName << ","
        << latitude << "," << longitude << ","
        << powerRating << "," << status << ","
        << supportedTiers << "\n";
}
