#ifndef USER_H
#define USER_H

#include <string>
#include <fstream>

class User {
public:
    User(const std::string& id, const std::string& name,
         const std::string& contact, double walletBalance,
         int totalSessions);
    virtual ~User();

    const std::string& getUserID() const;
    const std::string& getName() const;
    const std::string& getContact() const;
    double getWalletBalance() const;
    int getTotalSessions() const;

    void addSession();
    void adjustWallet(double amount);

    virtual std::string getTier() const = 0;
    virtual double calculateDiscount(double basePrice) const;
    virtual void displayInfo() const;
    virtual void saveToCsv(std::ostream& out) const;

protected:
    std::string userID;
    std::string name;
    std::string contact;
    double walletBalance;
    int totalSessions;
};

#endif // USER_H
