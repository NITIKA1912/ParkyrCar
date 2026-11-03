#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_SLOTS 10
#define NUMBER_LENGTH 20
#define DATA_FILE "parking.dat"

typedef struct {
    char vehicleNumber[NUMBER_LENGTH];
    int vehicleType;       /* 1 = Bike, 2 = Car, 3 = Truck */
    time_t entryTime;
    int occupied;
} Vehicle;

Vehicle parking[MAX_SLOTS];

/* Function declarations */
void initializeParking(void);
void loadData(void);
void saveData(void);
void displayMenu(void);
void showSlots(void);
void parkVehicle(void);
void removeVehicle(void);
void searchVehicle(void);
void showReport(void);
int findVehicle(const char number[]);
int getFreeSlot(void);
int getVehicleType(void);
void readVehicleNumber(char number[]);
double calculateFee(int type, time_t entry, time_t exitTime);
void clearInput(void);
void printVehicleType(int type);

int main(void) {
    int choice;

    initializeParking();
    loadData();

    do {
        displayMenu();
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInput();
            continue;
        }
        clearInput();

        switch (choice) {
            case 1:
                showSlots();
                break;
            case 2:
                parkVehicle();
                break;
            case 3:
                removeVehicle();
                break;
            case 4:
                searchVehicle();
                break;
            case 5:
                showReport();
                break;
            case 6:
                saveData();
                printf("Data saved. Thank you!\n");
                break;
            default:
                printf("Invalid choice. Select 1 to 6.\n");
        }
        printf("/nPress Enter to continue...");
        getchar();
    } while (choice != 6);

    return 0;
}

void initializeParking(void) {
    int i;

    for (i = 0; i < MAX_SLOTS; i++) {
        parking[i].vehicleNumber[0] = '\0';
        parking[i].vehicleType = 0;
        parking[i].entryTime = (time_t)0;
        parking[i].occupied = 0;
    }
}

void loadData(void) {
    FILE *fp = fopen(DATA_FILE, "rb");

    if (fp == NULL) {
        printf("No previous data found. Starting fresh.\n");
        return;
    }

    if (fread(parking, sizeof(Vehicle), MAX_SLOTS, fp)
        != MAX_SLOTS) {
        printf("Warning: Saved data could not be fully loaded.\n");
        initializeParking();
    } else {
        printf("Previous parking records loaded.\n");
    }

    fclose(fp);
}

void saveData(void) {
    FILE *fp = fopen(DATA_FILE, "wb");

    if (fp == NULL) {
        printf("Error: Unable to save parking data.\n");
        return;
    }

    if (fwrite(parking, sizeof(Vehicle), MAX_SLOTS, fp)
        != MAX_SLOTS) {
        printf("Error while writing parking data.\n");
    }

    fclose(fp);
}

void displayMenu(void) {
    printf("\n====================================\n");
    printf("    SMART PARKING MANAGEMENT SYSTEM\n");
    printf("====================================\n");
    printf("1. View parking slots\n");
    printf("2. Park a vehicle\n");
    printf("3. Remove a vehicle and calculate fee\n");
    printf("4. Search for a vehicle\n");
    printf("5. Parking report\n");
    printf("6. Save data and exit\n");
    printf("====================================\n");
}

void printVehicleType(int type) {
    switch (type) {
        case 1: printf("Bike"); break;
        case 2: printf("Car"); break;
        case 3: printf("Truck"); break;
        default: printf("Unknown"); break;
    }
}

void showSlots(void) {
    int i, available = 0;

    printf("\n--- PARKING SLOT STATUS ---\n");

    for (i = 0; i < MAX_SLOTS; i++) {
        printf("Slot %d: ", i + 1);

        if (parking[i].occupied) {
            printf("Occupied | %s | ",
                   parking[i].vehicleNumber);
            printVehicleType(parking[i].vehicleType);
            printf("\n");
        } else {
            printf("Available\n");
            available++;
        }
    }

    printf("Available slots: %d/%d\n",
           available, MAX_SLOTS);
}

void readVehicleNumber(char number[]) {
    size_t i, len;

    printf("Enter vehicle registration number: ");
    fgets(number, NUMBER_LENGTH, stdin);

    len = strlen(number);
    if (len > 0 && number[len - 1] == '\n') {
        number[len - 1] = '\0';
    } else {
        clearInput();
    }

    /* Convert registration to uppercase */
    for (i = 0; number[i] != '\0'; i++) {
        number[i] = (char)toupper((unsigned char)number[i]);
    }
}

