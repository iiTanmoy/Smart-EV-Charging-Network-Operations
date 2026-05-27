#include "EVChargingManager.h"
#include "ACStation.h"
#include "DCFastStation.h"
#include "DCUltraFastStation.h"
#include "RegularUser.h"
#include "PremiumUser.h"
#include "FleetUser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>

EVChargingManager::EVChargingManager()
    : stationsFile("stations.csv"), usersFile("users.csv"),
      bookingsFile("bookings.csv"), sessionsFile("sessions.log"),
      backupFile("backup.dat") {
}

EVChargingManager::~EVChargingManager() {
    for (std::map<std::string, Station*>::iterator it = stations.begin(); it != stations.end(); ++it) {
        delete it->second;
    }
    for (std::map<std::string, User*>::iterator it = users.begin(); it != users.end(); ++it) {
        delete it->second;
    }
    for (std::vector<Booking*>::iterator it = bookings.begin(); it != bookings.end(); ++it) {
        delete *it;
    }
}

bool EVChargingManager::loadData() {
    loadStations();
    loadUsers();
    loadBookings();
    loadSessionLog();
    return true;
}

std::string EVChargingManager::readLine(const char* prompt) const {
    std::string value;
    std::cout << prompt;
    if (!std::getline(std::cin, value)) {
        std::cin.clear();
    }
    return value;
}

int EVChargingManager::readInt(const char* prompt, int min, int max) const {
    std::string line;
    int value = min;
    while (true) {
        line = readLine(prompt);
        std::istringstream ss(line);
        if (ss >> value && value >= min && value <= max) {
            return value;
        }
        std::cout << "Invalid input" << std::endl;
    }
}

double EVChargingManager::readDouble(const char* prompt, double min, double max) const {
    std::string line;
    double value = min;
    while (true) {
        line = readLine(prompt);
        std::istringstream ss(line);
        if (ss >> value && value >= min && value <= max) {
            return value;
        }
        std::cout << "Invalid input" << std::endl;
    }
}

void EVChargingManager::pause() const {
    std::cout << "Press enter";
    std::cin.ignore(1000, '\n');
}

bool EVChargingManager::loadFile(const std::string& fileName,
                                 std::vector<std::string>& lines) const {
    std::ifstream in(fileName.c_str());
    if (!in.is_open()) {
        return false;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    in.close();
    return true;
}

bool EVChargingManager::saveFile(const std::string& fileName,
                                 const std::vector<std::string>& lines) const {
    std::string tempName = fileName + ".tmp";
    std::ofstream out(tempName.c_str());
    if (!out.is_open()) {
        return false;
    }
    for (std::vector<std::string>::const_iterator it = lines.begin(); it != lines.end(); ++it) {
        out << *it << "\n";
    }
    out.close();
    std::remove(fileName.c_str());
    std::rename(tempName.c_str(), fileName.c_str());
    return true;
}

Station* EVChargingManager::createStation(const std::string& type,
                                          const std::string& id,
                                          const std::string& name,
                                          double latitude,
                                          double longitude,
                                          double powerRating,
                                          int status,
                                          const std::string& supportedTiers,
                                          double extraValue) {
    if (type == "AC") {
        return new ACStation(id, name, latitude, longitude, powerRating, status, supportedTiers, (int)extraValue);
    }
    if (type == "DCFast") {
        return new DCFastStation(id, name, latitude, longitude, powerRating, status, supportedTiers, extraValue);
    }
    if (type == "DCUltra") {
        return new DCUltraFastStation(id, name, latitude, longitude, powerRating, status, supportedTiers, extraValue);
    }
    return 0;
}

User* EVChargingManager::createUser(const std::string& id,
                                    const std::string& name,
                                    const std::string& contact,
                                    const std::string& tier,
                                    double walletBalance,
                                    int totalSessions) {
    if (tier == "Regular") {
        return new RegularUser(id, name, contact, walletBalance, totalSessions);
    }
    if (tier == "Premium") {
        return new PremiumUser(id, name, contact, walletBalance, totalSessions);
    }
    if (tier == "Fleet") {
        return new FleetUser(id, name, contact, walletBalance, totalSessions, "", "");
    }
    return 0;
}

Booking* EVChargingManager::createBooking(const std::string& bookingID,
                                          Station* station,
                                          User* user,
                                          int slotDuration,
                                          time_t startTime,
                                          time_t endTime,
                                          int status) {
    return new Booking(bookingID, station, user, slotDuration, startTime, endTime, status);
}

void EVChargingManager::loadStations() {
    std::vector<std::string> lines;
    if (!loadFile(stationsFile, lines)) {
        return;
    }
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        std::string line = *it;
        std::vector<std::string> parts;
        std::string token;
        std::istringstream ss(line);
        while (std::getline(ss, token, ',')) {
            parts.push_back(token);
        }
        if (parts.size() < 8) {
            continue;
        }
        std::string id = parts[0];
        std::string type = parts[1];
        std::string name = parts[2];
        double lat = atof(parts[3].c_str());
        double lon = atof(parts[4].c_str());
        double power = atof(parts[5].c_str());
        int status = atoi(parts[6].c_str());
        std::string supported = parts[7];
        double extra = 0.0;
        if (parts.size() > 8) {
            extra = atof(parts[8].c_str());
        }
        Station* station = createStation(type, id, name, lat, lon, power, status, supported, extra);
        if (station != 0) {
            stations[id] = station;
        }
    }
}

