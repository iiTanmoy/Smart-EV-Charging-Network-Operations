#ifndef FLEETUSER_H
#define FLEETUSER_H

#include "User.h"

class FleetUser : public User {
public:
    FleetUser(const std::string& id, const std::string& name,
              const std::string& contact, double walletBalance,
              int totalSessions, const std::string& fleetID,
              const std::string& companyName);
    virtual ~FleetUser();

    virtual std::string getTier() const;
    virtual double calculateDiscount(double basePrice) const;
    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;

private:
    std::string fleetID;
    std::string companyName;
};

#endif // FLEETUSER_H
