#ifndef DCULTRAFASTSTATION_H
#define DCULTRAFASTSTATION_H

#include "Station.h"

class DCUltraFastStation : public Station {
public:
    DCUltraFastStation(const std::string& id, const std::string& locationName,
                       double latitude, double longitude, double powerRating,
                       int status, const std::string& supportedTiers,
                       double ultraPower);
    virtual ~DCUltraFastStation();

    double getUltraPower() const;
    virtual double calculatePricePerMin(const User* user) const;    virtual std::string getType() const;    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;

private:
    double ultraPower;
};

#endif // DCULTRAFASTSTATION_H
