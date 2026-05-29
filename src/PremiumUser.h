#ifndef PREMIUMUSER_H
#define PREMIUMUSER_H

#include "User.h"

class PremiumUser : public User {
public:
    PremiumUser(const std::string& id, const std::string& name,
                const std::string& contact, double walletBalance,
                int totalSessions, int freeMinutes = 0);
    virtual ~PremiumUser();

    virtual std::string getTier() const;
    virtual double calculateDiscount(double basePrice) const;
    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;

private:
    int freeMinutes;
};

#endif // PREMIUMUSER_H
