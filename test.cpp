#include "IndianRailway.h"

void printFormHeader(const std::string& title) {
    std::string horizontalLine;
    for (size_t i = 0; i < title.length(); ++i) {
        horizontalLine += Style::HORIZONTAL;
    }

    std::cout << Style::YELLOW;
    std::cout << Style::TOP_LEFT << horizontalLine << Style::TOP_RIGHT << "\n";
    std::cout << Style::VERTICAL << title << Style::VERTICAL << "\n";
    std::cout << Style::BOTTOM_LEFT << horizontalLine << Style::BOTTOM_RIGHT;
    std::cout << Style::RESET << "\n\n";
}

void getStnInput(std::shared_ptr<Station<std::string>>& stn) {
    std::string id;
    printFormHeader("    Station Input Form    ");

    std::cout << Style::YELLOW << "Enter Station ID: " << Style::RESET;
    std::cin >> id;
    stn = std::make_shared<Station<std::string>>(id);
}

void getLineInput(std::shared_ptr<Line<std::string>>& line) {
    std::string id, type;
    printFormHeader("     Line Input Form     ");

    std::cout << Style::YELLOW << "Enter Line ID (e.g., RL01): " << Style::RESET;
    std::cin >> id;

    std::cout << Style::YELLOW << "Enter Line Type (Express/Passenger/Freight): " << Style::RESET;
    std::cin >> type;

    line = std::make_shared<Line<std::string>>(id, type);
}

void getPlatformInput(std::shared_ptr<Platform<std::string>>& platform) {
    std::string id;
    int freq;
    bool access;
    printFormHeader("   Platform Input Form   ");

    std::cout << Style::YELLOW << "Enter Platform ID: " << Style::RESET;
    std::cin >> id;

    std::cout << Style::YELLOW << "Enter Stop Frequency (10-30): " << Style::RESET;
    std::cin >> freq;

    std::cout << Style::YELLOW << "Is Platform Accessible? (1/0): " << Style::RESET;
    std::cin >> access;

    platform = std::make_shared<Platform<std::string>>(id, freq, access);
}

void testRailway() {
    try {
        int trainCount;
        std::cout << Style::YELLOW << "How many trains will you enter details for? " << Style::RESET;
        std::cin >> trainCount;

        if (trainCount < 2) {
            throw RwyException("At least two trains are required for conflict checking.");
        }

        struct TrainDetails {
            std::shared_ptr<Station<std::string>> station;
            std::shared_ptr<Line<std::string>> line;
            std::shared_ptr<Platform<std::string>> platform;
            std::string arrivalTime; // Arrival time in HH:MM format
            std::string trainType;   // Stoppage or Through
            bool canceled = false;
            std::string conflictReason;
        };

        std::vector<TrainDetails> trains(trainCount);

        // Collect train details
        for (int i = 0; i < trainCount; ++i) {
            std::cout << "\nTrain " << (i + 1) << " Details:\n";
            getStnInput(trains[i].station);
            getLineInput(trains[i].line);
            getPlatformInput(trains[i].platform);

            std::cout << Style::YELLOW << "Enter Train Arrival Time (HH:MM): " << Style::RESET;
            std::cin >> trains[i].arrivalTime;

            std::cout << Style::YELLOW << "Enter Train Type (Stoppage/Through): " << Style::RESET;
            std::cin >> trains[i].trainType;

            trains[i].station->addLine(trains[i].line);
            trains[i].line->addStn(trains[i].station);
            trains[i].station->addPlatform(trains[i].platform);
        }

        // Conflict resolution based on multiple real-time conditions
        for (int i = 0; i < trainCount; ++i) {
            for (int j = i + 1; j < trainCount; ++j) {
                // Check for platform conflicts
                if (trains[i].platform->getId() == trains[j].platform->getId()) {
                    int timeDiff = abs(
                        std::stoi(trains[i].arrivalTime.substr(0, 2)) * 60 +
                        std::stoi(trains[i].arrivalTime.substr(3, 2)) -
                        (std::stoi(trains[j].arrivalTime.substr(0, 2)) * 60 +
                         std::stoi(trains[j].arrivalTime.substr(3, 2)))
                    );

                    int requiredBuffer = (trains[i].trainType == "Stoppage" || trains[j].trainType == "Stoppage") ? 30 : 10;

                    if (timeDiff < requiredBuffer) {
                        if (trains[i].trainType == "Stoppage" && trains[j].trainType == "Through") {
                            trains[j].canceled = true;
                            trains[j].conflictReason = "Platform overlap, priority given to Train " + 
                                std::to_string(i + 1) + " (Stoppage over Through).";
                        } else if (trains[i].trainType == "Through" && trains[j].trainType == "Stoppage") {
                            trains[i].canceled = true;
                            trains[i].conflictReason = "Platform overlap, priority given to Train " + 
                                std::to_string(j + 1) + " (Stoppage over Through).";
                        } else {
                            // If both are the same type, cancel the later one
                            if (trains[i].arrivalTime < trains[j].arrivalTime) {
                                trains[j].canceled = true;
                                trains[j].conflictReason = "Platform overlap with Train " + 
                                    std::to_string(i + 1) + " (Same priority, later arrival).";
                            } else {
                                trains[i].canceled = true;
                                trains[i].conflictReason = "Platform overlap with Train " + 
                                    std::to_string(j + 1) + " (Same priority, later arrival).";
                            }
                        }
                    }
                }

                // Check for platform accessibility
                if (!trains[i].platform->hasAccess()) {
                    trains[i].canceled = true;
                    trains[i].conflictReason = "Assigned platform is inaccessible.";
                }

                if (!trains[j].platform->hasAccess()) {
                    trains[j].canceled = true;
                    trains[j].conflictReason = "Assigned platform is inaccessible.";
                }
            }
        }

        // Display results
        std::cout << "\n" << Style::GREEN << "Railway System Details:" << Style::RESET << "\n";
        for (int i = 0; i < trainCount; ++i) {
            if (trains[i].canceled) {
                std::cout << Style::RED << "Train " << (i + 1) << " is CANCELED.\nReason: " 
                          << trains[i].conflictReason << Style::RESET << "\n";
            } else {
                std::cout << Style::GREEN << "Train " << (i + 1) << " Details:\n" << Style::RESET;
                trains[i].station->show();
                trains[i].line->show();
                trains[i].platform->show();
                std::cout << "Arrival Time: " << trains[i].arrivalTime << "\n";
                std::cout << "Train Type: " << trains[i].trainType << "\n";
            }
        }

    } catch (const RwyException& e) {
        std::cerr << Style::RED << "Railway Error: " << e.what() << Style::RESET << "\n";
    } catch (const std::exception& e) {
        std::cerr << Style::RED << "Error: " << e.what() << Style::RESET << "\n";
    }
}

int main() {
    testRailway();
    return 0;
}
