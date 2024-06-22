//410840 Fadeel Abuobead
//402515 Büşra Yıldız
//425510 Ahmed Buğra TİNYOZOĞLU

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_NAME_LENGTH 50
#define MAX_MEALS 100
#define MAX_ORDERS 1000

typedef struct {
    char name[MAX_NAME_LENGTH];
    float price;
    int prep_time;
    int available;
} Meal;

typedef struct {
    char id[MAX_NAME_LENGTH];
    char meal_name[MAX_NAME_LENGTH];
    float price;
    char order_time[MAX_NAME_LENGTH];
    char start_time[MAX_NAME_LENGTH];
    char end_time[MAX_NAME_LENGTH];
    char user[MAX_NAME_LENGTH];
    int status;  // 0: waiting, 1: approved, 2: completed, -1: rejected, 3: preparing
    int chef_id;
} Order;

Meal meal_list[MAX_MEALS];
Order order_list[MAX_ORDERS];
int meal_count = 0;
int order_count = 0;
int chef_count = 3;
int chef_busy[3] = {0}; // Keeps track of which chefs are busy

void load_meals(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening meal list file.\n");
        return;
    }
    meal_count = 0;
    while (fscanf(file, "%s %f %d %d", meal_list[meal_count].name, &meal_list[meal_count].price,
                  &meal_list[meal_count].prep_time, &meal_list[meal_count].available) != EOF) {
        meal_count++;
    }
    fclose(file);
}

void save_meals(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error opening meal list file.\n");
        return;
    }
    for (int i = 0; i < meal_count; i++) {
        fprintf(file, "%s %.2f %d %d\n", meal_list[i].name, meal_list[i].price, meal_list[i].prep_time, meal_list[i].available);
    }
    fclose(file);
}

void load_orders(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening orders file.\n");
        return;
    }
    order_count = 0;
    while (fscanf(file, "%s %s %f %s %s %s %s %d %d", order_list[order_count].id, order_list[order_count].meal_name,
                  &order_list[order_count].price, order_list[order_count].order_time,
                  order_list[order_count].start_time, order_list[order_count].end_time, order_list[order_count].user,
                  &order_list[order_count].status, &order_list[order_count].chef_id) != EOF) {
        order_count++;
    }
    fclose(file);
}

void save_orders(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error creating orders file.\n");
        return;
    }
    for (int i = 0; i < order_count; i++) {
        fprintf(file, "%s %s %.2f %s %s %s %s %d %d\n", order_list[i].id, order_list[i].meal_name,
                order_list[i].price, order_list[i].order_time, order_list[i].start_time,
                order_list[i].end_time, order_list[i].user, order_list[i].status, order_list[i].chef_id);
    }
    fclose(file);
}

char *generate_order_id() {
    static char id[MAX_NAME_LENGTH];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int day = t->tm_mday;
    int month = t->tm_mon + 1; // tm_mon is 0-based
    int year = t->tm_year % 100; // tm_year is years since 1900
    snprintf(id, MAX_NAME_LENGTH, "SIP%02d%02d%02d%03d", day, month, year, (order_count + 1) % 1000);
    return id;
}

