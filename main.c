#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BearLibTerminal.h"


typedef struct {
    int id;
    char product[50];
    char category[30];
    int quantity;
    float price;
} Sales;

Sales* salesData = NULL;
int salesCount = 0;
char** categories = NULL;
int categoryCount = 0;

void displayRecords(Sales* data, int count) {
    terminal_clear();
    terminal_print(1, 1, "Sales Records:");
    int y = 3;
    for (int i = 0; i < count && y < 22; i++) {
        terminal_printf(1, y++, "ID: %d, Product: %s, Category: %s, Qty: %d, Price: %.2f",
                        data[i].id, data[i].product, data[i].category, data[i].quantity, data[i].price);
    }
    terminal_print(1, 23, "Press any key to return...");
    terminal_refresh();
    terminal_read();
}

float calculateTotalRevenue(Sales* data, int count) {
    float total = 0;
    for (int i = 0; i < count; i++) {
        total += data[i].quantity * data[i].price;
    }
    return total;
}

Sales findHighestSale(Sales* data, int count) {
    Sales highest = data[0];
    float maxRev = data[0].quantity * data[0].price;
    for (int i = 1; i < count; i++) {
        float rev = data[i].quantity * data[i].price;
        if (rev > maxRev) {
            maxRev = rev;
            highest = data[i];
        }
    }
    return highest;
}

float calculateAverageRevenue(Sales* data, int count) {
    return calculateTotalRevenue(data, count) / count;
}

void sortByRevenue(Sales* data, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            float rev1 = data[j].quantity * data[j].price;
            float rev2 = data[j+1].quantity * data[j+1].price;
            if (rev1 < rev2) {
                Sales temp = data[j];
                data[j] = data[j+1];
                data[j+1] = temp;
            }
        }
    }
}

void sortByCategoryAlpha(Sales* data, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (strcmp(data[j].category, data[j+1].category) > 0) {
                Sales temp = data[j];
                data[j] = data[j+1];
                data[j+1] = temp;
            }
        }
    }
}

void sortById(Sales* data, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (data[j].id > data[j+1].id) {
                Sales temp = data[j];
                data[j] = data[j+1];
                data[j+1] = temp;
            }
        }
    }
}

void filterByCategory(Sales* data, int count, char* category) {
    terminal_clear();
    terminal_print(1, 1, "Filtered Records:");
    int y = 3;
    for (int i = 0; i < count && y < 22; i++) {
        if (strcmp(data[i].category, category) == 0) {
            terminal_printf(1, y++, "ID: %d, Product: %s, Qty: %d, Price: %.2f",
                            data[i].id, data[i].product, data[i].quantity, data[i].price);
        }
    }
    terminal_print(1, 23, "Press any key to return...");
    terminal_refresh();
    terminal_read();
}

// csv reading
int loadCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return 0;
    }

    // bom file ko lagi
    unsigned char bom[3];
    if (fread(bom, 1, 3, file) == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
        // BOM present, continue
    } else {
        // No BOM, rewind to start
        rewind(file);
    }

    char line[256];
    salesCount = 0;
    // Count lines
    while (fgets(line, sizeof(line), file)) salesCount++;
    rewind(file);

    // Skip BOM again if present
    if (fread(bom, 1, 3, file) == 3 && bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF) {
        // BOM present
    } else {
        rewind(file);
    }

    salesData = (Sales*)malloc(sizeof(Sales) * salesCount);
    int i = 0;

    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ",");
        if (token) salesData[i].id = atoi(token);

        token = strtok(NULL, ",");
        if (token) strcpy(salesData[i].product, token);

        token = strtok(NULL, ",");
        if (token) strcpy(salesData[i].category, token);

        token = strtok(NULL, ",");
        if (token) salesData[i].quantity = atoi(token);

        token = strtok(NULL, ",");
        if (token) salesData[i].price = atof(token);

        i++;
    }

    fclose(file);
    return 1;
}

// Collect unique categories
void collectCategories() {
    categoryCount = 0;
    categories = (char**)malloc(sizeof(char*) * salesCount);
    for (int i = 0; i < salesCount; i++) {
        int found = 0;
        for (int j = 0; j < categoryCount; j++) {
            if (strcmp(salesData[i].category, categories[j]) == 0) {
                found = 1;
                break;
            }
        }
        if (!found) {
            categories[categoryCount] = (char*)malloc(strlen(salesData[i].category) + 1);
            strcpy(categories[categoryCount], salesData[i].category);
            categoryCount++;
        }
    }
}

