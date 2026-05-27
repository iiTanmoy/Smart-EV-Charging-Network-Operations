#include "ChargingSession.h"
#include "Station.h"
#include "User.h"
#include <iostream>

ChargingSession::ChargingSession(const std::string& bookingID, Station* station,
                                 User* user, int slotDuration, time_t startTime,
                                 time_t endTime, int status,
                                 double energyConsumed, double finalCost)
    : Booking(bookingID, station, user, slotDuration, startTime, endTime, status),
      energyConsumed(energyConsumed), finalCost(finalCost) {
}

ChargingSession::~ChargingSession() {
}

double ChargingSession::getEnergyConsumed() const {
    return energyConsumed;
}

double ChargingSession::getFinalCost() const {
    return finalCost;
}

double ChargingSession::calculateCost() const {
    double cost = Booking::calculateCost();
    return cost > 0.0 ? cost : finalCost;
}

void ChargingSession::logSession(std::ofstream& out) const {
    if (getStation() == 0 || getUser() == 0) {
        return;
    }
    out << "SESSION," << getBookingID() << ","
        << getUser()->getUserID() << ","
        << getStation()->getStationID() << ","
        << getStartTime() << ","
        << getEndTime() << ","
        << energyConsumed << ","
        << finalCost << ","
        << getUser()->getTier() << "\n";
}
