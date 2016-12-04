#include "mbed.h"
#include "uLCD_4DGL.h"
#include "Speaker.h"
#include "DS1820.h"
#include "PinDetect.h"
#include <time.h>

//Program by: Tianhang Ren, Minsuk Chun, Richard Lee

////////////////////////////////////////////////////////
//Initialize
///////////////////////////////////////////////////////

DS1820 probe(p5);
PinDetect pbUp(p18);          // Buttons to raise/lower/enter temperature
PinDetect pbDown(p19);
PinDetect pbEnter (p20);
Serial pc(USBTX, USBRX);     // serial comms over usb back to console
//Timer T;
DigitalOut myled1(LED1);       // On when relay is engaged
DigitalOut myled2(LED2);
DigitalOut myled3(LED3);
DigitalOut myled4(LED4);
uLCD_4DGL uLCD(p28, p27, p29); // create a global uLCD object

DigitalOut ctrl(p8); //relay control

// setup instance of new Speaker class, mySpeaker using pin 21
// the pin must be a PWM output pin
Speaker mySpeaker(p26);

float userTemp = 0; //user desired temperature of device
int check = 0; //indicates whether enter key is pressed
float timeCook = 0;


////////////////////////////////////////////////////////
// Initialize pushbuttons
///////////////////////////////////////////////////////


// Callback routine is interrupt activated by a debounced pb1 hit
void pbUp_hit_callback (void)
{
    // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
    myled1 = !myled1;
    mySpeaker.PlayNote(200.0,0.25,0.1);
    userTemp++;
    timeCook +=5; //TODO CHANGE TIME BACK TO 10 INTERVALS
}

// Callback routine is interrupt activated by a debounced pb2 hit
void pbDown_hit_callback (void)
{
    // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
    myled2 = !myled2;
    mySpeaker.PlayNote(400.0,0.25,0.1);
    userTemp--;
    if (userTemp <=0) {
        userTemp = 0;
    }
    timeCook-=5;
    if (timeCook <=0)
        timeCook=0;
}
// Callback routine is interrupt activated by a debounced pb3 hit
void pbEnter_hit_callback (void)
{
    // CODE HERE WILL RUN WHEN INTERUPT IS GENERATED
    myled3 = !myled3;
    mySpeaker.PlayNote(800.0,0.10,0.1);
    mySpeaker.PlayNote(1000.0,0.10,0.1);
    mySpeaker.PlayNote(1200.0,0.10,0.1);
    check++;
}

////////////////////////////////////////////////////////
// Main method starts here
////////////////////////////////////////////////////////

