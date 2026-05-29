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

using namespace std;

static bool cmpRev(const pair<string,double>& a, const pair<string,double>& b) { return a.second > b.second; }

struct MonthlyRevenueCounter {
    int month;
    int year;
    double total;
    MonthlyRevenueCounter(int m, int y) : month(m), year(y), total(0.0) {}
    void operator()(const ChargingSession& session) {
        if (session.getStation() == 0) {
            return;
        }
        time_t endTime = session.getEndTime();
        struct tm* tmv = localtime(&endTime);
        if (tmv != 0 && tmv->tm_mon == month && tmv->tm_year == year) {
            total += session.getFinalCost();
        }
    }
};

struct StationSessionCount {
    std::string stationID;
    StationSessionCount(const std::string& id) : stationID(id) {}
    bool operator()(const ChargingSession& session) const {
        return session.getStation() != 0 && session.getStation()->getStationID() == stationID;
    }
};

struct HourCounter {
    int counts[24];
    HourCounter() { for (int i = 0; i < 24; ++i) counts[i] = 0; }
    void operator()(const ChargingSession& session) {
        time_t t = session.getStartTime();
        struct tm* tmv = localtime(&t);
        if (tmv != 0) {
            int h = tmv->tm_hour;
            if (h >= 0 && h < 24) {
                ++counts[h];
            }
        }
    }
};

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
    std::cout << "\n------------------------------------------------------------\n";
    std::cout << "Press enter to continue...\n";
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
    if (type == "DCFast" || type == "DC") {
        return new DCFastStation(id, name, latitude, longitude, powerRating, status, supportedTiers, extraValue);
    }
    if (type == "DCUltra" || type == "DCUltraFast") {
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
    std::map<std::string, bool> stationOccupied;
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
            if ((booking->getStatus() == Booking::Active || booking->getStatus() == Booking::Booked) && station != 0) {
                stationOccupied[station->getStationID()] = true;
            }
        }
    }
    for (std::map<std::string, Station*>::iterator it = stations.begin(); it != stations.end(); ++it) {
        const std::string& sid = it->first;
        Station* station = it->second;
        if (stationOccupied.find(sid) != stationOccupied.end()) {
            station->setStatus(Station::Occupied);
        } else if (station->getStatus() != Station::Faulty && station->getStatus() != Station::Maintenance) {
            station->setStatus(Station::Available);
        }
    }
}

void EVChargingManager::loadSessionLog() {
    std::ifstream in(sessionsFile.c_str());
    if (!in.is_open()) {
        return;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.find("SESSION,") != 0) {
            continue;
        }
        std::string payload = line.substr(8);
        std::vector<std::string> parts;
        std::string token;
        std::istringstream ss(payload);
        while (std::getline(ss, token, ',')) {
            parts.push_back(token);
        }
        if (parts.size() < 9) {
            continue;
        }
        std::string bid = parts[0];
        std::string uid = parts[1];
        std::string sid = parts[2];
        time_t start = (time_t)atol(parts[4].c_str());
        time_t end = (time_t)atol(parts[5].c_str());
        double energy = atof(parts[6].c_str());
        double finalCost = atof(parts[7].c_str());
        Station* station = findStation(sid);
        User* user = findUser(uid);
        ChargingSession session(bid, station, user, 0, start, end, Booking::Completed, energy, finalCost);
        completedSessions.push_back(session);
    }
    in.close();
}

void EVChargingManager::saveStations() {
    std::string tempName = stationsFile + ".tmp";
    std::ofstream out(tempName.c_str());
    if (!out.is_open()) {
        return;
    }
    for (std::map<std::string, Station*>::iterator it = stations.begin(); it != stations.end(); ++it) {
        it->second->saveToCsv(out);
    }
    out.close();
    std::remove(stationsFile.c_str());
    std::rename(tempName.c_str(), stationsFile.c_str());
}

