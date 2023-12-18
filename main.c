/**
 * Š - ARM-FITkit3: Game SNAKE
 * @authors
 *  xkalut00, Maksim Kalutski
 *
 * @file   main.c
 * @brief  Implementation of the game SNAKE for ARM-FITkit3.
 * @date   10.12.2023
 */

#include "MK60D10.h"

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK 0x1Fu
#define GPIO_PIN(x) (((1) << ((x) & GPIO_PIN_MASK)))

/* Constants specifying delay loop duration */
#define  tdelay1      10000
#define tdelay2     20

#define UP    0
#define RIGHT 1
#define DOWN  2
#define LEFT  3

#define SNAKE_LENGTH 4

/* Snake with dynamic length */
int snake[SNAKE_LENGTH][2];
int direction;

void init_snake() {
    /* Snake with size 4 */
    for (int i = 0; i < SNAKE_LENGTH; i++) {
        snake[i][0] = 4;
        snake[i][1] = 8 + i;
    }

    direction = UP;
}

/* Configuration of the necessary MCU peripherals */
void SystemConfig() {
    /* Turn on all port clocks */
    SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;

    /* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
    PORTA->PCR[8] = (0 | PORT_PCR_MUX(0x01));  // A0
    PORTA->PCR[10] = (0 | PORT_PCR_MUX(0x01));  // A1
    PORTA->PCR[6] = (0 | PORT_PCR_MUX(0x01));  // A2
    PORTA->PCR[11] = (0 | PORT_PCR_MUX(0x01));  // A3

    /* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
    PORTA->PCR[26] = (0 | PORT_PCR_MUX(0x01));  // R0
    PORTA->PCR[24] = (0 | PORT_PCR_MUX(0x01));  // R1
    PORTA->PCR[9] = (0 | PORT_PCR_MUX(0x01));  // R2
    PORTA->PCR[25] = (0 | PORT_PCR_MUX(0x01));  // R3
    PORTA->PCR[28] = (0 | PORT_PCR_MUX(0x01));  // R4
    PORTA->PCR[7] = (0 | PORT_PCR_MUX(0x01));  // R5
    PORTA->PCR[27] = (0 | PORT_PCR_MUX(0x01));  // R6
    PORTA->PCR[29] = (0 | PORT_PCR_MUX(0x01));  // R7

    /* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
    PORTE->PCR[28] = (0 | PORT_PCR_MUX(0x01)); // #EN

    /* Change corresponding PTA port pins as outputs */
    PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

    /* Change corresponding PTE port pins as outputs */
    PTE->PDDR = GPIO_PDDR_PDD(GPIO_PIN(28));

    /* Set corresponding PTE pins for GPIO functionality */
    NVIC_EnableIRQ(PORTE_IRQn);
    NVIC_SetPriority(PORTE_IRQn, 0);
    PORTE->PCR[10] = (PORT_PCR_ISF(0x01)    // right
                      | PORT_PCR_IRQC(0x0A)
                      | PORT_PCR_MUX(0x01)
                      | PORT_PCR_PE(0x01)
                      | PORT_PCR_PS(0x01)
    );
    PORTE->PCR[11] = (PORT_PCR_ISF(0x01)    // reset
                      | PORT_PCR_IRQC(0x0A)
                      | PORT_PCR_MUX(0x01)
                      | PORT_PCR_PE(0x01)
                      | PORT_PCR_PS(0x01)
    );
    PORTE->PCR[12] = (PORT_PCR_ISF(0x01)    // down
                      | PORT_PCR_IRQC(0x0A)
                      | PORT_PCR_MUX(0x01)
                      | PORT_PCR_PE(0x01)
                      | PORT_PCR_PS(0x01)
    );
    PORTE->PCR[26] = (PORT_PCR_ISF(0x01)    // up
                      | PORT_PCR_IRQC(0x0A)
                      | PORT_PCR_MUX(0x01)
                      | PORT_PCR_PE(0x01)
                      | PORT_PCR_PS(0x01)
    );
    PORTE->PCR[27] = (PORT_PCR_ISF(0x01)    // left
                      | PORT_PCR_IRQC(0x0A)
                      | PORT_PCR_MUX(0x01)
                      | PORT_PCR_PE(0x01)
                      | PORT_PCR_PS(0x01)
    );

    NVIC_ClearPendingIRQ(PORTE_IRQn);
    NVIC_EnableIRQ(PORTE_IRQn);
}