// menu
const char* menuItems[] = {
    "View Records",
    "Calculate Total Revenue",
    "Find Highest Sale",
    "Calculate Average Revenue",
    "Sort by Revenue/Category/ID",
    "Filter by Category",
    "Exit"
};
int menuSize = sizeof(menuItems)/sizeof(menuItems[0]);

// box
void drawBox(int x, int y, int w, int h, int color) {
    terminal_color(color);
    terminal_put(x, y, '+');
    terminal_put(x + w - 1, y, '+');
    terminal_put(x, y + h - 1, '+');
    terminal_put(x + w - 1, y + h - 1, '+');

    for (int i = 1; i < w - 1; i++) {
        terminal_put(x + i, y, '-');
        terminal_put(x + i, y + h - 1, '-');
    }
    for (int i = 1; i < h - 1; i++) {
        terminal_put(x, y + i, '|');
        terminal_put(x + w - 1, y + i, '|');
    }
}

// Prompt the user to type a string (supports backspace, enter, escape).
// Returns 1 if the user confirmed (Enter), 0 if cancelled (Esc).
int promptString(const char* prompt, char* out, int maxLen) {
    int pos = 0;
    out[0] = '\0';

    while (1) {
        terminal_clear();
        terminal_color(color_from_name("yellow"));
        terminal_printf(2, 2, "%s", prompt);

        terminal_color(color_from_name("white"));
        terminal_printf(2, 4, "> %s", out);

        terminal_color(color_from_name("gray"));
        terminal_printf(2, 6, "ENTER=OK  BACKSPACE=DEL  ESC=Cancel");
        terminal_refresh();

        int key = terminal_read();
        if (key == TK_ENTER) {
            return 1;
        }
        if (key == TK_ESCAPE || key == TK_CLOSE) {
            return 0;
        }
        if (key == TK_BACKSPACE) {
            if (pos > 0) {
                pos--;
                out[pos] = '\0';
            }
            continue;
        }

        // Accept printable ASCII (space..~)
        if (key >= 32 && key < 127 && pos < maxLen - 1) {
            out[pos++] = (char)key;
            out[pos] = '\0';
        }
    }
}

int promptSelection(const char* title, char** items, int count) {
    if (count <= 0) return -1;

    int selected = 0;
    int top = 0;
    const int visible = 14; // visible rows for items
    const int x = 2;
    const int y = 3;

    while (1) {
        terminal_clear();
        terminal_color(color_from_name("yellow"));
        terminal_printf(x, 1, "%s", title);

        drawBox(x - 1, y - 1, 60, visible + 2, color_from_name("gray"));

        for (int i = 0; i < visible && (top + i) < count; i++) {
            int idx = top + i;
            if (idx == selected) {
                terminal_color(color_from_name("black"));
                terminal_bkcolor(color_from_name("light_green"));
            } else {
                terminal_color(color_from_name("white"));
                terminal_bkcolor(color_from_name("black"));
            }
            terminal_printf(x, y + i, "%2d) %s", idx + 1, items[idx]);
        }

        terminal_bkcolor(color_from_name("black"));
        terminal_color(color_from_name("gray"));
        terminal_printf(x, y + visible + 1, "↑/↓ to move, ENTER to select, ESC to cancel");
        terminal_refresh();

        int key = terminal_read();
        if (key == TK_CLOSE || key == TK_ESCAPE) return -1;
        if (key == TK_UP) {
            if (selected > 0) {
                selected--;
                if (selected < top) top = selected;
            }
        } else if (key == TK_DOWN) {
            if (selected < count - 1) {
                selected++;
                if (selected >= top + visible) top = selected - visible + 1;
            }
        } else if (key == TK_ENTER) {
            return selected;
        } else if (key >= TK_1 && key <= TK_9) {
            int num = key - TK_1;
            if (num < count) return num;
        }
    }
}