void EVChargingManager::loadUsers() {
    std::vector<std::string> lines;
    if (!loadFile(usersFile, lines)) {
        return;
    }
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        std::string line = *it;
        std::vector<std::string> parts;
        std::string token;
        std::istringstream ss(line);
        while (std::getline(ss, token, ',')) {
            parts.push_back(token);
        }
        if (parts.size() < 6) {
            continue;
        }
        std::string id = parts[0];
        std::string name = parts[1];
        std::string contact = parts[2];
        std::string tier = parts[3];
        double wallet = atof(parts[4].c_str());
        int total = atoi(parts[5].c_str());
        User* user = createUser(id, name, contact, tier, wallet, total);
        if (user != 0) {
            users[id] = user;
        }
    }
}

void EVChargingManager::loadBookings() {
    std::vector<std::string> lines;
    if (!loadFile(bookingsFile, lines)) {
        return;
    }
    for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        std::string line = *it;
        std::vector<std::string> parts;
        std::string token;
        std::istringstream ss(line);
        while (std::getline(ss, token, ',')) {
            parts.push_back(token);
        }
        if (parts.size() < 7) {
            continue;
        }
        std::string bid = parts[0];
        std::string uid = parts[1];
        std::string sid = parts[2];
        int duration = atoi(parts[3].c_str());
        time_t start = (time_t)atol(parts[4].c_str());
        time_t end = (time_t)atol(parts[5].c_str());
        int status = atoi(parts[6].c_str());
        Station* station = findStation(sid);
        User* user = findUser(uid);
        Booking* booking = createBooking(bid, station, user, duration, start, end, status);
        if (booking != 0) {
            bookings.push_back(booking);
            if (booking->getStatus() == Booking::Active) {
                activeBookings.push(booking);
            }
        }
    }
}

void EVChargingManager::loadSessionLog() {
}

void EVChargingManager::saveStations() {
    std::vector<std::string> lines;
    for (std::map<std::string, Station*>::iterator it = stations.begin(); it != stations.end(); ++it) {
        std::ostringstream ss;
        it->second->saveToCsv(ss);
        lines.push_back(ss.str());
    }
    saveFile(stationsFile, lines);
}

void EVChargingManager::saveUsers() {
    std::vector<std::string> lines;
    for (std::map<std::string, User*>::iterator it = users.begin(); it != users.end(); ++it) {
        std::ostringstream ss;
        it->second->saveToCsv(ss);
        lines.push_back(ss.str());
    }
    saveFile(usersFile, lines);
}

void EVChargingManager::saveBookings() {
    std::vector<std::string> lines;
    for (std::vector<Booking*>::iterator it = bookings.begin(); it != bookings.end(); ++it) {
        std::ostringstream ss;
        (*it)->saveToCsv(ss);
        lines.push_back(ss.str());
    }
    saveFile(bookingsFile, lines);
}

void EVChargingManager::saveAll() {
    saveStations();
    saveUsers();
    saveBookings();
}

void EVChargingManager::logSession(const ChargingSession& session) {
    std::ofstream out(sessionsFile.c_str(), std::ios::app);
    if (!out.is_open()) {
        return;
    }
    session.logSession(out);
    out.close();
}

