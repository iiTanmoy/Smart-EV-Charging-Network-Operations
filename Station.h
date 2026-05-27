#ifndef STATION_H
#define STATION_H

#include <string>
#include <fstream>

class User;

class Station {
public:
    enum Status {
        Available = 0,
        Occupied = 1,
        Faulty = 2,
        Maintenance = 3
    };

    Station(const std::string& id, const std::string& locationName,
            double latitude, double longitude, double powerRating,
            int status, const std::string& supportedTiers);
    virtual ~Station();

    const std::string& getStationID() const;
    const std::string& getLocationName() const;
    double getLatitude() const;
    double getLongitude() const;
    double getPowerRating() const;
    int getStatus() const;
    const std::string& getSupportedTiers() const;

    void setStatus(int newStatus);
    bool isAvailable() const;

    virtual double calculatePricePerMin(const User* user) const = 0;
    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;

protected:
    std::string stationID;
    std::string locationName;
    double latitude;
    double longitude;
    double powerRating;
    int status;
    std::string supportedTiers;
};

#endif // STATION_H