void drawMenu(int selectedIndex) {
    terminal_clear();
    terminal_bkcolor(color_from_name("black"));
    terminal_color(color_from_name("white"));
    terminal_clear();

    // Header
    terminal_color(color_from_name("yellow"));
    terminal_printf(2, 1, "SALES ANALYTICS DASHBOARD");
    terminal_color(color_from_name("cyan"));
    terminal_printf(2, 2, "=======================");

    // Border around menu
    drawBox(1, 3, 38, menuSize + 4, color_from_name("gray"));

    // Menu items
    for (int i = 0; i < menuSize; i++) {
        if (i == selectedIndex) {
            terminal_color(color_from_name("black"));
            terminal_bkcolor(color_from_name("light_green"));
        } else {
            terminal_color(i % 2 == 0 ? color_from_name("light_green") : color_from_name("light_cyan"));
            terminal_bkcolor(color_from_name("black"));
        }
        terminal_printf(3, 4 + i, "%d) %s", i + 1, menuItems[i]);
    }

    terminal_bkcolor(color_from_name("black"));
    terminal_color(color_from_name("gray"));
    terminal_printf(2, 20, "Use ↑/↓ then ENTER, or press 1-7 to select.");
    terminal_refresh();
}

void showMessage(const char* msg) {
    terminal_clear();
    terminal_color(color_from_name("light_yellow"));
    terminal_printf(1, 1, "%s", msg);
    terminal_color(color_from_name("gray"));
    terminal_printf(1, 3, "Press any key to return to menu.");
    terminal_refresh();
    terminal_read();
}

int main() {
    terminal_open();
    terminal_set("window: size=80x25, title='Sales Dashboard'; font: default");

    // hardcoded csv
    char filename[100] = "sales.csv";

    terminal_clear();
    terminal_printf(1, 1, "Loading %s...", filename);
    terminal_refresh();

    if (!loadCSV(filename)) {
        terminal_clear();
        char msg[200];
        sprintf(msg, "Failed to load CSV file: %s. Press any key to exit.", filename);
        terminal_print(1, 1, msg);
        terminal_refresh();
        terminal_read();
        terminal_close();
        return 1;
    }

    collectCategories();

    int selectedMenu = 0;
    while (1) {
        drawMenu(selectedMenu);
        int key = terminal_read();

        if (key == TK_CLOSE) break;

        if (key == TK_UP) {
            if (selectedMenu > 0) selectedMenu--;
            continue;
        }
        if (key == TK_DOWN) {
            if (selectedMenu < menuSize - 1) selectedMenu++;
            continue;
        }

        int choice = -1;
        if (key == TK_ENTER) {
            choice = selectedMenu;
        } else if (key >= TK_1 && key <= TK_9) {
            int num = key - TK_1;
            if (num < menuSize) choice = num;
        }

        if (choice < 0) continue;

        switch (choice) {
            case 0:
                displayRecords(salesData, salesCount);
                break;
            case 1: {
                float total = calculateTotalRevenue(salesData, salesCount);
                char msg[100];
                sprintf(msg, "Total Revenue: %.2f", total);
                showMessage(msg);
                break;
            }
            case 2: {
                Sales s = findHighestSale(salesData, salesCount);
                char msg[100];
                sprintf(msg, "Highest Sale: ID %d, %s, %.2f", s.id, s.product, s.price);
                showMessage(msg);
                break;
            }
            case 3: {
                float avg = calculateAverageRevenue(salesData, salesCount);
                char msg[100];
                sprintf(msg, "Average Revenue: %.2f", avg);
                showMessage(msg);
                break;
            }
            case 4: {
                terminal_clear();
                terminal_print(1, 1, "Sort by:");
                terminal_print(1, 3, "1. Revenue (high->low)");
                terminal_print(1, 4, "2. Category (A->Z)");
                terminal_print(1, 5, "3. ID (ascending)");
                terminal_print(1, 7, "Press 1-3 to choose:");
                terminal_refresh();
                int skey = terminal_read();
                if (skey == TK_1) {
                    sortByRevenue(salesData, salesCount);
                } else if (skey == TK_2) {
                    sortByCategoryAlpha(salesData, salesCount);
                } else if (skey == TK_3) {
                    sortById(salesData, salesCount);
                }
                displayRecords(salesData, salesCount);
                break;
            }
            case 5: {
                if (categoryCount == 0) {
                    showMessage("No categories found in CSV.");
                    break;
                }

                int selected = promptSelection("Select a category to filter:", categories, categoryCount);
                if (selected >= 0) {
                    filterByCategory(salesData, salesCount, categories[selected]);
                    showMessage("Filtered by Category!");
                }
                break;
            }
            case 6:
                terminal_close();
                free(salesData);
                for (int i = 0; i < categoryCount; i++) {
                    free(categories[i]);
                }
                free(categories);
                return 0;
        }
    }

    terminal_close();
    free(salesData);
    for (int i = 0; i < categoryCount; i++) {
        free(categories[i]);
    }
    free(categories);
    return 0;
}