void EVChargingManager::backupSystem() const {
    std::ofstream out(backupFile.c_str(), std::ios::binary);
    if (!out.is_open()) {
        return;
    }
    out.close();
}

void EVChargingManager::restoreSystem() {
}

Station* EVChargingManager::findStation(const std::string& stationID) const {
    std::map<std::string, Station*>::const_iterator it = stations.find(stationID);
    return it != stations.end() ? it->second : 0;
}

User* EVChargingManager::findUser(const std::string& userID) const {
    std::map<std::string, User*>::const_iterator it = users.find(userID);
    return it != users.end() ? it->second : 0;
}

Booking* EVChargingManager::findBooking(const std::string& bookingID) const {
    for (std::vector<Booking*>::const_iterator it = bookings.begin(); it != bookings.end(); ++it) {
        if ((*it)->getBookingID() == bookingID) {
            return *it;
        }
    }
    return 0;
}

void EVChargingManager::mainMenu() {
    while (true) {
        std::cout << "============================================================\n";
        std::cout << "     SMART EV CHARGING NETWORK OPERATIONS PLATFORM\n";
        std::cout << "                ChargeGrid Mobility Pvt. Ltd.\n";
        std::cout << "============================================================\n";
        std::cout << "1. Admin / Operator Portal\n";
        std::cout << "2. User Simulation Portal\n";
        std::cout << "3. Analytics & Reports\n";
        std::cout << "4. System Backup & Restore\n";
        std::cout << "5. Exit\n";
        int choice = readInt("Enter your choice (1-5): ", 1, 5);
        if (choice == 1) {
            adminMenu();
        } else if (choice == 2) {
            userPortal();
        } else if (choice == 3) {
            analyticsMenu();
        } else if (choice == 4) {
            backupMenu();
        } else {
            break;
        }
    }
}

void EVChargingManager::adminMenu() {
    while (true) {
        std::cout << "1. Station Management\n";
        std::cout << "2. User Management\n";
        std::cout << "3. View Real-time Station Status\n";
        std::cout << "4. Manage Active Bookings\n";
        std::cout << "5. Back to Main Menu\n";
        int choice = readInt("Enter your choice: ", 1, 5);
        if (choice == 1) {
            stationManagementMenu();
        } else if (choice == 2) {
            userManagementMenu();
        } else if (choice == 3) {
            listAllStations();
            pause();
        } else if (choice == 4) {
            activeBookingMenu();
        } else {
            break;
        }
    }
}

void EVChargingManager::stationManagementMenu() {
    while (true) {
        std::cout << "1. Add New Charging Station\n";
        std::cout << "2. Remove Station\n";
        std::cout << "3. Update Station Status\n";
        std::cout << "4. List All Stations\n";
        std::cout << "5. Search Station by ID\n";
        std::cout << "6. Back to Admin Menu\n";
        int choice = readInt("Enter your choice: ", 1, 6);
        if (choice == 1) {
            addStation();
        } else if (choice == 2) {
            removeStation();
        } else if (choice == 3) {
            updateStationStatus();
        } else if (choice == 4) {
            listAllStations();
            pause();
        } else if (choice == 5) {
            searchStationById();
            pause();
        } else {
            break;
        }
    }
}

void EVChargingManager::userManagementMenu() {
    while (true) {
        std::cout << "1. Add New User (Register)\n";
        std::cout << "2. Remove User\n";
        std::cout << "3. List All Users\n";
        std::cout << "4. Search User by ID\n";
        std::cout << "5. View User Details\n";
        std::cout << "6. Back to Admin Menu\n";
        int choice = readInt("Enter your choice: ", 1, 6);
        if (choice == 1) {
            addUser();
        } else if (choice == 2) {
            removeUser();
        } else if (choice == 3) {
            listAllUsers();
            pause();
        } else if (choice == 4) {
            searchUserById();
            pause();
        } else if (choice == 5) {
            viewUserDetails();
            pause();
        } else {
            break;
        }
    }
}