int getVehicleType(void) {
    int type;

    printf("Vehicle type:\n");
    printf("1. Bike (Rs. 20 per hour)\n");
    printf("2. Car  (Rs. 50 per hour)\n");
    printf("3. Truck (Rs. 80 per hour)\n");
    printf("Select type: ");

    if (scanf("%d", &type) != 1) {
        clearInput();
        return 0;
    }
    clearInput();

    if (type < 1 || type > 3) {
        return 0;
    }

    return type;
}

int findVehicle(const char number[]) {
    int i;

    for (i = 0; i < MAX_SLOTS; i++) {
        if (parking[i].occupied &&
            strcmp(parking[i].vehicleNumber, number) == 0) {
            return i;
        }
    }

    return -1;
}

int getFreeSlot(void) {
    int i;

    for (i = 0; i < MAX_SLOTS; i++) {
        if (!parking[i].occupied) {
            return i;
        }
    }

    return -1;
}

void parkVehicle(void) {
    char number[NUMBER_LENGTH];
    int slot, type;

    slot = getFreeSlot();

    if (slot == -1) {
        printf("Parking is full. No slots available.\n");
        return;
    }

    readVehicleNumber(number);

    if (number[0] == '\0') {
        printf("Registration number cannot be empty.\n");
        return;
    }

    if (findVehicle(number) != -1) {
        printf("This vehicle is already parked.\n");
        return;
    }

    type = getVehicleType();

    if (type == 0) {
        printf("Invalid vehicle type.\n");
        return;
    }

    strncpy(parking[slot].vehicleNumber, number,
            NUMBER_LENGTH - 1);
    parking[slot].vehicleNumber[NUMBER_LENGTH - 1] = '\0';
    parking[slot].vehicleType = type;
    parking[slot].entryTime = time(NULL);
    parking[slot].occupied = 1;

    saveData();

    printf("\nVehicle parked successfully!\n");
    printf("Assigned slot: %d\n", slot + 1);
    printf("Entry time: %s", ctime(&parking[slot].entryTime));
}

double calculateFee(int type, time_t entry, time_t exitTime) {
    double seconds = difftime(exitTime, entry);
    int hours = (int)(seconds / 3600.0);

    /* Round any partial hour up; minimum charge is one hour */
    if (seconds > hours * 3600.0) {
        hours++;
    }
    if (hours < 1) {
        hours = 1;
    }

    switch (type) {
        case 1: return hours * 20.0;
        case 2: return hours * 50.0;
        case 3: return hours * 80.0;
        default: return 0.0;
    }
}

void removeVehicle(void) {
    char number[NUMBER_LENGTH];
    int index, slot;
    time_t exitTime;
    double fee;

    readVehicleNumber(number);
    index = findVehicle(number);

    if (index == -1) {
        printf("Vehicle not found in the parking lot.\n");
        return;
    }

    exitTime = time(NULL);
    fee = calculateFee(parking[index].vehicleType,
                       parking[index].entryTime, exitTime);
    slot = index + 1;

    printf("\n--- PARKING BILL ---\n");
    printf("Vehicle: %s\n", parking[index].vehicleNumber);
    printf("Slot: %d\n", slot);
    printf("Type: ");
    printVehicleType(parking[index].vehicleType);
    printf("\nEntry time: %s",
           ctime(&parking[index].entryTime));
    printf("Exit time: %s", ctime(&exitTime));
    printf("Parking fee: Rs. %.2f\n", fee);

    parking[index].occupied = 0;
    parking[index].vehicleNumber[0] = '\0';
    parking[index].vehicleType = 0;
    parking[index].entryTime = (time_t)0;

    saveData();
    printf("Vehicle removed. Slot %d is now available.\n", slot);
}

void searchVehicle(void) {
    char number[NUMBER_LENGTH];
    int index;

    readVehicleNumber(number);
    index = findVehicle(number);

    if (index == -1) {
        printf("Vehicle not found.\n");
    } else {
        printf("\nVehicle found!\n");
        printf("Registration: %s\n",
               parking[index].vehicleNumber);
        printf("Slot: %d\n", index + 1);
        printf("Type: ");
        printVehicleType(parking[index].vehicleType);
        printf("\nEntry time: %s",
               ctime(&parking[index].entryTime));
    }
}

void showReport(void) {
    int i, occupied = 0;

    printf("\n--- PARKING REPORT ---\n");

    for (i = 0; i < MAX_SLOTS; i++) {
        if (parking[i].occupied) {
            occupied++;
        }
    }

    printf("Total slots: %d\n", MAX_SLOTS);
    printf("Occupied slots: %d\n", occupied);
    printf("Available slots: %d\n", MAX_SLOTS - occupied);
    printf("Occupancy: %.1f%%\n",
           occupied * 100.0 / MAX_SLOTS);
}

void clearInput(void) {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remaining input */
    }
}