/* Variable delay loop */
void delay(int t1, int t2) {
    int i, j;

    for (i = 0; i < t1; i++) {
        for (j = 0; j < t2; j++);
    }
}

/* Conversion of requested column number into the 4-to-16 decoder control.  */
void column_select(unsigned int col_num) {
    unsigned i, result, col_sel[4];

    for (i = 0; i < 4; i++) {
        result = col_num / 2;      // Whole-number division of the input number
        col_sel[i] = col_num % 2;
        col_num = result;

        switch (i) {
            // Selection signal A0
            case 0:
                ((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO(GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO(
                        GPIO_PIN(8)));
                break;

                // Selection signal A1
            case 1:
                ((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO(GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO(
                        GPIO_PIN(10)));
                break;

                // Selection signal A2
            case 2:
                ((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO(GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO(
                        GPIO_PIN(6)));
                break;

                // Selection signal A3
            case 3:
                ((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO(GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO(
                        GPIO_PIN(11)));
                break;

                // Otherwise nothing to do...
            default:
                break;
        }
    }
}

/* Select and display rows of the snake on the display */
void row_select() {
    for (int i = 3; i >= 0; i--) {
        PTA->PDOR &= ~GPIO_PDOR_PDO(0x3F000280);    // Clear row selection
        column_select(snake[i][1]); // Select the column based on snake position

        /* Set corresponding GPIO pins for row activation */
        switch (snake[i][0]) {
            case 0:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(29));
                break;
            case 1:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(27));
                break;
            case 2:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(7));
                break;
            case 3:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(28));
                break;
            case 4:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(25));
                break;
            case 5:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(9));
                break;
            case 6:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(24));
                break;
            case 7:
                PTA->PDOR |= GPIO_PDOR_PDO(GPIO_PIN(26));
                break;
        }
        delay(20, 100);
    }
}

/* If there is an interrupt (any button is pressed) */
void PORTE_IRQHandler(void) {
    /* Check which button was pressed */
    if (PORTE->ISFR & 0x4000000) {
        direction = UP;
        PORTE->ISFR |= 0x4000000;
    } else if (PORTE->ISFR & 0x1000) {
        direction = DOWN;
        PORTE->ISFR |= 0x1000;
    } else if (PORTE->ISFR & 0x8000000) {
        direction = LEFT;
        PORTE->ISFR |= 0x8000000;
    } else if (PORTE->ISFR & 0x400) {
        direction = RIGHT;
        PORTE->ISFR |= 0x400;
    } else if (PORTE->ISFR & 0x800) {
        init_snake();
        PORTE->ISFR |= 0x800;
    }
}

/* Move the snake based on the current direction */
void move_snake() {
    /* Move the body of the snake */
    for (int i = SNAKE_LENGTH - 1; i > 0; i--) {
        snake[i][0] = snake[i - 1][0];
        snake[i][1] = snake[i - 1][1];
    }

    /* Move the head of the snake */
    switch (direction) {
        case UP:
            snake[0][1] = (snake[0][1] - 1 + 16) % 16;
            break;
        case DOWN:
            snake[0][1] = (snake[0][1] + 1) % 16;
            break;
        case LEFT:
            snake[0][0] = (snake[0][0] - 1 + 8) % 8;
            break;
        case RIGHT:
            snake[0][0] = (snake[0][0] + 1) % 8;
            break;
    }
}

int main(void) {
    SystemConfig();
    init_snake();

    for (;;) {
        move_snake();

        /* Update the display */
        for (int i = 0; i < 100; i++) {
            row_select();
        }
    }

    //* Never leave main */
    return 0;
}