void EVChargingManager::activeBookingMenu() {
    while (true) {
        std::cout << "1. View All Active Bookings\n";
        std::cout << "2. Cancel Booking (Admin)\n";
        std::cout << "3. Force End Session\n";
        std::cout << "4. Back to Admin Menu\n";
        int choice = readInt("Enter your choice: ", 1, 4);
        if (choice == 1) {
            listAllBookings();
            pause();
        } else if (choice == 2) {
            std::string id = readLine("Booking ID: ");
            Booking* booking = findBooking(id);
            if (booking != 0) {
                booking->endSession(std::time(0));
                saveBookings();
            }
        } else if (choice == 3) {
            std::string id = readLine("Booking ID: ");
            Booking* booking = findBooking(id);
            if (booking != 0 && booking->isActive()) {
                booking->endSession(std::time(0));
                saveBookings();
            }
        } else {
            break;
        }
    }
}

void EVChargingManager::userPortal() {
    while (true) {
        std::cout << "1. Register New User\n";
        std::cout << "2. Login (Simulate User Session)\n";
        std::cout << "3. Search & Book Charging Slot\n";
        std::cout << "4. View My Bookings & History\n";
        std::cout << "5. Cancel My Booking\n";
        std::cout << "6. Back to Main Menu\n";
        int choice = readInt("Enter your choice: ", 1, 6);
        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            loginUser();
        } else if (choice == 3) {
            std::string id = readLine("User ID: ");
            searchAndBook(id);
        } else if (choice == 4) {
            std::string id = readLine("User ID: ");
            viewMyBookings(id);
            pause();
        } else if (choice == 5) {
            std::string id = readLine("User ID: ");
            cancelBookingForUser(id);
        } else {
            break;
        }
    }
}

void EVChargingManager::analyticsMenu() {
    while (true) {
        std::cout << "1. Revenue Summary Report (Current Month)\n";
        std::cout << "2. Station Utilization Report\n";
        std::cout << "3. Top 10 Highest Revenue Stations\n";
        std::cout << "4. List Inactive Users (No session > 30 days)\n";
        std::cout << "5. Peak Hour Utilization Analysis\n";
        std::cout << "6. Export Report to File\n";
        std::cout << "7. Back to Main Menu\n";
        int choice = readInt("Enter your choice: ", 1, 7);
        if (choice == 1) {
            revenueSummary();
            pause();
        } else if (choice == 2) {
            utilizationReport();
            pause();
        } else if (choice == 3) {
            topRevenueStations();
            pause();
        } else if (choice == 4) {
            inactiveUsers();
            pause();
        } else if (choice == 5) {
            peakHourAnalysis();
            pause();
        } else if (choice == 6) {
            exportReports();
            pause();
        } else {
            break;
        }
    }
}

void EVChargingManager::backupMenu() {
    while (true) {
        std::cout << "1. Create Full System Backup\n";
        std::cout << "2. Restore System from Backup\n";
        std::cout << "3. Export All Stations to CSV\n";
        std::cout << "4. Export All Users to CSV\n";
        std::cout << "5. Export All Sessions Log\n";
        std::cout << "6. Back to Main Menu\n";
        int choice = readInt("Enter your choice: ", 1, 6);
        if (choice == 1) {
            backupSystem();
        } else if (choice == 2) {
            restoreSystem();
        } else if (choice == 3) {
            exportStations();
        } else if (choice == 4) {
            exportUsers();
        } else if (choice == 5) {
            exportSessions();
        } else {
            break;
        }
    }
}

void EVChargingManager::listAllStations() const {
    for (std::map<std::string, Station*>::const_iterator it = stations.begin(); it != stations.end(); ++it) {
        it->second->displayInfo();
    }
}

void EVChargingManager::listAllUsers() const {
    for (std::map<std::string, User*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        it->second->displayInfo();
    }
}

void EVChargingManager::listAllBookings() const {
    for (std::vector<Booking*>::const_iterator it = bookings.begin(); it != bookings.end(); ++it) {
        std::cout << "Booking ID: " << (*it)->getBookingID() << " | User: "
                  << ((*it)->getUser() ? (*it)->getUser()->getUserID() : "")
                  << " | Station: "
                  << ( (*it)->getStation() ? (*it)->getStation()->getStationID() : "" )
                  << " | Status: " << (*it)->getStatus() << std::endl;
    }
}

