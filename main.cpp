#include "mbed.h"
//http://os.mbed.com/users/simon/code/TextLCD/
#include "TextLCD.h"
//http://os.mbed.com/users/rlanghbv/code/KeypadLib/
#include "keypad.h"
//https://os.mbed.com/users/quevedo/code/ESP8266/
#include "ESP8266.h"
#include "cctype"
//http://os.mbed.com/users/mbed_official/code/mbed-rtos/
#include "rtos.h"

TextLCD lcd(PC_0, PC_1, PC_2, PC_3, PC_4, PC_5);
//PC_0 RS
//PC_1 E
//PC_2 D4
//PC_3 D5
//PC_4 D6
//PC_5 D7
          // c0    c1     c2    c3    r0    r1     r2    r3    
Keypad kpad( PB_3, PA_10, PA_2, PA_3, PA_8, PB_10, PB_4, PB_5 );

Serial pc(USBTX, USBRX);

InterruptIn reqBtn(PA_0);
DigitalOut buzzer(PC_9);
DigitalOut redLight(PC_10);
DigitalOut greenLight(PC_12);
int c = 0;
Thread thread1, thread2;
int askForPassKey = 0;
const int passkey = 147;
int checkPasskey(int keyvalue) {
    if (keyvalue == passkey) {
        return 1;
    } else {
        return 0;
    }
}

void buzzsound() //buzzer sound when entered wrong passkey
{
    buzzer = 1;
    wait(0.5);
    buzzer = 0;
}

void opensound() //buzzer sound when gate is opened
{
    buzzer = 1;
    wait(0.1);
    buzzer = 0;
    wait(0.1);
    buzzer = 1;
    wait(0.1);
    buzzer = 0;
}   

void KeyPad() {
    char key;
    int num = 0;
    lcd.locate(0, 0);
    lcd.printf("%*s", 16, "");
    //lcd.locate(0, 0);
    //lcd.printf("Password:%d\n\r",num);
    lcd.locate(0, 1);
    lcd.printf("%*s", 16, "");
    while(1) {
        if (askForPassKey == 1) {
            lcd.locate(0, 0);
            lcd.printf("Password:%d",num);
            key = kpad.getKey();    
            if(key != KEY_RELEASED)
            {
                if (isdigit(key)) {
                    num = num < 0 ? 0 : num * 10;
                    num += key - '0';
                    if (num <= 1000) { // increase the value for digit, 1000 = 3 digits, 100 = 2 digits
                        lcd.locate(0, 0);
                        lcd.printf("%*s", 16, "");
                        //lcd.locate(0, 1);
                        //lcd.printf("%d", num);
                    } else {
                        num -= key - '0';
                        num /= 10;
                    }
                } else if (key == 'D') {
                    num = num < 0 ? 0: num / 10;
                    lcd.locate(0, 0);
                    lcd.printf("%*s", 16, "");
                    //lcd.locate(0, 1);
                    //lcd.printf("%d", num);
                } else if (key == '*') {
                    lcd.locate(0, 0);
                    lcd.printf("%*s", 16, "");
                    //lcd.locate(0, 1);
                    //lcd.printf("%*s", 16, "");
                    // Check password
                    if (checkPasskey(num) == 1) {
                        askForPassKey = 0;
                    } else {
                        lcd.locate(0, 0);
                        lcd.printf("%*s", 16, "");
                        lcd.locate(0, 0);
                        lcd.printf("Passkey FAIL!");
                        buzzsound();
                        c=c+1;
                        if (c>2) {
                            buzzer = 1;
                            }
                        wait(2);
                        lcd.locate(0, 0);
                        lcd.printf("%*s", 16, "");
                        lcd.locate(0, 0);
                        lcd.printf("Try Again!");
                        wait(2);
                        lcd.locate(0, 0);
                        lcd.printf("%*s", 16, "");
                            
                    }
                    num = 0;
                    //return num;
                }

                while (key != KEY_RELEASED) {
                    key = kpad.getKey();
                }
            }
        }
    }
}

void GateEntrance() {
    while(1) {
        if (reqBtn == 1) {
            while(reqBtn == 1);
            lcd.locate(0, 0);
            lcd.printf("%*s", 16, "");
            askForPassKey = 1;
            wait(1);
            while(askForPassKey==1){
                redLight = 1;
            }
            lcd.locate(0, 0);
            lcd.printf("%*s", 16, "");
            lcd.locate(0, 0);
            lcd.printf("Gate Opened!");
            opensound();
            redLight = 0;
            greenLight = 1;
            wait(2);
            lcd.locate(0, 0);
            lcd.printf("%*s", 16, "");
            lcd.locate(0, 1);
            lcd.printf("%*s", 16, "");
            lcd.locate(0, 0);
        } else {
            redLight = 1;
            greenLight = 0;
            lcd.locate(0, 0);
            lcd.printf("Gate Closed!");
            lcd.locate(0, 1);
            lcd.printf("%*s", 16, "");
        }
    }
}

int main() {
    // Initialization 
    kpad.enablePullUp();
    lcd.locate(0, 0);
    lcd.printf("Please press start...");
    while(reqBtn == 0);
    while(reqBtn == 1);
    lcd.locate(0, 0);
    lcd.printf("%*s", 16, "");
    lcd.locate(0, 1);
    lcd.printf("%*s", 16, "");
    lcd.locate(0, 0);
    lcd.printf("Starting Up...");
    // start wifi here...
    askForPassKey = 0;
    redLight = 1;
    greenLight = 0;
    wait(1.0);
    lcd.locate(0, 0);
    lcd.printf("%*s", 16, "");
    lcd.locate(0, 1);
    lcd.printf("%*s", 16, "");
    // Start the threads 
    thread1.start(KeyPad);
    thread2.start(GateEntrance);
    
    while (1) {}
}
