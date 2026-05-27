#ifndef DCFASTSTATION_H
#define DCFASTSTATION_H

#include "Station.h"

class DCFastStation : public Station {
public:
    DCFastStation(const std::string& id, const std::string& locationName,
                  double latitude, double longitude, double powerRating,
                  int status, const std::string& supportedTiers,
                  double maxPower);
    virtual ~DCFastStation();

    double getMaxPower() const;
    virtual double calculatePricePerMin(const User* user) const;    virtual std::string getType() const;    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;

private:
    double maxPower;
};

#endif // DCFASTSTATION_H