void EVChargingManager::addStation() {
    std::string id = readLine("Station ID: ");
    std::string type = readLine("Type (AC/DCFast/DCUltra): ");
    std::string name = readLine("Location name: ");
    double lat = readDouble("Latitude: ", -90.0, 90.0);
    double lon = readDouble("Longitude: ", -180.0, 180.0);
    double power = readDouble("Power rating: ", 0.0, 1000.0);
    int status = readInt("Status (0=Available,1=Occupied,2=Faulty,3=Maintenance): ", 0, 3);
    std::string tiers = readLine("Supported tiers: ");
    double extra = 0.0;
    if (type == "AC") {
        extra = readDouble("Max slots: ", 1.0, 100.0);
    } else {
        extra = readDouble("Max power: ", 1.0, 1000.0);
    }
    Station* station = createStation(type, id, name, lat, lon, power, status, tiers, extra);
    if (station != 0) {
        stations[id] = station;
        saveStations();
    }
}

void EVChargingManager::removeStation() {
    std::string id = readLine("Station ID: ");
    std::map<std::string, Station*>::iterator it = stations.find(id);
    if (it != stations.end()) {
        delete it->second;
        stations.erase(it);
        saveStations();
    }
}

void EVChargingManager::updateStationStatus() {
    std::string id = readLine("Station ID: ");
    Station* station = findStation(id);
    if (station != 0) {
        int status = readInt("Status (0=Available,1=Occupied,2=Faulty,3=Maintenance): ", 0, 3);
        station->setStatus(status);
        saveStations();
    }
}

void EVChargingManager::searchStationById() const {
    std::string id = readLine("Station ID: ");
    Station* station = findStation(id);
    if (station != 0) {
        station->displayInfo();
    }
}

void EVChargingManager::addUser() {
    std::string id = readLine("User ID: ");
    std::string name = readLine("Name: ");
    std::string contact = readLine("Contact: ");
    std::string tier = readLine("Tier (Regular/Premium/Fleet): ");
    double wallet = readDouble("Wallet balance: ", 0.0, 1000000.0);
    int total = readInt("Total sessions: ", 0, 100000);
    User* user = createUser(id, name, contact, tier, wallet, total);
    if (user != 0) {
        users[id] = user;
        saveUsers();
    }
}

void EVChargingManager::removeUser() {
    std::string id = readLine("User ID: ");
    std::map<std::string, User*>::iterator it = users.find(id);
    if (it != users.end()) {
        delete it->second;
        users.erase(it);
        saveUsers();
    }
}

void EVChargingManager::searchUserById() const {
    std::string id = readLine("User ID: ");
    User* user = findUser(id);
    if (user != 0) {
        user->displayInfo();
    }
}

void EVChargingManager::viewUserDetails() const {
    std::string id = readLine("User ID: ");
    User* user = findUser(id);
    if (user != 0) {
        user->displayInfo();
    }
}

void EVChargingManager::registerUser() {
    addUser();
}

void EVChargingManager::loginUser() {
    std::string id = readLine("User ID: ");
    User* user = findUser(id);
    if (user == 0) {
        return;
    }
    while (true) {
        std::cout << "1. Search Available Stations (with Recommendation)\n";
        std::cout << "2. Book a Charging Slot\n";
        std::cout << "3. View My Active Booking\n";
        std::cout << "4. Start Charging Session (Simulate)\n";
        std::cout << "5. End Charging Session\n";
        std::cout << "6. View Charging History\n";
        std::cout << "7. Logout\n";
        int choice = readInt("Enter your choice: ", 1, 7);
        if (choice == 1) {
            availableStationsForUser(id);
            pause();
        } else if (choice == 2) {
            searchAndBook(id);
        } else if (choice == 3) {
            viewMyBookings(id);
            pause();
        } else if (choice == 4) {
            startSessionForUser(id);
        } else if (choice == 5) {
            endSessionForUser(id);
        } else if (choice == 6) {
            viewMyBookings(id);
            pause();
        } else {
            break;
        }
    }
}

void EVChargingManager::searchAndBook(const std::string& userID) {
    User* user = findUser(userID);
    if (user == 0) {
        return;
    }
    std::string type = readLine("Station type (AC/DCFast/DCUltra): ");
    std::vector<Station*> available;
    for (std::map<std::string, Station*>::iterator it = stations.begin(); it != stations.end(); ++it) {
        Station* station = it->second;
        if (station->isAvailable()) {
            available.push_back(station);
        }
    }
    if (available.empty()) {
        return;
    }
    std::cout << "Available stations:\n";
    for (std::vector<Station*>::iterator it = available.begin(); it != available.end(); ++it) {
        (*it)->displayInfo();
    }
    std::string sid = readLine("Station ID: ");
    Station* station = findStation(sid);
    if (station == 0) {
        return;
    }
    int minutes = readInt("Duration minutes: ", 1, 240);
    std::string bid = generateBookingId();
    time_t now = std::time(0);
    Booking* booking = createBooking(bid, station, user, minutes, now, now + minutes*60, Booking::Booked);
    if (booking != 0) {
        bookings.push_back(booking);
        saveBookings();
    }
}

