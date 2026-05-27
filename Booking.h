#ifndef BOOKING_H
#define BOOKING_H

#include <string>
#include <ctime>
#include <fstream>

class Station;
class User;

class Booking {
public:
    enum Status {
        Booked = 0,
        Active = 1,
        Completed = 2,
        Cancelled = 3
    };

    Booking(const std::string& bookingID, Station* station, User* user,
            int slotDuration, time_t startTime, time_t endTime, int status);
    virtual ~Booking();

    const std::string& getBookingID() const;
    Station* getStation() const;
    User* getUser() const;
    int getSlotDuration() const;
    time_t getStartTime() const;
    time_t getEndTime() const;
    int getStatus() const;

    void startSession(time_t startTime);
    void endSession(time_t endTime);
    bool isActive() const;
    virtual double calculateCost() const;
    virtual void saveToCsv(std::ostream& out) const;

protected:
    std::string bookingID;
    Station* station;
    User* user;
    int slotDuration;
    time_t startTime;
    time_t endTime;
    int status;
};

#endif // BOOKING_H
