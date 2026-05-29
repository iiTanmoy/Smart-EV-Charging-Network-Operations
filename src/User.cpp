#include "User.h"
#include <iostream>
using namespace std;

User::User(const std::string& id, const std::string& name,
           const std::string& contact, double walletBalance,
           int totalSessions)
    : userID(id), name(name), contact(contact), walletBalance(walletBalance),
      totalSessions(totalSessions) {
}

User::~User() {
}

const std::string& User::getUserID() const {
    return userID;
}

const std::string& User::getName() const {
    return name;
}

const std::string& User::getContact() const {
    return contact;
}

double User::getWalletBalance() const {
    return walletBalance;
}

int User::getTotalSessions() const {
    return totalSessions;
}

void User::addSession() {
    totalSessions++;
}

void User::adjustWallet(double amount) {
    walletBalance += amount;
}

double User::calculateDiscount(double /* basePrice */) const {
    return 0.0;
}

void User::displayInfo() const {
    std::cout << "User ID: " << userID << " | "
              << "Name: " << name << " | "
              << "Contact: " << contact << " | "
              << "Tier: " << getTier() << " | "
              << "Wallet: " << walletBalance << " | "
              << "Sessions: " << totalSessions << std::endl;
}

void User::saveToCsv(std::ostream& out) const {
    out << userID << "," << name << "," << contact << ","
        << getTier() << "," << walletBalance << ","
        << totalSessions << "\n";
}
