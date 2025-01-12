// AntFarmSimulation.cpp
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <algorithm>

using namespace std;

// Forward declarations
class Ant;
class Room;

// Base class for ants
class Ant {
protected:
    int health;
    int strength;
    bool isResting;

public:
    Ant(int h, int s) : health(h), strength(s), isResting(false) {}
    virtual ~Ant() = default;

    virtual void action() = 0;

    // Battle mechanism
    bool battle(Ant& opponent) {
        if (strength > opponent.strength) {
            health += opponent.strength / 2; // Gain half of opponent’s strength
            return true; // Winner
        } else {
            opponent.health += strength / 2; // Opponent gains half of this ant’s strength
            return false; // Loser
        }
    }

    void rest() { isResting = true; }
    void recover() { isResting = false; }
    bool resting() const { return isResting; }
    bool isAlive() const { return health > 0; }
};

class WorkerAnt : public Ant {
public:
    WorkerAnt() : Ant(50, 10) {}
    void action() override { cout << "WorkerAnt is working.\n"; }
};

class SoldierAnt : public Ant {
public:
    SoldierAnt() : Ant(70, 30) {}
    void action() override { cout << "SoldierAnt is patrolling.\n"; }
};

class QueenAnt : public Ant {
public:
    QueenAnt() : Ant(100, 50) {}
    void action() override { cout << "QueenAnt is commanding the colony.\n"; }
};

// Room for ants
class Room {
    string name;
    string species;
    vector<shared_ptr<Ant>> ants;
    bool underConstruction;
    int buildProgress;
    int requiredTicks;

public:
    Room() : underConstruction(false), buildProgress(0), requiredTicks(0) {} // Default constructor
    Room(const string& n, const string& s, int ticks)
        : name(n), species(s), underConstruction(true), buildProgress(0), requiredTicks(ticks) {}

    // Enable move semantics
    Room(Room&& other) noexcept = default;
    Room& operator=(Room&& other) noexcept = default;

    void addAnt(shared_ptr<Ant> ant) {
        if (species.empty()) {
            species = typeid(*ant).name(); // Set species based on first ant
        }
        if (typeid(*ant).name() == species) {
            ants.push_back(ant);
        } else {
            cerr << "Error: Cannot add ants of a different species to this room.\n";
        }
    }

    void contributeToBuilding() {
        if (underConstruction) {
            buildProgress++;
            if (buildProgress >= requiredTicks) {
                underConstruction = false;
                cout << "Room " << name << " has been completed.\n";
            }
        }
    }

    bool isComplete() const { return !underConstruction; }

    const vector<shared_ptr<Ant>>& getAnts() const { return ants; }
    const string& getSpecies() const { return species; }
};

// AntFarm
class AntFarm {
    map<string, Room> rooms;
    string species;
    int food;

public:
    AntFarm(const string& s) : species(s), food(100) {}

    void addRoom(const string& name, int buildTicks) {
        rooms.emplace(name, Room(name, species, buildTicks));
    }

    void addAntToRoom(const string& roomName, shared_ptr<Ant> ant) {
        auto it = rooms.find(roomName);
        if (it != rooms.end()) {
            it->second.addAnt(ant);
        } else {
            cerr << "Error: Room " << roomName << " does not exist.\n";
        }
    }

    void contributeToRoom(const string& roomName) {
        auto it = rooms.find(roomName);
        if (it != rooms.end()) {
            it->second.contributeToBuilding();
        } else {
            cerr << "Error: Room " << roomName << " does not exist.\n";
        }
    }

    void feedAnts() {
        if (food <= 0) {
            cerr << "No food available. Ants are starving.\n";
        } else {
            food -= rooms.size(); // Deduct food based on the number of rooms
            cout << "Ants have been fed. Remaining food: " << food << "\n";
        }
    }
};

// Simulation Manager
class SimulationManager {
    vector<shared_ptr<AntFarm>> antFarms;

public:
    void addAntFarm(shared_ptr<AntFarm> farm) {
        antFarms.push_back(farm);
    }

    void simulateTick() {
        for (auto& farm : antFarms) {
            farm->feedAnts();
        }
        checkActiveColonies();
    }

    void checkActiveColonies() {
        int activeColonies = count_if(antFarms.begin(), antFarms.end(), [](const shared_ptr<AntFarm>& farm) {
            return farm != nullptr;
        });

        if (activeColonies <= 1) {
            cout << "Simulation ends. Only one active colony remains.\n";
            exit(0);
        }
    }
};

int main() {
    auto farm1 = make_shared<AntFarm>("RedAnts");
    farm1->addRoom("Room1", 5);
    farm1->addAntToRoom("Room1", make_shared<WorkerAnt>());

    auto farm2 = make_shared<AntFarm>("BlackAnts");
    farm2->addRoom("Room2", 5);
    farm2->addAntToRoom("Room2", make_shared<SoldierAnt>());

    SimulationManager manager;
    manager.addAntFarm(farm1);
    manager.addAntFarm(farm2);

    for (int i = 0; i < 10; ++i) {
        cout << "Tick: " << i + 1 << "\n";
        manager.simulateTick();
    }

    return 0;
}
