#ifndef CHARGINGSESSION_H
#define CHARGINGSESSION_H

#include "Booking.h"

class ChargingSession : public Booking {
public:
    ChargingSession(const std::string& bookingID, Station* station, User* user,
                    int slotDuration, time_t startTime, time_t endTime,
                    int status, double energyConsumed, double finalCost);
    virtual ~ChargingSession();

    double getEnergyConsumed() const;
    double getFinalCost() const;

    virtual double calculateCost() const;
    void logSession(std::ofstream& out) const;

private:
    double energyConsumed;
    double finalCost;
};

#endif // CHARGINGSESSION_H
