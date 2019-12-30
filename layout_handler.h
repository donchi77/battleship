#ifndef LAYOUT_H
    #define LAYOUT_H

void clear_console() {
    system("clear");
}

void wait_player() {
    printf("\n\tPremere [ENTER] per continuare... \n");
    while (getchar() != '\n');
    getchar();

    clear_console();
}

void newline(int DIM_TABLE) {
    printf("\n");
    for (int k = 0; k < (DIM_TABLE * 4 + 1); k++)
        printf("-");
    printf("\n");
}

#endif