#ifndef REGULARUSER_H
#define REGULARUSER_H

#include "User.h"

class RegularUser : public User {
public:
    RegularUser(const std::string& id, const std::string& name,
                const std::string& contact, double walletBalance,
                int totalSessions);
    virtual ~RegularUser();

    virtual std::string getTier() const;
    virtual double calculateDiscount(double basePrice) const;
    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;
};

#endif // REGULARUSER_H
