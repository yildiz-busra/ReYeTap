#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    char prep_time[MAX_NAME_LENGTH];
    char user[MAX_NAME_LENGTH];
    int status;  // 0: waiting, 1: approved, -1: rejected
} Order;

Meal meal_list[MAX_MEALS];
Order order_list[MAX_ORDERS];
int meal_count = 0;
int order_count = 0;
int chef_count = 3;

void load_meals() {
    FILE *file = fopen("yemeklistesi.txt", "r");
    if (!file) {
        printf("Error opening meal list file.\n");
        return;
    }
    while (fscanf(file, "%s %f %d %d", meal_list[meal_count].name, &meal_list[meal_count].price,
                  &meal_list[meal_count].prep_time, &meal_list[meal_count].available) != EOF) {
        meal_count++;
    }
    fclose(file);
}

void save_meals() {
    FILE *file = fopen("yemeklistesi.txt", "w");
    if (!file) {
        printf("Error opening meal list file.\n");
        return;
    }
    for (int i = 0; i < meal_count; i++) {
        fprintf(file, "%s %.2f %d %d\n", meal_list[i].name, meal_list[i].price, meal_list[i].prep_time, meal_list[i].available);
    }
    fclose(file);
}

void load_orders() {
    FILE *file = fopen("siparisler.txt", "r");
    if (!file) {
        printf("Error opening orders file.\n");
        return;
    }
    while (fscanf(file, "%s %s %f %s %s %s %d", order_list[order_count].id, order_list[order_count].meal_name, 
                  &order_list[order_count].price, order_list[order_count].order_time, 
                  order_list[order_count].prep_time, order_list[order_count].user, 
                  &order_list[order_count].status) != EOF) {
        order_count++;
    }
    fclose(file);
}

void save_orders() {
    FILE *file = fopen("siparisler.txt", "w");
    if (!file) {
        printf("Error opening orders file.\n");
        return;
    }
    for (int i = 0; i < order_count; i++) {
        fprintf(file, "%s %s %.2f %s %s %s %d\n", order_list[i].id, order_list[i].meal_name, order_list[i].price, 
                order_list[i].order_time, order_list[i].prep_time, order_list[i].user, order_list[i].status);
    }
    fclose(file);
}

void save_order(Order *order) {
    FILE *file = fopen("siparisler.txt", "a");
    if (!file) {
        printf("Error opening orders file.\n");
        return;
    }
    fprintf(file, "%s %s %.2f %s %s %s %d\n", order->id, order->meal_name, order->price, order->order_time,
            order->prep_time, order->user, order->status);
    fclose(file);
}

void place_order(char *user) {
    char meal_name[MAX_NAME_LENGTH];
    int meal_index = -1;
    printf("Enter meal name to order: ");
    scanf("%s", meal_name);

    for (int i = 0; i < meal_count; i++) {
        if (strcmp(meal_list[i].name, meal_name) == 0 && meal_list[i].available) {
            meal_index = i;
            break;
        }
    }

    if (meal_index == -1) {
        printf("Meal not available.\n");
        return;
    }

    Order order;
    snprintf(order.id, sizeof(order.id), "SIP%ld_%03d", time(NULL), order_count + 1);
    strcpy(order.meal_name, meal_list[meal_index].name);
    order.price = meal_list[meal_index].price;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(order.order_time, sizeof(order.order_time) - 1, "%d-%m-%Y %H:%M", t);
    strftime(order.prep_time, sizeof(order.prep_time) - 1, "%d-%m-%Y %H:%M", t);
    strcpy(order.user, user);
    order.status = 0;

    order_list[order_count++] = order;
    save_order(&order);

    printf("Order placed: %s, %s, %.2f TL\n", order.id, order.meal_name, order.price);
}

void view_orders(char *user) {
    for (int i = 0; i < order_count; i++) {
        if (strcmp(order_list[i].user, user) == 0) {
            printf("Order ID: %s, Meal: %s, Price: %.2f TL, Order Time: %s, Preparation Time: %s, Status: %d\n",
                   order_list[i].id, order_list[i].meal_name, order_list[i].price, order_list[i].order_time,
                   order_list[i].prep_time, order_list[i].status);
        }
    }
}

void view_menu() {
    for (int i = 0; i < meal_count; i++) {
        if (meal_list[i].available) {
            printf("Meal: %s, Price: %.2f TL, Preparation Time: %d minutes\n",
                   meal_list[i].name, meal_list[i].price, meal_list[i].prep_time);
        }
    }
}