void get_current_time(char *buffer, size_t buffer_size) {
    time_t now = time(NULL);
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

const char* get_status_description(int status) {
    switch (status) {
        case 0: return "Waiting";
        case 1: return "Approved";
        case 2: return "Completed";
        case -1: return "Rejected";
        case 3: return "Preparing";
        default: return "Unknown";
    }
}

void view_menu_and_place_order() {
    printf("Menu:\n");
    for (int i = 0; i < meal_count; i++) {
        if (meal_list[i].available) {
            printf("%d. %s - %.2f TL - Preparation Time: %d minutes\n",
                   i + 1, meal_list[i].name, meal_list[i].price, meal_list[i].prep_time);
        }
    }

    int choice;
    printf("Enter the number of the meal you want to order (0 to cancel): ");
    scanf("%d", &choice);

    if (choice > 0 && choice <= meal_count && meal_list[choice - 1].available) {
        Order new_order;
        strcpy(new_order.id, generate_order_id());
        strcpy(new_order.meal_name, meal_list[choice - 1].name);
        new_order.price = meal_list[choice - 1].price;
        get_current_time(new_order.order_time, MAX_NAME_LENGTH);
        strcpy(new_order.start_time, ""); // Start time not set yet
        strcpy(new_order.end_time, ""); // End time not set yet
        printf("Enter your name: ");
        scanf("%s", new_order.user);
        new_order.status = 0;
        new_order.chef_id = -1; // Chef not assigned yet

        order_list[order_count] = new_order;
        order_count++;
        printf("Order placed successfully!\n");

        save_orders("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\S.txt");
    } else if (choice == 0) {
        printf("Order cancelled.\n");
    } else {
        printf("Invalid choice or meal not available. Please try again.\n");
    }
}

void view_current_order_status(const char *user_name) {
    int found = 0;
    for (int i = 0; i < order_count; i++) {
        if (strcmp(order_list[i].user, user_name) == 0 && (order_list[i].status == 0 || order_list[i].status == 1 || order_list[i].status == 3)) {
            printf("Current Order ID: %s\n", order_list[i].id);
            printf("Meal: %s\n", order_list[i].meal_name);
            printf("Price: %.2f TL\n", order_list[i].price);
            printf("Order Time: %s\n", order_list[i].order_time);
            if (order_list[i].status == 3) {
                printf("Start Time: %s\n", order_list[i].start_time);
            }
            printf("Status: %s\n", get_status_description(order_list[i].status));
            if (order_list[i].status == 1 || order_list[i].status == 3) {
                printf("Chef ID: %d\n", order_list[i].chef_id);
            }
            found = 1;
            break;
        }
    }
    if (!found) {
        printf("No current orders found.\n");
    }
}

void view_previous_orders(const char *user_name) {
    int found = 0;
    for (int i = 0; i < order_count; i++) {
        if (strcmp(order_list[i].user, user_name) == 0 && order_list[i].status == 2) {
            printf("Order ID: %s\n", order_list[i].id);
            printf("Meal: %s\n", order_list[i].meal_name);
            printf("Price: %.2f TL\n", order_list[i].price);
            printf("Order Time: %s\n", order_list[i].order_time);
            printf("Start Time: %s\n", order_list[i].start_time);
            printf("End Time: %s\n", order_list[i].end_time);
            printf("Status: %s\n", get_status_description(order_list[i].status));
            printf("Chef ID: %d\n", order_list[i].chef_id);
            found = 1;
        }
    }
    if (!found) {
        printf("No previous orders found.\n");
    }
}

void add_food() {
    if (meal_count >= MAX_MEALS) {
        printf("Cannot add more meals. Maximum limit reached.\n");
        return;
    }
    Meal new_meal;
    printf("Enter food name: ");
    scanf("%s", new_meal.name);
    printf("Enter price: ");
    scanf("%f", &new_meal.price);
    printf("Enter preparation time (minutes): ");
    scanf("%d", &new_meal.prep_time);
    new_meal.available = 1; // New meals are available by default
    meal_list[meal_count++] = new_meal;
    save_meals("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\yemeklistesi.txt");
    printf("Food added successfully.\n");
}

void update_food() {
    char name[MAX_NAME_LENGTH];
    printf("Enter the food name to update: ");
    scanf("%s", name);
    for (int i = 0; i < meal_count; i++) {
        if (strcmp(meal_list[i].name, name) == 0) {
            printf("Enter new price: ");
            scanf("%f", &meal_list[i].price);
            printf("Enter new preparation time (minutes): ");
            scanf("%d", &meal_list[i].prep_time);
            printf("Enter availability (1 for available, 0 for not available): ");
            scanf("%d", &meal_list[i].available);
            save_meals("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\yemeklistesi.txt");
            printf("Food updated successfully.\n");
            return;
        }
    }
    printf("Food not found.\n");
}

void delete_food() {
    char name[MAX_NAME_LENGTH];
    printf("Enter the food name to delete: ");
    scanf("%s", name);
    for (int i = 0; i < meal_count; i++) {
        if (strcmp(meal_list[i].name, name) == 0) {
            for (int j = i; j < meal_count - 1; j++) {
                meal_list[j] = meal_list[j + 1];
            }
            meal_count--;
            save_meals("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\yemeklistesi.txt");
            printf("Food deleted successfully.\n");
            return;
        }
    }
    printf("Food not found.\n");
}

void approve_or_reject_orders() {
    int unapproved_count = 0;
    for (int i = 0; i < order_count; i++) {
        if (order_list[i].status == 0) { // Only consider waiting orders
            printf("%d. Order ID: %s, Meal: %s, User: %s\n", unapproved_count + 1, order_list[i].id, order_list[i].meal_name, order_list[i].user);
            unapproved_count++;
        }
    }

    if (unapproved_count == 0) {
        printf("No unapproved orders found.\n");
        return;
    }

    int choice;
    printf("Enter the number of the order to approve/reject (0 to cancel): ");
    scanf("%d", &choice);

    if (choice > 0 && choice <= unapproved_count) {
        int index = -1;
        unapproved_count = 0;
        for (int i = 0; i < order_count; i++) {
            if (order_list[i].status == 0) {
                unapproved_count++;
                if (unapproved_count == choice) {
                    index = i;
                    break;
                }
            }
        }

        if (index != -1) {
            printf("Approve (1) or Reject (0) this order: ");
            int decision;
            scanf("%d", &decision);
            if (decision == 1) {
                order_list[index].status = 1; // Approved
                printf("Order approved.\n");
            } else if (decision == 0) {
                order_list[index].status = -1; // Rejected
                printf("Order rejected.\n");
            } else {
                printf("Invalid decision.\n");
            }
        }
    } else if (choice == 0) {
        printf("Action cancelled.\n");
    } else {
        printf("Invalid choice. Please try again.\n");
    }

    save_orders("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\S.txt");
}

void generate_daily_report() {
    char date[11];
    printf("Enter the date (YYYY-MM-DD) for the report: ");
    scanf("%s", date);
    char filename[200];
    snprintf(filename, sizeof(filename), "C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\daily_report_%s.txt", date);

    FILE *report_file = fopen(filename, "w");
    if (!report_file) {
        printf("Error creating report file.\n");
        return;
    }

    fprintf(report_file, "Order ID         Food Name    Food Price    Order Time           Preparation Time  User   Chef\n");

    double total_earnings = 0.0;
    int user_order_count[MAX_ORDERS] = {0};
    char users[MAX_ORDERS][MAX_NAME_LENGTH];
    int unique_user_count = 0;

    for (int i = 0; i < order_count; i++) {
        if (strncmp(order_list[i].order_time, date, 10) == 0) { // Match the date
            double prep_time_diff = 0.0;
            if (strcmp(order_list[i].start_time, "") != 0 && strcmp(order_list[i].end_time, "") != 0) {
                struct tm start_tm = {0}, end_tm = {0};

                sscanf(order_list[i].start_time, "%d-%d-%d %d:%d:%d",
                       &start_tm.tm_year, &start_tm.tm_mon, &start_tm.tm_mday,
                       &start_tm.tm_hour, &start_tm.tm_min, &start_tm.tm_sec);
                sscanf(order_list[i].end_time, "%d-%d-%d %d:%d:%d",
                       &end_tm.tm_year, &end_tm.tm_mon, &end_tm.tm_mday,
                       &end_tm.tm_hour, &end_tm.tm_min, &end_tm.tm_sec);

                start_tm.tm_year -= 1900;
                start_tm.tm_mon -= 1;
                end_tm.tm_year -= 1900;
                end_tm.tm_mon -= 1;

                time_t start_time = mktime(&start_tm);
                time_t end_time = mktime(&end_tm);
                prep_time_diff = difftime(end_time, start_time) / 60.0; // Difference in minutes
            }
            fprintf(report_file, "%-16s %-12s %-12.2f %-19s %-18.2f %-6s A%d\n", order_list[i].id, order_list[i].meal_name,
                    order_list[i].price, order_list[i].order_time, prep_time_diff,
                    order_list[i].user, order_list[i].chef_id);
            total_earnings += order_list[i].price;

            // Track user orders
            int user_found = 0;
            for (int j = 0; j < unique_user_count; j++) {
                if (strcmp(users[j], order_list[i].user) == 0) {
                    user_order_count[j]++;
                    user_found = 1;
                    break;
                }
            }
            if (!user_found) {
                strcpy(users[unique_user_count], order_list[i].user);
                user_order_count[unique_user_count]++;
                unique_user_count++;
            }
        }
    }

    // Determine the most ordered user
    int max_orders = 0;
    char most_ordered_user[MAX_NAME_LENGTH] = "None";
    for (int i = 0; i < unique_user_count; i++) {
        if (user_order_count[i] > max_orders) {
            max_orders = user_order_count[i];
            strcpy(most_ordered_user, users[i]);
        }
    }

    fprintf(report_file, "\nTotal Earnings for %s: %.2f TL\n", date, total_earnings);
    fprintf(report_file, "Most Ordered User: %s with %d orders\n", most_ordered_user, max_orders);

    fclose(report_file);
    printf("Daily report generated: %s\n", filename);
}

void generate_periodic_report() {
    char start_date[11], end_date[11];
    printf("Enter the start date (YYYY-MM-DD): ");
    scanf("%s", start_date);
    printf("Enter the end date (YYYY-MM-DD): ");
    scanf("%s", end_date);

    struct tm start_tm = {0}, end_tm = {0};
    sscanf(start_date, "%d-%d-%d", &start_tm.tm_year, &start_tm.tm_mon, &start_tm.tm_mday);
    sscanf(end_date, "%d-%d-%d", &end_tm.tm_year, &end_tm.tm_mon, &end_tm.tm_mday);

    start_tm.tm_year -= 1900;
    start_tm.tm_mon -= 1;
    end_tm.tm_year -= 1900;
    end_tm.tm_mon -= 1;

    time_t start_time = mktime(&start_tm);
    time_t end_time = mktime(&end_tm);

    double total_earnings = 0.0;

    for (int i = 0; i < order_count; i++) {
        struct tm order_tm = {0};
        sscanf(order_list[i].order_time, "%d-%d-%d %d:%d:%d", &order_tm.tm_year, &order_tm.tm_mon, &order_tm.tm_mday,
               &order_tm.tm_hour, &order_tm.tm_min, &order_tm.tm_sec);

        order_tm.tm_year -= 1900;
        order_tm.tm_mon -= 1;

        time_t order_time = mktime(&order_tm);

        if (difftime(order_time, start_time) >= 0 && difftime(end_time, order_time) >= 0) {
            total_earnings += order_list[i].price;
        }
    }

    printf("Total Earnings from %s to %s: %.2f TL\n", start_date, end_date, total_earnings);
}

void choose_order_to_prepare() {
    printf("Available orders for preparation:\n");
    int available_count = 0;
    for (int i = 0; i < order_count; i++) {
        if (order_list[i].status == 1 && order_list[i].chef_id == -1) { // Only approved orders without a chef assigned
            printf("%d. Order ID: %s, Meal: %s, User: %s\n", available_count + 1, order_list[i].id, order_list[i].meal_name, order_list[i].user);
            available_count++;
        }
    }

    if (available_count == 0) {
        printf("No orders available for preparation.\n");
        return;
    }

    int choice;
    printf("Enter the number of the order to prepare (0 to cancel): ");
    scanf("%d", &choice);

    if (choice > 0 && choice <= available_count) {
        int index = -1;
        available_count = 0;
        for (int i = 0; i < order_count; i++) {
            if (order_list[i].status == 1 && order_list[i].chef_id == -1) {
                available_count++;
                if (available_count == choice) {
                    index = i;
                    break;
                }
            }
        }

        if (index != -1) {
            printf("Enter the chef ID (1 to %d) to prepare this order: ", chef_count);
            int chef_id;
            scanf("%d", &chef_id);
            if (chef_id > 0 && chef_id <= chef_count && chef_busy[chef_id - 1] == 0) {
                order_list[index].chef_id = chef_id;
                order_list[index].status = 3; // Preparing
                get_current_time(order_list[index].start_time, MAX_NAME_LENGTH); // Record start time
                chef_busy[chef_id - 1] = 1; // Mark chef as busy
                printf("Order ID: %s is being prepared by Chef %d.\n", order_list[index].id, chef_id);
                save_orders("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\S.txt");
            } else {
                printf("Invalid chef ID or chef is already busy. Skipping this order.\n");
            }
        }
    } else if (choice == 0) {
        printf("Action cancelled.\n");
    } else {
        printf("Invalid choice. Please try again.\n");
    }
}

void complete_order_preparation() {
    int chef_id;
    printf("Enter your chef ID (1 to %d): ", chef_count);
    scanf("%d", &chef_id);

    if (chef_id < 1 || chef_id > chef_count) {
        printf("Invalid chef ID. Please try again.\n");
        return;
    }

    int found = 0;
    for (int i = 0; i < order_count; i++) {
        if (order_list[i].status == 3 && order_list[i].chef_id == chef_id) { // Only orders being prepared by this chef
            printf("Order ID: %s is completed by Chef %d.\n", order_list[i].id, order_list[i].chef_id);
            order_list[i].status = 2; // Completed
            get_current_time(order_list[i].end_time, MAX_NAME_LENGTH); // Record end time
            chef_busy[chef_id - 1] = 0; // Mark chef as available
            found = 1;
        }
    }

    if (!found) {
        printf("Chef %d, you are not preparing anything now.\n", chef_id);
    } else {
        save_orders("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\S.txt");
    }
}

void kitchen_application() {
    int choice;
    do {
        printf("\n1. Choose Order to Prepare\n2. Complete Order Preparation\n3. Back to Main Menu\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                choose_order_to_prepare();
                break;
            case 2:
                complete_order_preparation();
                break;
            case 3:
                printf("Returning to main menu.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 3);
}

void customer_application() {
    int choice;
    char user_name[MAX_NAME_LENGTH];

    printf("Enter your name: ");
    scanf("%s", user_name);

    do {
        printf("\n1. New Order\n2. Current Order Status\n3. My Previous Orders\n4. Back to Main Menu\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                view_menu_and_place_order();
                break;
            case 2:
                view_current_order_status(user_name);
                break;
            case 3:
                view_previous_orders(user_name);
                break;
            case 4:
                printf("Returning to main menu.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 4);
}

void restaurant_application() {
    int choice;
    do {
        printf("\n1. Add Food\n2. Update Food\n3. Delete Food\n4. Approve/Reject Orders\n5. Generate Daily Report\n6. Generate Periodic Earnings Report\n7. Back to Main Menu\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                add_food();
                break;
            case 2:
                update_food();
                break;
            case 3:
                delete_food();
                break;
            case 4:
                approve_or_reject_orders();
                break;
            case 5:
                generate_daily_report();
                break;
            case 6:
                generate_periodic_report();
                break;
            case 7:
                printf("Returning to main menu.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 7);
}

int main() {
    load_meals("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\yemeklistesi.txt");
    load_orders("C:\\Users\\fadee\\OneDrive\\Documents\\Com. Eng. Books\\C\\RR\\S.txt");

    int choice;
    do {
        printf("\n1. Customer Application\n2. Restaurant Application\n3. Kitchen Application\n4. Exit\nChoose an application: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                customer_application();
                break;
            case 2:
                restaurant_application();
                break;
            case 3:
                kitchen_application();
                break;
            case 4:
                printf("Exiting program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 4);

    return 0;
}
