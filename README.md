# Smart EV Charging Network Operations Platform

[![C++](https://img.shields.io/badge/C%2B%2B-98-blue)](https://isocpp.org)
[![License](https://img.shields.io/badge/License-MIT-green)](LICENSE)
[![Status](https://img.shields.io/badge/Status-Active-brightgreen)](README.md)

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [System Requirements](#system-requirements)
- [Project Structure](#project-structure)
- [Installation & Setup](#installation--setup)
- [Building the Project](#building-the-project)
- [Running the Application](#running-the-application)
- [Usage Guide](#usage-guide)
- [Architecture](#architecture)
- [File Organization](#file-organization)
- [Database Schema](#database-schema)
- [Troubleshooting](#troubleshooting)

---

## 🎯 Overview

**Smart EV Charging Network Operations** is a comprehensive C++98-based platform for managing electric vehicle charging networks. It provides a complete solution for station management, user booking systems, charging session tracking, and analytics reporting.

### Organization
**ChargeGrid Mobility Pvt. Ltd.**

### Purpose
This platform enables operators to efficiently manage EV charging stations, handle user bookings, track charging sessions, generate revenue reports, and analyze network utilization patterns in real-time.

---

## ✨ Features

### Core Functionality

#### **Admin & Operator Portal**
- 📍 **Station Management**: Add, remove, and update charging stations
- 👥 **User Management**: Register, manage, and track user accounts
- 📊 **Booking Management**: Monitor active and completed bookings
- 🔧 **System Maintenance**: Backup, restore, and system configuration

#### **User Portal**
- 🔓 **Authentication**: User login and registration system
- 🔍 **Station Discovery**: Search and filter available charging stations
- 📅 **Booking System**: Reserve charging slots with tier-based pricing
- ⚡ **Charging Sessions**: Start, monitor, and end charging sessions
- 💳 **Wallet Management**: Track balance and transaction history
- 📜 **History Tracking**: View completed and cancelled bookings

#### **Station Types**
- **AC Stations**: Standard AC charging with adjustable slots
- **DC Fast Stations**: High-speed DC charging
- **DC Ultra Fast Stations**: Premium ultra-fast charging

#### **User Tiers**
- **Regular Users**: Access to AC stations only
- **Premium Users**: Access to all stations with no discount
- **Fleet Users**: Access to all stations with 10% discount

#### **Analytics & Reporting**
- 💰 **Revenue Summary**: Monthly and historical revenue analysis
- 📈 **Utilization Reports**: Station usage patterns and efficiency metrics
- 🏆 **Top Performers**: Ranking of highest revenue-generating stations
- ⏰ **Peak Hour Analysis**: Hourly utilization patterns
- 👤 **Inactive User Detection**: Identify users inactive for 30+ days
- 📁 **Export Functionality**: Generate timestamped reports and backups

#### **Data Persistence**
- 🗄️ **CSV Persistence**: Station, user, and booking data in CSV format
- 📝 **Session Logging**: Complete charging session history
- 💾 **Automatic Backup**: Full system backup and restore capabilities
- 📊 **Data Export**: Export analytics, stations, users, and sessions

---

## 🖥️ System Requirements

### Compiler & Build Tools
- **Compiler**: GNU g++ (supporting C++98 standard)
- **Build Tool**: Make (GNU Make)
- **Language Standard**: C++98 with `-std=c++98 -Wall -pedantic`

### Operating System
- Linux (Tested on Ubuntu 24.04.4 LTS)
- macOS (Should work with compatible g++ installation)
- Windows (WSL 2 or similar Linux subsystem recommended)

### Disk Space
- Minimum: 50 MB for source code and executable
- Recommended: 200+ MB for development and data storage

### Runtime Libraries
- Standard C++ library (included with g++)
- POSIX-compliant system (for directory operations)

---

## 📁 Project Structure

```
Smart-EV-Charging-Network-Operations/
├── src/                          # Source code directory
│   ├── main.cpp                  # Application entry point
│   ├── EVChargingManager.cpp     # Main controller and business logic
│   ├── EVChargingManager.h       # Manager header
│   ├── Station.cpp               # Base station class
│   ├── Station.h
│   ├── ACStation.cpp             # AC station implementation
│   ├── ACStation.h
│   ├── DCFastStation.cpp         # DC Fast station implementation
│   ├── DCFastStation.h
│   ├── DCUltraFastStation.cpp    # DC Ultra Fast station implementation
│   ├── DCUltraFastStation.h
│   ├── User.cpp                  # Base user class
│   ├── User.h
│   ├── RegularUser.cpp           # Regular user implementation
│   ├── RegularUser.h
│   ├── PremiumUser.cpp           # Premium user implementation
│   ├── PremiumUser.h
│   ├── FleetUser.cpp             # Fleet user implementation
│   ├── FleetUser.h
│   ├── Booking.cpp               # Booking management
│   ├── Booking.h
│   ├── ChargingSession.cpp       # Charging session tracking
│   └── ChargingSession.h
│
├── db/                           # Database directory
│   ├── stations.csv              # Station master data
│   ├── users.csv                 # User account data
│   └── bookings.csv              # Active booking records
│
├── data/                         # Data exports directory
│   ├── sessions.log              # Complete session history
│   ├── backup.dat                # System backup file
│   └── analytics_report_*.txt    # Generated analytics reports
│
├── obj/                          # Compiled object files (auto-created)
│   └── *.o                       # Object files
│
├── Makefile                      # Build configuration
├── start.sh                      # Legacy build script
├── README.md                     # This file
└── evmanager                     # Compiled executable
```

---

## 💾 Installation & Setup

### Prerequisites

1. **Install g++ compiler** (if not already installed):
   ```bash
   # Ubuntu/Debian
   sudo apt-get install build-essential

   # macOS (with Homebrew)
   brew install gcc

   # Fedora/RHEL
   sudo yum install gcc-c++
   ```

2. **Install Make** (if not already installed):
   ```bash
   # Ubuntu/Debian
   sudo apt-get install make

   # macOS (usually pre-installed)
   # Fedora/RHEL
   sudo yum install make
   ```

### Clone the Repository

```bash
git clone https://github.com/Prantik2495095/Smart-EV-Charging-Network-Operations.git
cd Smart-EV-Charging-Network-Operations
```

### Verify Installation

```bash
# Check g++ version
g++ --version

# Check Make version
make --version

# Verify project structure
ls -la
```

---

## 🔨 Building the Project

### Using Makefile (Recommended)

#### Basic Build
```bash
# Build the project
make

# Or explicitly
make build
```

#### Clean Build (Recommended for first-time setup)
```bash
# Clean all previous builds and rebuild
make rebuild
```

#### View All Available Commands
```bash
make help
```

### Makefile Targets

| Command | Description |
|---------|-------------|
| `make` | Build project (default) |
| `make build` | Compile and link |
| `make run` | Build and run application |
| `make rebuild` | Clean and build from scratch |
| `make clean` | Remove object files and executable |
| `make clean-all` | Remove everything including data |
| `make info` | Display project configuration |
| `make status` | Show build status and directories |
| `make help` | Display help information |

### Manual Build (Without Makefile)

If Make is not available:
```bash
cd /workspaces/Smart-EV-Charging-Network-Operations
g++ -std=c++98 -Wall -pedantic src/*.cpp -o evmanager
```

### Build Verification

```bash
# Verify executable exists
ls -lh evmanager

# Check build status
make status
```

---

## 🚀 Running the Application

### Method 1: Using Makefile (Recommended)

```bash
# Build and run in one command
make run
```

### Method 2: Direct Execution

```bash
# Run the compiled executable
./evmanager
```

### Method 3: Using Legacy Script

```bash
# Build and run using start.sh
bash start.sh
```

### Startup Verification

When the application starts, you should see:
```
============================================================
     SMART EV CHARGING NETWORK OPERATIONS PLATFORM
                ChargeGrid Mobility Pvt. Ltd.
============================================================
1. Admin / Operator Portal
2. User Portal
3. Analytics & Reports
4. System Backup & Restore
5. Exit
------------------------------------------------------------
Enter your choice (1-5):
```

---

## 📖 Usage Guide

### Main Menu Navigation

#### 1. **Admin / Operator Portal**
   - Manage charging stations (add, remove, update)
   - Manage user accounts
   - Monitor active bookings
   - View all system data

#### 2. **User Portal**
   - Register new user account
   - Login to existing account
   - Browse available stations
   - Make charging bookings
   - Start/end charging sessions
   - View booking history
   - Cancel bookings

#### 3. **Analytics & Reports**
   - View monthly revenue summary
   - Check station utilization reports
   - Identify top-performing stations
   - Detect inactive users
   - Analyze peak hour patterns
   - Export comprehensive analytics reports

#### 4. **System Backup & Restore**
   - Create full system backup
   - Restore from backup file
   - Export stations data
   - Export users data
   - Export session logs

### Example User Workflow

```
1. Start application → make run
2. Select "User Portal" (Option 2)
3. Select "Register" (Option 1) → Enter user details
4. Select "Login" (Option 2) → Enter user ID
5. Select "Search Available Stations" (Option 1)
6. Select "Book a Charging Slot" (Option 2)
7. Choose station type and station
8. Complete booking
```

### Pricing System

**Base Prices (per minute):**
- AC Stations: $0.10/min
- DC Fast Stations: $0.30/min
- DC Ultra Fast Stations: $0.50/min

**Tier Discounts:**
- Regular Users: No discount
- Premium Users: No discount
- Fleet Users: 10% discount on all charges

---

## 🏗️ Architecture

### Design Pattern: Object-Oriented Programming

#### Class Hierarchy

**Stations:**
```
Station (Abstract Base)
├── ACStation
├── DCFastStation
└── DCUltraFastStation
```

**Users:**
```
User (Abstract Base)
├── RegularUser
├── PremiumUser
└── FleetUser
```

**Core Components:**
- `EVChargingManager` - Main controller handling all operations
- `Booking` - Booking entity and cost calculation
- `ChargingSession` - Session tracking and billing

### Data Flow

```
User Input
    ↓
EVChargingManager (Business Logic)
    ↓
Station/User/Booking Objects
    ↓
CSV Files (db/) / Logs (data/)
    ↓
Analytics & Reports
```

### Tier-Based Access Control

- **Regular Users**: AC stations only
- **Premium/Fleet Users**: All station types
- **Fleet Users**: Additional 10% discount

---

## 💾 File Organization

### Database Files (db/ directory)

#### `stations.csv`
Stores all charging station information:
- Station ID, Type, Location name, Coordinates
- Power rating, Status, Supported tiers, Extra parameters

#### `users.csv`
Stores user account information:
- User ID, Name, Contact, Tier
- Wallet balance, Total sessions

#### `bookings.csv`
Stores active and historical bookings:
- Booking ID, User ID, Station ID
- Duration, Start time, End time, Status

### Data Files (data/ directory)

#### `sessions.log`
Chronological log of all completed charging sessions:
- Session metadata, energy consumed, cost charged

#### `backup.dat`
Complete system backup containing:
- All stations, users, bookings, and sessions data
- Binary format for efficient storage

#### `analytics_report_[timestamp].txt`
Generated analytics reports with:
- Revenue summaries, utilization metrics
- Top performers, peak hour analysis
- Inactive user lists, timestamp for traceability

### Source Code (src/ directory)

Organized by component type:
- `main.cpp` - Application entry point
- `EVChargingManager.*` - Main controller
- `Station.*` and derived classes
- `User.*` and derived classes
- `Booking.*` and `ChargingSession.*`

---

## 🗄️ Database Schema

### CSV Format Specifications

#### stations.csv
```
ID,Type,LocationName,Latitude,Longitude,PowerRating,Status,SupportedTiers,ExtraValue
S001,AC,Downtown Hub,40.7128,-74.0060,7.5,0,Regular|Premium|Fleet,8
```

#### users.csv
```
UserID,Name,Contact,Tier,WalletBalance,TotalSessions
U001,John Doe,john@email.com,Regular,500.00,5
```

#### bookings.csv
```
BookingID,UserID,StationID,DurationMins,StartTime,EndTime,Status
B1,U001,S001,60,1705363200,1705366800,0
```

### Status Codes

**Booking Status:**
- 0 = Booked
- 1 = Active
- 2 = Completed
- 3 = Cancelled

**Station Status:**
- 0 = Available
- 1 = Occupied
- 2 = Faulty
- 3 = Maintenance

---

## 🔧 Troubleshooting

### Build Issues

#### Issue: "g++: command not found"
**Solution:**
```bash
# Install g++ compiler
sudo apt-get install build-essential
```

#### Issue: "make: command not found"
**Solution:**
```bash
# Install Make
sudo apt-get install make
```

#### Issue: Compilation errors with C++98
**Solution:**
Ensure you're compiling with the correct standard:
```bash
g++ -std=c++98 -Wall -pedantic src/*.cpp -o evmanager
```

### Runtime Issues

#### Issue: "Cannot open database file"
**Solution:**
Check if `db/` directory exists:
```bash
mkdir -p db/
make rebuild
```

#### Issue: Missing data directory
**Solution:**
The `data/` directory should auto-create. If not:
```bash
mkdir -p data/
```

#### Issue: "Permission denied" when running
**Solution:**
Make the executable runnable:
```bash
chmod +x evmanager
./evmanager
```

### Data Issues

#### Issue: Lost booking data
**Solution:**
Restore from backup:
1. Run application → System Backup & Restore
2. Select option 2 (Restore System from Backup)

#### Issue: Corrupted CSV file
**Solution:**
Clean and rebuild:
```bash
make clean-all
make rebuild
```

---

## 📊 Analytics & Reporting

### Report Generation

Generated reports include:
- **Revenue Summary**: Total revenue for current month
- **Utilization Metrics**: Percentage usage per station
- **Top 10 Stations**: Ranked by revenue
- **Inactive Users**: No activity in 30+ days
- **Peak Hours**: Hourly session distribution

### Export Locations

All exports are saved in `data/` directory with timestamps:
```
data/analytics_report_20260529_070245.txt
data/stations_export_20260529_070245.csv
data/users_export_20260529_070245.csv
data/sessions_export_20260529_070245.log
```

---

## 🔐 Data Security

### Backup Strategy

- **Automatic Creation**: Backups can be created anytime from admin panel
- **Full Coverage**: Includes all stations, users, bookings, and sessions
- **Binary Format**: `backup.dat` for efficient storage
- **Restoration**: One-click restore to previous state

### Data Persistence

- **Transaction Safety**: CSV files written to temporary files then renamed
- **Atomic Operations**: Prevent partial writes during system shutdown
- **Automatic Loading**: Data loaded on startup

---

## 📝 License

This project is provided as-is for educational and commercial purposes.

---

## 👨‍💼 Support & Contact

For issues, questions, or contributions, please contact:
- **Organization**: ChargeGrid Mobility Pvt. Ltd.
- **Repository**: [GitHub Repository](https://github.com/Prantik2495095/Smart-EV-Charging-Network-Operations)

---

## 🎓 Learning Resources

### C++ Features Used
- Object-Oriented Programming (OOP)
- Polymorphism and Inheritance
- File I/O Operations
- STL Containers (map, vector, queue)
- Time Management (time_t, struct tm)
- String Processing and CSV Parsing

### Best Practices
- Separation of concerns
- Clean architecture
- Proper error handling
- Data persistence patterns
- User interface design for CLI

---

## 📋 Changelog

### Version 1.0 (Latest)
- ✅ Full station management system
- ✅ Multi-tier user system with tier-based pricing
- ✅ Comprehensive booking and charging session management
- ✅ Analytics and reporting system
- ✅ Backup and restore functionality
- ✅ Data export capabilities
- ✅ Makefile-based build system
- ✅ Professional CLI interface

---

**Last Updated**: May 29, 2026  
**Status**: Active & Maintained  
**C++ Standard**: C++98 Compatible