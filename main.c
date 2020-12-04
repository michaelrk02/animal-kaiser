#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SPECIES_HUMAN       3
#define NUM_SPECIES_COMPUTER    10
#define NUM_PACKAGES            4

#ifndef __cplusplus
typedef int bool;
#define true 1
#define false 0
#endif

typedef enum {
    pkgkind_bandage,
    pkgkind_booster,
} pkgkind_t;

typedef struct {
    pkgkind_t kind;
    const char *name;
    int cost;
    union {
        int bandage_health;
        int booster_strength;
    } item;
} package_t;

typedef struct {
    const char *name;
    int health;
    int strength;
} species_t;

typedef struct {
    const species_t *spec;
    int health;
    int strength;
    int credits;
} human_t;

typedef struct {
    const species_t *spec;
    int health;
} computer_t;

typedef enum {
    atkkind_rock,
    atkkind_paper,
    atkkind_scissors,
} atkkind_t;

const species_t g_human_species[NUM_SPECIES_HUMAN] = {
    {"Electric Ray", 75, 10},
    {"Cheetah", 100, 7},
    {"Hippo Barry", 150, 5},
};

const species_t g_computer_species[NUM_SPECIES_COMPUTER] = {
    {"African Wild Dog", 30, 5},
    {"Vampire Bat", 40, 7},
    {"Andean Condor", 50, 9},
    {"Snow Leopard", 60, 10},
    {"Archaeopteryx", 70, 13},
    {"Manta Ray", 80, 15},
    {"Smalltooth Sawfish", 90, 18},
    {"Anaconda", 100, 20},
    {"Mammoth", 125, 22},
    {"Narwhal", 150, 25},
};

const char *g_atkkind_lookup[] = {"Rock", "Paper", "Scissors"};

package_t g_packages[NUM_PACKAGES];

unsigned int g_current_level = 1;

void init_human(human_t *, const species_t *);
void init_computer(computer_t *, const species_t *);
void init_packages(void);
void attack(human_t *, computer_t *);
void insight(const human_t *, const computer_t *);
void go_to_store(human_t *);
bool wins(atkkind_t, atkkind_t);
int random_range(int, int);
int randomize(int, int);
bool check_gameover(const human_t *);
bool check_level_up(const computer_t *);

int main() {
    human_t human;
    computer_t computer;
    int i, spec_id;

    srand(time(NULL));

    puts("!! WELCOME TO ANIMAL KAISER !!");
    puts("Choose your fighter:");
    for (i = 0; i < NUM_SPECIES_HUMAN; i++) {
        printf(" [%d] %s (H: %d, S: %d)\n", i + 1, g_human_species[i].name, g_human_species[i].health, g_human_species[i].strength);
    }
    printf("Your choice? ");
    scanf("%d", &spec_id);

    init_human(&human, &g_human_species[spec_id - 1]);
    init_computer(&computer, &g_computer_species[0]);
    init_packages();

    while (true) {
        int action;

        if (check_gameover(&human)) {
            break;
        }
        if (check_level_up(&computer)) {
            int prize = (g_current_level - 1) * 250;
            if (g_current_level > NUM_SPECIES_COMPUTER) {
                puts("\n>>> !! CONGRATULATIONS !!");
                puts(">>> You have passed through all levels");
                puts(">>> Have a good day ^_^");
                break;
            }
            human.credits += prize;
            printf(">>> Added %d credits to your bank account\n", prize);

            init_computer(&computer, &g_computer_species[g_current_level - 1]);
            printf(">>> A new opposite has spawned: %s\n", computer.spec->name);
        }

        printf("\n=== LEVEL %d: %s vs. %s ===\n", g_current_level, human.spec->name, computer.spec->name);
        puts("Actions:");
        puts(" [1] Attack");
        puts(" [2] Insight");
        puts(" [3] Go to store");
        puts(" [other] Exit");
        printf("Your choice? ");
        scanf("%d", &action);

        if (action == 1) {
            attack(&human, &computer);
        } else if (action == 2) {
            insight(&human, &computer);
        } else if (action == 3) {
            go_to_store(&human);
        } else {
            break;
        }
    }
    return 0;
}

void init_human(human_t *human, const species_t *spec) {
    human->spec = spec;
    human->health = spec->health;
    human->strength = spec->strength;
    human->credits = 0;
}

void init_computer(computer_t *computer, const species_t *spec) {
    computer->spec = spec;
    computer->health = spec->health;
}

void init_packages(void) {
    int n = 0;

    g_packages[n].kind = pkgkind_bandage;
    g_packages[n].name = "Fat";
    g_packages[n].cost = 150;
    g_packages[n].item.bandage_health = 25;
    n++;

    g_packages[n].kind = pkgkind_bandage;
    g_packages[n].name = "Mineral";
    g_packages[n].cost = 250;
    g_packages[n].item.bandage_health = 50;
    n++;

    g_packages[n].kind = pkgkind_booster;
    g_packages[n].name = "Carbohydrate";
    g_packages[n].cost = 500;
    g_packages[n].item.booster_strength = 1;
    n++;

    g_packages[n].kind = pkgkind_booster;
    g_packages[n].name = "Protein";
    g_packages[n].cost = 800;
    g_packages[n].item.booster_strength = 2;
    n++;
}