void EVChargingManager::saveUsers() {
    std::string tempName = usersFile + ".tmp";
    std::ofstream out(tempName.c_str());
    if (!out.is_open()) {
        return;
    }
    for (std::map<std::string, User*>::iterator it = users.begin(); it != users.end(); ++it) {
        it->second->saveToCsv(out);
    }
    out.close();
    std::remove(usersFile.c_str());
    std::rename(tempName.c_str(), usersFile.c_str());
}

void EVChargingManager::saveBookings() {
    std::string tempName = bookingsFile + ".tmp";
    std::ofstream out(tempName.c_str());
    if (!out.is_open()) {
        return;
    }
    for (std::vector<Booking*>::iterator it = bookings.begin(); it != bookings.end(); ++it) {
        (*it)->saveToCsv(out);
    }
    out.close();
    std::remove(bookingsFile.c_str());
    std::rename(tempName.c_str(), bookingsFile.c_str());
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
    std::ostringstream ss;
    ss << "STATIONS\n";
    for (std::map<std::string, Station*>::const_iterator it = stations.begin(); it != stations.end(); ++it) {
        it->second->saveToCsv(ss);
    }
    ss << "USERS\n";
    for (std::map<std::string, User*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        it->second->saveToCsv(ss);
    }
    ss << "BOOKINGS\n";
    for (std::vector<Booking*>::const_iterator it = bookings.begin(); it != bookings.end(); ++it) {
        (*it)->saveToCsv(ss);
    }
    ss << "SESSIONS\n";
    for (std::vector<ChargingSession>::const_iterator it = completedSessions.begin(); it != completedSessions.end(); ++it) {
        it->logSession(ss);
    }
    std::string data = ss.str();
    std::ofstream out(backupFile.c_str(), std::ios::binary);
    if (!out.is_open()) return;
    out.write(data.c_str(), data.size());
    out.close();
}

