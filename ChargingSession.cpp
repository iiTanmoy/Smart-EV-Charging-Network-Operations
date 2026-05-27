#include "ChargingSession.h"
#include "Station.h"
#include "User.h"
#include <iostream>

using namespace std;

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
    if (getStation() == 0 || getUser() == 0) {
        return finalCost;
    }
    double actualMinutes = difftime(getEndTime(), getStartTime()) / 60.0;
    if (actualMinutes <= 0.0) {
        actualMinutes = getSlotDuration();
    }
    if (actualMinutes < 1.0) {
        actualMinutes = 1.0;
    }
    double pricePerMin = getStation()->calculatePricePerMin(getUser());
    double baseCost = pricePerMin * actualMinutes;
    double discount = getUser()->calculateDiscount(baseCost);
    double calculated = baseCost - discount;
    if (calculated <= 0.0) {
        return finalCost;
    }
    return calculated;
}

void ChargingSession::logSession(std::ostream& out) const {
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
