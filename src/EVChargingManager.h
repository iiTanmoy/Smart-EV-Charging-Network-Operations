#ifndef EVCHARGINGMANAGER_H
#define EVCHARGINGMANAGER_H

#include <string>
#include <map>
#include <queue>
#include <vector>
#include "Station.h"
#include "Booking.h"
#include "ChargingSession.h"
#include "User.h"

class EVChargingManager {
public:
    EVChargingManager();
    ~EVChargingManager();
    bool loadData();
    void mainMenu();
private:
    std::map<std::string, Station*> stations;
    std::map<std::string, User*> users;
    std::vector<Booking*> bookings;
    std::queue<Booking*> activeBookings;
    std::vector<ChargingSession> completedSessions;
    std::string dataDir;
    std::string dbDir;
    std::string stationsFile;
    std::string usersFile;
    std::string bookingsFile;
    std::string sessionsFile;
    std::string backupFile;
    Station* createStation(const std::string& type,
                           const std::string& id,
                           const std::string& name,
                           double latitude,
                           double longitude,
                           double powerRating,
                           int status,
                           const std::string& supportedTiers,
                           double extraValue);
    User* createUser(const std::string& id,
                     const std::string& name,
                     const std::string& contact,
                     const std::string& tier,
                     double walletBalance,
                     int totalSessions);
    Booking* createBooking(const std::string& bookingID,
                           Station* station,
                           User* user,
                           int slotDuration,
                           time_t startTime,
                           time_t endTime,
                           int status);
    bool loadFile(const std::string& fileName,
                  std::vector<std::string>& lines) const;
    bool saveFile(const std::string& fileName,
                  const std::vector<std::string>& lines) const;
    void loadStations();
    void loadUsers();
    void loadBookings();
    void loadSessionLog();
    void saveStations();
    void saveUsers();
    void saveBookings();
    void saveAll();
    void logSession(const ChargingSession& session);
    void backupSystem() const;
    void restoreSystem();
    bool ensureDataDirectory() const;
    bool ensureDbDirectory() const;
    std::string buildDataPath(const std::string& fileName) const;
    std::string buildDbPath(const std::string& fileName) const;
    std::string readLine(const char* prompt) const;
    int readInt(const char* prompt, int min, int max) const;
    double readDouble(const char* prompt, double min, double max) const;
    void pause() const;
    void adminMenu();
    void stationManagementMenu();
    void userManagementMenu();
    void activeBookingMenu();
    void userPortal();
    void analyticsMenu();
    void backupMenu();
    void listAllStations() const;
    void listAllUsers() const;
    void listAllBookings() const;
    Booking* findBooking(const std::string& bookingID) const;
    Station* findStation(const std::string& stationID) const;
    User* findUser(const std::string& userID) const;
    void addStation();
    void removeStation();
    void updateStationStatus();
    void searchStationById() const;
    void addUser();
    void removeUser();
    void searchUserById() const;
    void viewUserDetails() const;
    void registerUser();
    void loginUser();
    void searchAndBook(const std::string& userID);
    void viewMyBookings(const std::string& userID) const;
    void viewMyActiveBooking(const std::string& userID) const;
    void viewMyHistory(const std::string& userID) const;
    void cancelBookingForUser(const std::string& userID);
    void startSessionForUser(const std::string& userID);
    void endSessionForUser(const std::string& userID);
    void availableStationsForUser(const std::string& userID) const;
    void revenueSummary() const;
    void utilizationReport() const;
    void topRevenueStations() const;
    void inactiveUsers() const;
    void peakHourAnalysis() const;
    void exportReports() const;
    void exportStations() const;
    void exportUsers() const;
    void exportSessions() const;
    std::string generateBookingId() const;
};

#endif // EVCHARGINGMANAGER_H
