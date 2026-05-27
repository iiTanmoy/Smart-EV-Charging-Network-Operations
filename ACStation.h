#ifndef ACSTATION_H
#define ACSTATION_H

#include "Station.h"

class ACStation : public Station {
public:
    ACStation(const std::string& id, const std::string& locationName,
              double latitude, double longitude, double powerRating,
              int status, const std::string& supportedTiers, int maxSlots);
    virtual ~ACStation();

    int getMaxSlots() const;
    virtual double calculatePricePerMin(const User* user) const;    virtual std::string getType() const;    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;

private:
    int maxSlots;
};

#endif // ACSTATION_H