void EVChargingManager::restoreSystem() {
    std::ifstream in(backupFile.c_str(), std::ios::binary);
    if (!in.is_open()) return;
    std::ostringstream ss;
    ss << in.rdbuf();
    std::string data = ss.str();
    in.close();
    size_t p1 = data.find("STATIONS\n");
    size_t p2 = data.find("USERS\n");
    size_t p3 = data.find("BOOKINGS\n");
    size_t p4 = data.find("SESSIONS\n");
    if (p1 == std::string::npos || p2 == std::string::npos || p3 == std::string::npos || p4 == std::string::npos) return;
    std::string stationsData = data.substr(p1 + 9, p2 - (p1 + 9));
    std::string usersData = data.substr(p2 + 6, p3 - (p2 + 6));
    std::string bookingsData = data.substr(p3 + 9, p4 - (p3 + 9));
    std::string sessionsData = data.substr(p4 + 9);
    std::ofstream out;
    out.open(stationsFile.c_str()); out << stationsData; out.close();
    out.open(usersFile.c_str()); out << usersData; out.close();
    out.open(bookingsFile.c_str()); out << bookingsData; out.close();
    out.open(sessionsFile.c_str()); out << sessionsData; out.close();
    // clear current state
    for (std::map<std::string, Station*>::iterator it = stations.begin(); it != stations.end(); ++it) delete it->second;
    stations.clear();
    for (std::map<std::string, User*>::iterator it = users.begin(); it != users.end(); ++it) delete it->second;
    users.clear();
    for (std::vector<Booking*>::iterator it = bookings.begin(); it != bookings.end(); ++it) delete *it;
    bookings.clear();
    while (!activeBookings.empty()) activeBookings.pop();
    completedSessions.clear();
    // reload
    loadData();
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
        std::cout << "\n============================================================\n";
        std::cout << "     SMART EV CHARGING NETWORK OPERATIONS PLATFORM\n";
        std::cout << "                ChargeGrid Mobility Pvt. Ltd.\n";
        std::cout << "============================================================\n";
        std::cout << "1. Admin / Operator Portal\n";
        std::cout << "2. User Portal\n";
        std::cout << "3. Analytics & Reports\n";
        std::cout << "4. System Backup & Restore\n";
        std::cout << "5. Exit\n";
        std::cout << "------------------------------------------------------------\n";
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
        std::cout << "\n==================== ADMIN PORTAL ====================\n";
        std::cout << "1. Station Management\n";
        std::cout << "2. User Management\n";
        std::cout << "3. View Real-time Station Status\n";
        std::cout << "4. Manage Active Bookings\n";
        std::cout << "5. Back to Main Menu\n";
        std::cout << "------------------------------------------------------------\n";
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
        std::cout << "\n------------------ STATION MANAGEMENT ------------------\n";
        std::cout << "1. Add New Charging Station\n";
        std::cout << "2. Remove Station\n";
        std::cout << "3. Update Station Status\n";
        std::cout << "4. List All Stations\n";
        std::cout << "5. Search Station by ID\n";
        std::cout << "6. Back to Admin Menu\n";
        std::cout << "------------------------------------------------------------\n";
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
        std::cout << "\n------------------- USER MANAGEMENT --------------------\n";
        std::cout << "1. Add New User (Register)\n";
        std::cout << "2. Remove User\n";
        std::cout << "3. List All Users\n";
        std::cout << "4. Search User by ID\n";
        std::cout << "5. View User Details\n";
        std::cout << "6. Back to Admin Menu\n";
        std::cout << "------------------------------------------------------------\n";
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
        std::cout << "\n------------------ ACTIVE BOOKINGS ---------------------\n";
        std::cout << "1. View All Active Bookings\n";
        std::cout << "2. Cancel Booking (Admin)\n";
        std::cout << "3. Force End Session\n";
        std::cout << "4. Back to Admin Menu\n";
        std::cout << "------------------------------------------------------------\n";
        int choice = readInt("Enter your choice: ", 1, 4);
        if (choice == 1) {
            listAllBookings();
            pause();
        } else if (choice == 2) {
            std::string id = readLine("Booking ID: ");
            Booking* booking = findBooking(id);
            if (booking != 0) {
                booking->cancelBooking();
                if (booking->getStation() != 0) booking->getStation()->setStatus(Station::Available);
                saveBookings();
                saveStations();
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
        std::cout << "\n===================== USER PORTAL =======================\n";
        std::cout << "1. Register New User\n";
        std::cout << "2. Login\n";
        std::cout << "3. Search & Book Charging Slot\n";
        std::cout << "4. View My Bookings\n";
        std::cout << "5. Cancel My Booking\n";
        std::cout << "6. View My History\n";
        std::cout << "7. Back to Main Menu\n";
        std::cout << "------------------------------------------------------------\n";
        int choice = readInt("Enter your choice: ", 1, 7);
        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            loginUser();
        } else if (choice == 3) {
            std::string id = readLine("User ID (or type 'back' to return): ");
            if (id == "back") {
                continue;
            }
            searchAndBook(id);
        } else if (choice == 4) {
            std::string id = readLine("User ID (or type 'back' to return): ");
            if (id == "back") {
                continue;
            }
            viewMyBookings(id);
            pause();
        } else if (choice == 5) {
            std::string id = readLine("User ID (or type 'back' to return): ");
            if (id == "back") {
                continue;
            }
            cancelBookingForUser(id);
        } else if (choice == 6) {
            std::string id = readLine("User ID (or type 'back' to return): ");
            if (id == "back") {
                continue;
            }
            viewMyHistory(id);
            pause();
        } else {
            break;
        }
    }
}