void EVChargingManager::viewMyBookings(const std::string& userID) const {
    for (std::vector<Booking*>::const_iterator it = bookings.begin(); it != bookings.end(); ++it) {
        if ((*it)->getUser() != 0 && (*it)->getUser()->getUserID() == userID) {
            std::cout << "Booking: " << (*it)->getBookingID() << " | Station: "
                      << ((*it)->getStation() ? (*it)->getStation()->getStationID() : "")
                      << " | Status: " << (*it)->getStatus() << std::endl;
        }
    }
}

void EVChargingManager::cancelBookingForUser(const std::string& userID) {
    std::string bid = readLine("Booking ID: ");
    Booking* booking = findBooking(bid);
    if (booking != 0 && booking->getUser() != 0 && booking->getUser()->getUserID() == userID) {
        booking->endSession(std::time(0));
        saveBookings();
    }
}

void EVChargingManager::startSessionForUser(const std::string& userID) {
    std::string bid = readLine("Booking ID: ");
    Booking* booking = findBooking(bid);
    if (booking != 0 && booking->getUser() != 0 && booking->getUser()->getUserID() == userID) {
        booking->startSession(std::time(0));
        saveBookings();
    }
}

void EVChargingManager::endSessionForUser(const std::string& userID) {
    std::string bid = readLine("Booking ID: ");
    Booking* booking = findBooking(bid);
    if (booking != 0 && booking->getUser() != 0 && booking->getUser()->getUserID() == userID && booking->isActive()) {
        booking->endSession(std::time(0));
        ChargingSession session(booking->getBookingID(), booking->getStation(), booking->getUser(), booking->getSlotDuration(), booking->getStartTime(), booking->getEndTime(), booking->getStatus(), 0.0, booking->calculateCost());
        completedSessions.push_back(session);
        logSession(session);
        saveBookings();
    }
}

void EVChargingManager::availableStationsForUser(const std::string& userID) const {
    for (std::map<std::string, Station*>::const_iterator it = stations.begin(); it != stations.end(); ++it) {
        if (it->second->isAvailable()) {
            it->second->displayInfo();
        }
    }
}

void EVChargingManager::revenueSummary() const {
    double total = 0.0;
    for (std::vector<ChargingSession>::const_iterator it = completedSessions.begin(); it != completedSessions.end(); ++it) {
        total += it->getFinalCost();
    }
    std::cout << "Revenue: " << total << std::endl;
}

void EVChargingManager::utilizationReport() const {
    int total = stations.size();
    int occupied = 0;
    for (std::map<std::string, Station*>::const_iterator it = stations.begin(); it != stations.end(); ++it) {
        if (it->second->getStatus() != Station::Available) {
            occupied++;
        }
    }
    std::cout << "Utilization: " << occupied << " / " << total << std::endl;
}

void EVChargingManager::topRevenueStations() const {
    std::cout << "Top revenue stations not implemented" << std::endl;
}

void EVChargingManager::inactiveUsers() const {
    std::cout << "Inactive user report not implemented" << std::endl;
}

void EVChargingManager::peakHourAnalysis() const {
    std::cout << "Peak hour analysis not implemented" << std::endl;
}

void EVChargingManager::exportReports() const {
    std::cout << "Export reports not implemented" << std::endl;
}

void EVChargingManager::exportStations() const {
    std::cout << "Export stations not implemented" << std::endl;
}

void EVChargingManager::exportUsers() const {
    std::cout << "Export users not implemented" << std::endl;
}

void EVChargingManager::exportSessions() const {
    std::cout << "Export sessions not implemented" << std::endl;
}

std::string EVChargingManager::generateBookingId() const {
    std::ostringstream ss;
    ss << "B" << (bookings.size() + 1);
    return ss.str();
}