void customer_application() {
    char user[MAX_NAME_LENGTH];
    printf("Enter your username: ");
    scanf("%s", user);

    int choice;
    do {
        printf("\n1. Place Order\n2. View Orders\n3. View Menu\n4. Exit\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                place_order(user);
                break;
            case 2:
                view_orders(user);
                break;
            case 3:
                view_menu();
                break;
            case 4:
                printf("Exiting customer application.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 4);
}

void add_meal() {
    Meal meal;
    printf("Enter meal name: ");
    scanf("%s", meal.name);
    printf("Enter meal price: ");
    scanf("%f", &meal.price);
    printf("Enter preparation time: ");
    scanf("%d", &meal.prep_time);
    printf("Enter availability (1: available, 0: not available): ");
    scanf("%d", &meal.available);

    meal_list[meal_count++] = meal;
    save_meals();
    printf("Meal added.\n");
}

void update_meal() {
    char meal_name[MAX_NAME_LENGTH];
    int meal_index = -1;
    printf("Enter meal name to update: ");
    scanf("%s", meal_name);

    for (int i = 0; i < meal_count; i++) {
        if (strcmp(meal_list[i].name, meal_name) == 0) {
            meal_index = i;
            break;
        }
    }

    if (meal_index == -1) {
        printf("Meal not found.\n");
        return;
    }

    printf("Enter new meal price: ");
    scanf("%f", &meal_list[meal_index].price);
    printf("Enter new preparation time: ");
    scanf("%d", &meal_list[meal_index].prep_time);
    printf("Enter new availability (1: available, 0: not available): ");
    scanf("%d", &meal_list[meal_index].available);

    save_meals();
    printf("Meal updated.\n");
}

void delete_meal() {
    char meal_name[MAX_NAME_LENGTH];
    int meal_index = -1;
    printf("Enter meal name to delete: ");
    scanf("%s", meal_name);

    for (int i = 0; i < meal_count; i++) {
        if (strcmp(meal_list[i].name, meal_name) == 0) {
            meal_index = i;
            break;
        }
    }

    if (meal_index == -1) {
        printf("Meal not found.\n");
        return;
    }

    for (int i = meal_index; i < meal_count - 1; i++) {
        meal_list[i] = meal_list[i + 1];
    }
    meal_count--;

    save_meals();
    printf("Meal deleted.\n");
}

void approve_order() {
    char order_id[MAX_NAME_LENGTH];
    printf("Enter order ID to approve: ");
    scanf("%s", order_id);

    for (int i = 0; i < order_count; i++) {
        if (strcmp(order_list[i].id, order_id) == 0) {
            order_list[i].status = 1;
            save_orders();
            printf("Order approved.\n");
            return;
        }
    }

    printf("Order not found.\n");
}

void reject_order() {
    char order_id[MAX_NAME_LENGTH];
    printf("Enter order ID to reject: ");
    scanf("%s", order_id);

    for (int i = 0; i < order_count; i++) {
        if (strcmp(order_list[i].id, order_id) == 0) {
            order_list[i].status = -1;
            save_orders();
            printf("Order rejected.\n");
            return;
        }
    }

    printf("Order not found.\n");
}

void dailyOrders(){
    FILE *fp = fopen("siparisler.txt", "r");
    if (!fp) {
        printf("Error opening file.\n");
        return;
    }
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    char day[25];
    strftime(day, sizeof(day)-1, "%d-%m-%Y", tm);
    char filename[20];
    snprintf(filename, sizeof(filename), "%s.txt", day);
    FILE *fdaily = fopen(filename, "w");
    if (!fdaily) {
        printf("Error opening file.\n");
        return;
    }
    int i=0;
    Order orders[100];
    while(fscanf(fp, "%s %s %f %s %s %s %d", orders[i].id, orders[i].meal_name, orders[i].price, orders[i].order_time, orders[i].prep_time, orders[i].user, orders[i].status) != EOF){
        if (strncmp(orders[i].order_time, day, 10) == 0){
            fprintf(fdaily, "%s %s %.2f %s %s %s %d", orders[i].id, orders[i].meal_name, orders[i].price, orders[i].order_time, orders[i].prep_time, orders[i].user, orders[i].status);
        }
        i++;
    }

}

void view_daily_report() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char today[MAX_NAME_LENGTH];
    strftime(today, sizeof(today) - 1, "%d-%m-%Y", t);

    printf("Daily Report for %s:\n", today);
    for (int i = 0; i < order_count; i++) {
        if (strncmp(order_list[i].order_time, today, 10) == 0) {
            printf("%s %s %.2f TL %s %s %d\n",
                   order_list[i].id, order_list[i].meal_name, order_list[i].price, order_list[i].order_time,
                   order_list[i].prep_time, order_list[i].status);
        }
    }
    dailyOrders();
}





void restaurant_application() {
    int choice;
    do {
        printf("\n1. Add Meal\n2. Update Meal\n3. Delete Meal\n4. Approve Order\n5. Reject Order\n6. View Daily Report\n7. Exit\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                add_meal();
                break;
            case 2:
                update_meal();
                break;
            case 3:
                delete_meal();
                break;
            case 4:
                approve_order();
                break;
            case 5:
                reject_order();
                break;
            case 6:
                view_daily_report();
                break;
            case 7:
                printf("Exiting restaurant application.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 7);
}

void view_pending_orders() {
    printf("Pending Orders:\n");
    for (int i = 0; i < order_count; i++) {
        if (order_list[i].status == 0) {
            printf("Order ID: %s, Meal: %s, Price: %.2f TL, Order Time: %s, Preparation Time: %s, User: %s\n",
                   order_list[i].id, order_list[i].meal_name, order_list[i].price, order_list[i].order_time,
                   order_list[i].prep_time, order_list[i].user);
        }
    }
}

void kitchen_application() {
    int choice;
    do {
        printf("\n1. View Pending Orders\n2. Exit\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                view_pending_orders();
                break;
            case 2:
                printf("Exiting kitchen application.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 2);
}

int main() {
    load_meals();
    load_orders();

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
