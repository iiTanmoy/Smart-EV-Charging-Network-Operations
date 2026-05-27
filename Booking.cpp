#include "Booking.h"
#include "Station.h"
#include "User.h"

Booking::Booking(const std::string& bookingID, Station* station, User* user,
                 int slotDuration, time_t startTime, time_t endTime, int status)
    : bookingID(bookingID), station(station), user(user), slotDuration(slotDuration),
      startTime(startTime), endTime(endTime), status(status) {
}

Booking::~Booking() {
}

const std::string& Booking::getBookingID() const {
    return bookingID;
}

Station* Booking::getStation() const {
    return station;
}

User* Booking::getUser() const {
    return user;
}

int Booking::getSlotDuration() const {
    return slotDuration;
}

time_t Booking::getStartTime() const {
    return startTime;
}

time_t Booking::getEndTime() const {
    return endTime;
}

int Booking::getStatus() const {
    return status;
}

void Booking::startSession(time_t startTime) {
    this->startTime = startTime;
    status = Active;
}

void Booking::endSession(time_t endTime) {
    this->endTime = endTime;
    status = Completed;
    if (user != 0) {
        user->addSession();
    }
}

void Booking::setStatus(int newStatus) {
    status = newStatus;
}

void Booking::cancelBooking() {
    status = Cancelled;
}

bool Booking::isActive() const {
    return status == Active;
}

double Booking::calculateCost() const {
    if (station == 0 || user == 0) {
        return 0.0;
    }
    double pricePerMin = station->calculatePricePerMin(user);
    double baseCost = pricePerMin * slotDuration;
    double discount = user->calculateDiscount(baseCost);
    return baseCost - discount;
}

void Booking::saveToCsv(std::ostream& out) const {
    out << bookingID << ","
        << (station != 0 ? station->getStationID() : "") << ","
        << (user != 0 ? user->getUserID() : "") << ","
        << slotDuration << ","
        << startTime << ","
        << endTime << ","
        << status << "\n";
}