void attack(human_t *human, computer_t *computer) {
    int human_mode;

    printf("\nAttacking %s ...\n", computer->spec->name);
    puts("Select attack mode:");
    puts(" [1] Rock");
    puts(" [2] Paper");
    puts(" [3] Scissors");
    printf("Your choice? ");
    scanf("%d", &human_mode);

    if ((1 <= human_mode) && (human_mode <= 3)) {
        int damage = 0;
        atkkind_t human_atk, computer_atk;

        switch (human_mode) {
        case 1:
            human_atk = atkkind_rock;
            break;
        case 2:
            human_atk = atkkind_paper;
            break;
        case 3:
            human_atk = atkkind_scissors;
            break;
        }

        computer_atk = (atkkind_t)random_range(atkkind_rock, atkkind_scissors);
        printf("Human chooses: %s\n", g_atkkind_lookup[human_atk]);
        printf("Computer chooses: %s\n", g_atkkind_lookup[computer_atk]);
        if (wins(human_atk, computer_atk)) {
            damage = randomize(human->strength, human->strength / 5);
            computer->health = computer->health - damage;
            printf("\n>>> %s dealt %d damage to %s\n", human->spec->name, damage, computer->spec->name);
        } else if (wins(computer_atk, human_atk)) {
            damage = randomize(computer->spec->strength, computer->spec->strength / 5);
            human->health = human->health - damage;
            printf("\n>>> %s dealt %d damage to %s\n", computer->spec->name, damage, human->spec->name);
        } else {
            puts("\n>>> No one attacked");
        }

        return;
    }
    puts("Invalid input");
    return attack(human, computer);
}

void insight(const human_t *human, const computer_t *computer) {
    puts("\n=== INSIGHT ===");

    puts("Human:");
    printf(" %s health: %d/%d\n", human->spec->name, human->health, human->spec->health);
    printf(" %s strength: %d\n", human->spec->name, human->strength);

    puts("Computer:");
    printf(" %s health: %d/%d\n", computer->spec->name, computer->health, computer->spec->health);
    printf(" %s strength: %d\n", computer->spec->name, computer->spec->strength);
}

void go_to_store(human_t *human) {
    int i, choice;

    puts("\n=== WELCOME TO THE BIO-STORE ===");
    printf("Bank account: %d credits\n", human->credits);
    puts("What do you want to buy?");
    for (i = 0; i < NUM_PACKAGES; i++) {
        printf(" [%d] %s ", (i + 1), g_packages[i].name);
        switch (g_packages[i].kind) {
        case pkgkind_bandage:
            printf("(+%d health) ", g_packages[i].item.bandage_health);
            break;
        case pkgkind_booster:
            printf("(+%d strength) ", g_packages[i].item.booster_strength);
            break;
        }
        printf("-> $%d\n", g_packages[i].cost);
    }
    puts(" [other] Go back");
    printf("Your choice? ");
    scanf("%d", &choice);

    if ((1 <= choice) && (choice <= NUM_PACKAGES)) {
        const package_t *pkg = &g_packages[choice - 1];

        if (human->credits >= pkg->cost) {
            switch (pkg->kind) {
            case pkgkind_bandage:
                human->health = human->health + pkg->item.bandage_health;
                human->health = human->health > human->spec->health ? human->spec->health : human->health;
                break;
            case pkgkind_booster:
                human->strength = human->strength + pkg->item.booster_strength;
                break;
            }
            human->credits = human->credits - pkg->cost;
            printf("\n>>> Successfully bought %s for $%d\n", pkg->name, pkg->cost);
        } else {
            puts("\n>>> Not enough credits :(");
        }
    } else {
        puts("\n>>> Come back next time ;)");
        return;
    }
    go_to_store(human);
}

bool wins(atkkind_t invoker, atkkind_t receiver) {
    if (invoker == atkkind_rock) {
        if (receiver == atkkind_scissors) {
            return true;
        }
    }
    if (invoker == atkkind_paper) {
        if (receiver == atkkind_rock) {
            return true;
        }
    }
    if (invoker == atkkind_scissors) {
        if (receiver == atkkind_paper) {
            return true;
        }
    }
    return false;
}

int random_range(int low, int high) {
    return low + rand() % (high - low + 1);
}

int randomize(int n, int delta) {
    int low = n - delta;
    int high = n + delta;

    low = low < 0 ? 0 : low;

    return random_range(low, high);
}

bool check_gameover(const human_t *human) {
    if (human->health <= 0) {
        puts("\n>>> !! GAME OVER !!");
        printf(">>> Current level: %d\n", g_current_level);
        puts(">>> Better luck next time ;)");
        return true;
    }
    return false;
}

bool check_level_up(const computer_t *computer) {
    if (computer->health <= 0) {
        puts("\n>>> !! LEVEL UP !!");
        g_current_level++;
        return true;
    }
    return false;
}

/*
    Copyright (C) 2020, Michael R. Krisnadhi (M0520047)

    Program Studi S-1 Informatika
    Fakultas Matematika dan Ilmu Pengetahuan Alam
    Universitas Sebelas Maret
*/

