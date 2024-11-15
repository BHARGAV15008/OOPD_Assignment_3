#include "IndianRailway.h"

void printFormHeader(const std::string& title) {
    std::string horizontalLine;
    for(size_t i = 0; i < title.length(); ++i) {
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

        std::vector<std::shared_ptr<Station<std::string>>> stations;
        std::vector<std::shared_ptr<Line<std::string>>> lines;
        std::vector<std::shared_ptr<Platform<std::string>>> platforms;

        // Collect train details
        for (int i = 0; i < trainCount; ++i) {
            std::shared_ptr<Station<std::string>> station;
            std::shared_ptr<Line<std::string>> line;
            std::shared_ptr<Platform<std::string>> platform;

            std::cout << "\nTrain " << (i + 1) << " Details:\n";
            getStnInput(station);
            getLineInput(line);
            getPlatformInput(platform);

            station->addLine(line);
            line->addStn(station);
            station->addPlatform(platform);

            stations.push_back(station);
            lines.push_back(line);
            platforms.push_back(platform);
        }

        // Conflict check with updated rules
        std::vector<std::string> conflicts;

        for (int i = 0; i < trainCount; ++i) {
            for (int j = i + 1; j < trainCount; ++j) {
                // Check for same Line ID conflict
                if (lines[i]->getId() == lines[j]->getId()) {
                    conflicts.push_back("Conflict: Train " + std::to_string(i + 1) + 
                                      " and Train " + std::to_string(j + 1) + 
                                      " have the same Line ID (" + lines[i]->getId() + ").");
                }

                // Check for Station and Platform conflicts
                if (stations[i]->getId() == stations[j]->getId() &&
                    platforms[i]->getId() == platforms[j]->getId()) {
                    
                    bool platform1Access = platforms[i]->hasAccess();
                    bool platform2Access = platforms[j]->hasAccess();

                    // Conflict only if both platforms are accessible
                    if (platform1Access && platform2Access) {
                        conflicts.push_back("Conflict: Train " + std::to_string(i + 1) + 
                                          " and Train " + std::to_string(j + 1) + 
                                          " have the same Station ID (" + stations[i]->getId() + 
                                          ") and Platform ID (" + platforms[i]->getId() + 
                                          ") with both platforms being accessible.");
                    }
                    // No conflict if either or both platforms are inaccessible
                    else {
                        std::cout << Style::YELLOW << "Note: Trains " + std::to_string(i + 1) + 
                                 " and " + std::to_string(j + 1) + 
                                 " share Station/Platform but have different accessibility settings" +
                                 " (Train " + std::to_string(i + 1) + ": " + 
                                 (platform1Access ? "accessible" : "not accessible") +
                                 ", Train " + std::to_string(j + 1) + ": " + 
                                 (platform2Access ? "accessible" : "not accessible") +
                                 ") - No conflict." << Style::RESET << "\n";
                    }
                }
            }
        }

        // Display collected conflicts (if any)
        if (!conflicts.empty()) {
            std::cout << "\n" << Style::RED << "Conflicts detected:" << Style::RESET << "\n";
            for (const auto& conflict : conflicts) {
                std::cout << Style::RED << conflict << Style::RESET << "\n";
            }
        } else {
            std::cout << "\n" << Style::GREEN << "No conflicts detected." << Style::RESET << "\n";
        }

        // Display details
        std::cout << "\n" << Style::GREEN << "Railway System Details:" << Style::RESET << "\n";
        for (int i = 0; i < trainCount; ++i) {
            std::cout << "\nStation " << i + 1 << " Details:\n";
            stations[i]->show();
            std::cout << "\nLine " << i + 1 << " Details:\n";
            lines[i]->show();
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