int main()
{
//setup three SPST push buttons
    pbUp.mode(PullUp); //add internal pullup resistor
    pbDown.mode(PullUp);
    pbEnter.mode(PullUp);

// Delay for initial pullup to take effect
    wait(.01);

// Setup Interrupt callback functions for a pb hit
    pbUp.attach_deasserted(&pbUp_hit_callback);
    pbDown.attach_deasserted(&pbDown_hit_callback);
    pbEnter.attach_deasserted(&pbEnter_hit_callback);

// Start sampling pb inputs using interrupts
    pbUp.setSampleFrequency();
    pbDown.setSampleFrequency();
    pbEnter.setSampleFrequency();

// pushbuttons now setup and running

////////////////////////////////////////////////////////
// Ask for temperature and time desired
////////////////////////////////////////////////////////

    uLCD.printf("Enter desired temperature"); //Ask for desired temperature
    while(check ==0) { //if check flag is 0, means enter has not been pushed
        if (pbUp) {
            uLCD.locate(0,3);
            uLCD.printf("%4.0f", userTemp);
        }
        if (pbDown) {
            uLCD.locate(0,3);
            uLCD.printf("%4.0f", userTemp);
        }
    }

    float finalTemp = userTemp; //reassign to final temp
    check = 0;
    uLCD.cls();
    uLCD.locate(0,0);
    timeCook = 0; //reset timeCook
    uLCD.printf("Enter cooking time"); //Cooking time in minutes
    while (check == 0) {
        if (pbUp) {
            uLCD.locate(0,3);
            uLCD.printf("%4.0f", timeCook);
        }
        if (pbDown) {
            uLCD.locate(0,3);
            uLCD.printf("%4.0f", timeCook);
        }
    }
    float finalTime = timeCook *60; //reassign to final time
    ctrl = 1;                   // Turn on relay
    float tempLimit = 0;        //intitialize variable to track temperature
    uLCD.locate(0,0);
    uLCD.printf("Set Temp (C)", finalTemp);  //print set temperature
    while (tempLimit < (finalTemp + 1)) {       //Loop while temperature is 1 degrees over destination
        probe.convertTemperature(true, DS1820::all_devices);         //Start temperature conversion, wait until ready
        tempLimit = probe.temperature();
        uLCD.locate(0,3);
        uLCD.printf("It is %3.1foC\r", probe.temperature());
        wait(1);
    }
////////////////////////////////////////////////////////
// Start cooking with finalized temperatures and time
////////////////////////////////////////////////////////

    //PLAY SOUND FROM SPEAKER
    /*    T.start();  //Timer start (s)*/
    float time_remaining = 1;
    ctrl = 0;
    uLCD.cls();
    uLCD.locate(0,0);
    uLCD.printf("Set Temperature %4.0foC\r", finalTemp);
    set_time(1256729737);  // Set RTC time to Wed, 28 Oct 2009 11:35:37
    // time_t seconds = time(NULL);

    while(time_remaining > 0) { //Loop while timer has not reached desired time
        if (tempLimit < (finalTemp - 1)) { //If temperature is 2 degrees C lower than desired, turn relay on
            ctrl = 1;
        } else if (tempLimit >= finalTemp) {
            ctrl = 0;
        }
        tempLimit = probe.temperature();
        probe.convertTemperature(true, DS1820::all_devices);        //Start temperature conversion, wait until ready
        uLCD.locate(0,3);
        uLCD.printf("It is %3.1foC\r", probe.temperature());
        uLCD.locate(0,5);
        time_t seconds = time(NULL); //Cycle RTC
        time_remaining = finalTime - (seconds - 1256729737);

        if ((time_remaining/3600) >= 1 )
            uLCD.printf("Time remaining: \n%4.0f hours", time_remaining/3600);
        else if ((time_remaining/60) >= 1)
            uLCD.printf("Time remaining: %4.0f minutes", time_remaining/60);
        else
            uLCD.printf("Time remaining: %4.0f seconds", time_remaining);
    }

    ctrl = 0;
    
    //PLAY SOUND FROM SPEAKER
    mySpeaker.PlayNote(1567.98,0.25,0.1);
    mySpeaker.PlayNote(1046.50,0.25,0.1);
    mySpeaker.PlayNote(1174.66,0.25,0.1);
    mySpeaker.PlayNote(1318.51,0.25,0.1);
    mySpeaker.PlayNote(1396.91,0.25,0.1);
    mySpeaker.PlayNote(1567.98,0.25,0.1);
    mySpeaker.PlayNote(1046.50,0.25,0.1);
    mySpeaker.PlayNote(1046.50,0.25,0.1);
    mySpeaker.PlayNote(1760.00,0.25,0.1);
    mySpeaker.PlayNote(1396.91,0.25,0.1);
    mySpeaker.PlayNote(1567.98,0.25,0.1);
    mySpeaker.PlayNote(1760.00,0.25,0.1);
    mySpeaker.PlayNote(1975.53,0.25,0.1);
    mySpeaker.PlayNote(2093.00,0.25,0.1);
    mySpeaker.PlayNote(1046.50,0.25,0.1);
    mySpeaker.PlayNote(1046.50,0.25,0.1);
    
    uLCD.cls();
    uLCD.locate(0,0);
    uLCD.printf("Food is ready");
}