void EVChargingManager::analyticsMenu() {
    while (true) {
        std::cout << "\n================== ANALYTICS & REPORTS ==================\n";
        std::cout << "1. Revenue Summary Report (Current Month)\n";
        std::cout << "2. Station Utilization Report\n";
        std::cout << "3. Top 10 Highest Revenue Stations\n";
        std::cout << "4. List Inactive Users (No session > 30 days)\n";
        std::cout << "5. Peak Hour Utilization Analysis\n";
        std::cout << "6. Export Report to File\n";
        std::cout << "7. Back to Main Menu\n";
        std::cout << "------------------------------------------------------------\n";
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
        std::cout << "\n==================== BACKUP & RESTORE ===================\n";
        std::cout << "1. Create Full System Backup\n";
        std::cout << "2. Restore System from Backup\n";
        std::cout << "3. Export All Stations to CSV\n";
        std::cout << "4. Export All Users to CSV\n";
        std::cout << "5. Export All Sessions Log\n";
        std::cout << "6. Back to Main Menu\n";
        std::cout << "------------------------------------------------------------\n";
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
        int status = (*it)->getStatus();
        std::string statusLabel = "Unknown";
        if (status == Booking::Booked) {
            statusLabel = "Booked";
        } else if (status == Booking::Active) {
            statusLabel = "Active";
        } else if (status == Booking::Completed) {
            statusLabel = "Completed";
        } else if (status == Booking::Cancelled) {
            statusLabel = "Cancelled";
        }
        std::cout << "Booking ID: " << (*it)->getBookingID() << " | User: "
                  << ((*it)->getUser() ? (*it)->getUser()->getUserID() : "")
                  << " | Station: "
                  << ( (*it)->getStation() ? (*it)->getStation()->getStationID() : "" )
                  << " | Status: " << statusLabel << std::endl;
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
    std::string id = readLine("User ID (or type 'back' to return): ");
    if (id == "back") {
        std::cout << "Returning to User Portal.\n";
        return;
    }
    User* user = findUser(id);
    if (user == 0) {
        std::cout << "User ID not found. Please register or try again.\n";
        return;
    }
    std::cout << "User successfully logged in: " << user->getUserID() << "\n";
    while (true) {
        std::cout << "\n========================================\n";
        std::cout << "1. Search Available Stations (with Recommendation)\n";
        std::cout << "2. Book a Charging Slot\n";
        std::cout << "3. View My Active Booking\n";
        std::cout << "4. Start Charging Session\n";
        std::cout << "5. End Charging Session\n";
        std::cout << "6. View My History\n";
        std::cout << "7. Logout\n";
        std::cout << "========================================\n";
        int choice = readInt("Enter your choice: ", 1, 7);
        if (choice == 1) {
            availableStationsForUser(id);
            pause();
        } else if (choice == 2) {
            searchAndBook(id);
        } else if (choice == 3) {
            viewMyActiveBooking(id);
            pause();
        } else if (choice == 4) {
            startSessionForUser(id);
        } else if (choice == 5) {
            endSessionForUser(id);
        } else if (choice == 6) {
            viewMyHistory(id);
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
    std::string tier = user->getTier();
    std::cout << "\n Station Type Selection:\n";
    std::cout << "0. Back\n";
    std::cout << "1. AC Station\n";
    std::cout << "2. DC Fast Station\n";
    std::cout << "3. DC Ultra Fast Station\n";
    std::cout << "4. Both DC Types\n";
    int choice = readInt("Choose station type (0-4): ", 0, 4);
    if (choice == 0) {
        std::cout << "Booking cancelled. Returning to previous menu.\n";
        return;
    }
    
    std::vector<Station*> available;
    std::string selectedType = "";
    
    for (std::map<std::string, Station*>::iterator it = stations.begin(); it != stations.end(); ++it) {
        Station* station = it->second;
        if (!station->isAvailable()) {
            continue;
        }
        std::string stationType = station->getType();
        bool allowed = false;
        bool match = false;
        
        if (tier == "Regular") {
            allowed = (stationType == "AC");
        } else {
            allowed = true;
        }
        
        if (choice == 1 && stationType == "AC") {
            match = true;
            if (selectedType.empty()) selectedType = "AC";
        } else if (choice == 2 && stationType == "DCFast") {
            match = true;
            if (selectedType.empty()) selectedType = "DCFast";
        } else if (choice == 3 && stationType == "DCUltra") {
            match = true;
            if (selectedType.empty()) selectedType = "DCUltra";
        } else if (choice == 4 && (stationType == "DCFast" || stationType == "DCUltra")) {
            match = true;
            if (selectedType.empty()) selectedType = "DC";
        }
        
        if (match && allowed) {
            available.push_back(station);
        }
    }
    
    if (available.empty()) {
        std::cout << "No stations available for your selection and tier.\n";
        return;
    }
    
    std::cout << "\nAvailable stations:\n";
    for (std::vector<Station*>::iterator it = available.begin(); it != available.end(); ++it) {
        (*it)->displayInfo();
    }
    
    std::string sid = readLine("Enter Station ID (or type 'back' to return): ");
    if (sid == "back") {
        std::cout << "Booking cancelled. Returning to previous menu.\n";
        return;
    }
    Station* station = findStation(sid);
    if (station == 0) {
        std::cout << "Station ID not found.\n";
        return;
    }
    if (!station->isAvailable()) {
        std::cout << "Selected station is not available.\n";
        return;
    }
    
    bool stationAllowed = false;
    if (tier == "Regular" && station->getType() == "AC") {
        stationAllowed = true;
    } else if (tier != "Regular") {
        stationAllowed = true;
    }
    if (!stationAllowed) {
        std::cout << "Regular tier users can only book AC stations.\n";
        return;
    }
    
    int minutes = readInt("Duration minutes: ", 1, 240);
    double perMin = station->calculatePricePerMin(user);
    double estBase = perMin * minutes;
    double estDiscount = user->calculateDiscount(estBase);
    double estCost = estBase - estDiscount;
    
    std::cout << "Estimated cost: " << estCost << " | Wallet: " << user->getWalletBalance() << "\n";
    if (user->getWalletBalance() < estCost) {
        std::cout << "Insufficient wallet balance to book.\n";
        return;
    }
    
    std::string bid = generateBookingId();
    time_t now = std::time(0);
    Booking* booking = createBooking(bid, station, user, minutes, now, now + minutes * 60, Booking::Booked);
    if (booking != 0) {
        station->setStatus(Station::Occupied);
        bookings.push_back(booking);
        std::cout << "Successfully booked the slot. Booking number: " << bid << "\n";
        saveBookings();
        saveStations();
    }
}

void EVChargingManager::viewMyBookings(const std::string& userID) const {
    User* user = findUser(userID);
    if (user == 0) {
        std::cout << "User ID not found.\n";
        return;
    }
    bool found = false;
    for (std::vector<Booking*>::const_iterator it = bookings.begin(); it != bookings.end(); ++it) {
        if ((*it)->getUser() != 0 && (*it)->getUser()->getUserID() == userID) {
            int status = (*it)->getStatus();
            if (status == Booking::Booked || status == Booking::Active) {
                std::cout << "Booking: " << (*it)->getBookingID() << " | Station: "
                          << ((*it)->getStation() ? (*it)->getStation()->getStationID() : "")
                          << " | Status: " << (status == Booking::Active ? "Active" : "Booked") << std::endl;
                found = true;
            }
        }
    }
    if (!found) {
        std::cout << "No active or upcoming bookings found for user " << userID << ".\n";
    }
}

void EVChargingManager::viewMyActiveBooking(const std::string& userID) const {
    bool found = false;
    for (std::vector<Booking*>::const_iterator it = bookings.begin(); it != bookings.end(); ++it) {
        if ((*it)->getUser() != 0 && (*it)->getUser()->getUserID() == userID) {
            int st = (*it)->getStatus();
            if (st == Booking::Booked || st == Booking::Active) {
                cout << "Booking: " << (*it)->getBookingID() << " | Station: "
                     << ((*it)->getStation() ? (*it)->getStation()->getStationID() : "")
                     << " | Status: " << (st == Booking::Active ? "Active" : "Booked")
                     << " | Start: " << (*it)->getStartTime() << " | Duration: " << (*it)->getSlotDuration() << " mins\n";
                found = true;
            }
        }
    }
    if (!found) cout << "No active or upcoming bookings found.\n";
}

void EVChargingManager::viewMyHistory(const std::string& userID) const {
    User* user = findUser(userID);
    if (user == 0) {
        std::cout << "User ID not found.\n";
        return;
    }
    bool found = false;
    for (std::vector<Booking*>::const_iterator it = bookings.begin(); it != bookings.end(); ++it) {
        if ((*it)->getUser() != 0 && (*it)->getUser()->getUserID() == userID) {
            int status = (*it)->getStatus();
            if (status == Booking::Completed || status == Booking::Cancelled) {
                std::cout << "History: " << (*it)->getBookingID() << " | Station: "
                          << ((*it)->getStation() ? (*it)->getStation()->getStationID() : "")
                          << " | Status: " << (status == Booking::Completed ? "Completed" : "Cancelled") << std::endl;
                found = true;
            }
        }
    }
    if (!found) {
        std::cout << "No completed or cancelled bookings found for user " << userID << ".\n";
    }
}

void EVChargingManager::cancelBookingForUser(const std::string& userID) {
    std::string bid = readLine("Booking ID (or type 'back' to return): ");
    if (bid == "back") {
        std::cout << "Cancel operation aborted.\n";
        return;
    }
    Booking* booking = findBooking(bid);
    if (booking == 0) {
        std::cout << "Booking ID not found.\n";
        return;
    }
    if (booking->getUser() == 0 || booking->getUser()->getUserID() != userID) {
        std::cout << "Booking does not belong to this user.\n";
        return;
    }
    booking->cancelBooking();
    if (booking->getStation() != 0) {
        booking->getStation()->setStatus(Station::Available);
    }
    saveBookings();
    saveStations();
    std::cout << "Booking " << bid << " cancelled successfully.\n";
}

void EVChargingManager::startSessionForUser(const std::string& userID) {
    std::string bid = readLine("Booking ID: ");
    Booking* booking = findBooking(bid);
    if (booking != 0 && booking->getUser() != 0 && booking->getUser()->getUserID() == userID && booking->getStatus() == Booking::Booked) {
        double est = booking->calculateCost();
        if (booking->getUser()->getWalletBalance() < est) {
            cout << "Insufficient wallet to start session.\n";
            return;
        }
        booking->startSession(std::time(0));
        if (booking->getStation() != 0) {
            booking->getStation()->setStatus(Station::Occupied);
        }
        saveBookings();
        saveStations();
    }
}

void EVChargingManager::endSessionForUser(const std::string& userID) {
    std::string bid = readLine("Booking ID: ");
    Booking* booking = findBooking(bid);
    if (booking != 0 && booking->getUser() != 0 && booking->getUser()->getUserID() == userID && booking->isActive()) {
        booking->endSession(std::time(0));
        if (booking->getStation() != 0) {
            booking->getStation()->setStatus(Station::Available);
        }
        
        // Charge for full booked duration regardless of actual session time
        int bookedMinutes = booking->getSlotDuration();
        double perMin = booking->getStation()->calculatePricePerMin(booking->getUser());
        double baseCost = perMin * bookedMinutes;
        double discount = booking->getUser()->calculateDiscount(baseCost);
        double finalCost = baseCost - discount;
        
        // Calculate energy consumed based on booked duration
        double energy = 0.0;
        if (booking->getStation() != 0) {
            energy = booking->getStation()->getPowerRating() * bookedMinutes / 60.0;
        }
        
        ChargingSession session(booking->getBookingID(), booking->getStation(), booking->getUser(), bookedMinutes, booking->getStartTime(), booking->getEndTime(), booking->getStatus(), energy, finalCost);
        completedSessions.push_back(session);
        
        // deduct from wallet
        if (booking->getUser() != 0) {
            booking->getUser()->adjustWallet(-finalCost);
        }
        
        logSession(session);
        saveBookings();
        saveStations();
        saveUsers();
    }
}

void EVChargingManager::availableStationsForUser(const std::string& userID) const {
    User* user = findUser(userID);
    if (user == 0) {
        return;
    }
    std::string preferred;
    if (user->getTier() == "Fleet") {
        preferred = "DCUltra";
    } else if (user->getTier() == "Premium") {
        preferred = "DCFast";
    } else {
        preferred = "AC";
    }
    std::cout << "Recommended station type: " << preferred << "\n";
    for (std::map<std::string, Station*>::const_iterator it = stations.begin(); it != stations.end(); ++it) {
        if (it->second->isAvailable()) {
            if (it->second->getType() == preferred) {
                std::cout << "Recommended: ";
            }
            it->second->displayInfo();
        }
    }
}

void EVChargingManager::revenueSummary() const {
    time_t now = time(0);
    struct tm* nowtm = localtime(&now);
    int currentMonth = nowtm ? nowtm->tm_mon : 0;
    int currentYear = nowtm ? nowtm->tm_year : 0;
    MonthlyRevenueCounter counter(currentMonth, currentYear);
    std::for_each(completedSessions.begin(), completedSessions.end(), counter);
    cout << "Total revenue (current month): " << counter.total << endl;
}

void EVChargingManager::utilizationReport() const {
    int totalSessions = completedSessions.size();
    for (std::map<std::string, Station*>::const_iterator it = stations.begin(); it != stations.end(); ++it) {
        int sessionCount = std::count_if(completedSessions.begin(), completedSessions.end(), StationSessionCount(it->first));
        double pct = totalSessions > 0 ? (double)sessionCount * 100.0 / (double)totalSessions : 0.0;
        cout << it->first << " | ";
        it->second->displayInfo();
        cout << "  Sessions: " << sessionCount << " | Utilization%: " << pct << endl;
    }
}

void EVChargingManager::topRevenueStations() const {
    std::map<std::string, double> rev;
    for (std::vector<ChargingSession>::const_iterator it = completedSessions.begin(); it != completedSessions.end(); ++it) {
        std::string sid = it->getStation() ? it->getStation()->getStationID() : "";
        if (sid != "") {
            rev[sid] += it->getFinalCost();
        }
    }
    std::vector<std::pair<std::string, double> > list;
    for (std::map<std::string, double>::const_iterator it = rev.begin(); it != rev.end(); ++it) {
        list.push_back(*it);
    }
    sort(list.begin(), list.end(), cmpRev);
    int n = 0;
    for (std::vector<std::pair<std::string,double> >::const_iterator it = list.begin(); it != list.end() && n < 10; ++it, ++n) {
        cout << "#" << (n+1) << " " << it->first << " => " << it->second << endl;
    }
}

void EVChargingManager::inactiveUsers() const {
    time_t now = time(0);
    std::map<std::string, time_t> last;
    for (std::vector<ChargingSession>::const_iterator it = completedSessions.begin(); it != completedSessions.end(); ++it) {
        std::string uid = it->getUser() ? it->getUser()->getUserID() : "";
        if (uid != "") {
            time_t end = it->getEndTime();
            if (last.find(uid) == last.end() || last[uid] < end) {
                last[uid] = end;
            }
        }
    }
    const time_t THIRTY_DAYS = 30 * 24 * 3600;
    for (std::map<std::string, User*>::const_iterator it = users.begin(); it != users.end(); ++it) {
        std::string uid = it->first;
        if (last.find(uid) == last.end() || (now - last[uid]) > THIRTY_DAYS) {
            cout << "Inactive: " << uid << " | ";
            it->second->displayInfo();
        }
    }
}

void EVChargingManager::peakHourAnalysis() const {
    HourCounter counter;
    std::for_each(completedSessions.begin(), completedSessions.end(), counter);
    int maxCount = 0;
    cout << "Peak hours (hour:count)\n";
    for (int i = 0; i < 24; ++i) {
        if (counter.counts[i] > 0) {
            cout << i << ":" << counter.counts[i] << "\n";
            if (counter.counts[i] > maxCount) {
                maxCount = counter.counts[i];
            }
        }
    }
    cout << "Top peak count: " << maxCount << "\n";
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
