//From the 1Sheeld website: Adding #define CUSTOM_SETTINGS along with #define INCLUDE_XX_SHIELD is a key 
//that will let you unlock XX shield only among all shields, 
//preventing 1Sheeld library from eating your memory

#define CUSTOM_SETTINGS
#define INCLUDE_INTERNET_SHIELD
#define INCLUDE_TERMINAL_SHIELD
#define INCLUDE_GPS_SHIELD
#include <OneSheeld.h>

int previousButtonState;
int currentButtonState;

//Period of time during which at least 3 button presses have to be registered to trigger an Uber request
//I chose 1.5 seconds (1500 miliseconds)
const unsigned short interval = 1500;

int pressCounter = 0;
unsigned long startTime = 0;

//Setup for the http requests we will make to the Uber API
//We will need the ride request endpoint and the ride status endpoint
//... obviously so that we can request a cab and then check on its progress
HttpRequest RideRequest("https://sandbox-api.uber.com/v1.2/requests");
HttpRequest RideStatus("https://sandbox-api.uber.com/v1.2/requests/current");

String longitude;
String latitude;

// I realise this is inelegant, but it was the simplest way I found 
//to make 1Sheeld send the exact http request Uber wanted
//(See later)
String myString1, myString2, myString3, myString4, myString5, myString6;
const int buttonPin = 9;

const int led1 = 11;  //red
const int led2 = 10;   //orange
const int led3 = 12;   //white

//Later on, where we check the status of the Uber ride request, the different colour lights have different meanings
//red = heel click successfully registered
//orange = ride successfully called
//white = ride accepted and on the way
//all together = your driver is here

int thisResponse =0;

void setup()
{

  OneSheeld.begin();
  
  //Add headers for the http requests – Uber uses OAuth 2.0 authentication
  //so you will need to make an account on developer.uber.com and generate yourself an authorisation token to add here
   RideStatus.addHeader("Authorization", "<PUT YOUR TOKEN HERE>");
   RideStatus.addHeader("Content-Type", "application/json");
   RideStatus.addHeader("Accept-Language", "en_EN");
   
   //These functions are covered in 1Sheeld's documentation
   //but essentially, they set what happens when a http request is successfully made
   //and what happens when a JSON response is received 
   RideStatus.setOnSuccess(&statusSuccess);
   RideStatus.getResponse().setOnJsonResponse(&statusResponse);
   
   RideRequest.addHeader("Authorization", "<PUT YOUR TOKEN HERE>");
   RideRequest.addHeader("Content-Type", "application/json");  
   RideRequest.addHeader("Accept-Language", "en_EN");
    
   RideRequest.setOnSuccess(&onSuccess);
  
pinMode(buttonPin, INPUT);
digitalWrite(buttonPin, HIGH);
currentButtonState = digitalRead(9);
previousButtonState = digitalRead(9);
pinMode(led1, OUTPUT);
pinMode(led2, OUTPUT);
pinMode(led3, OUTPUT);

}


void loop()
{
  
  //Read the button state so we can compare it to previous button state and find out if it has been pressed 
  currentButtonState = digitalRead(9);
  
  //Take the current time so we can check whether the interval during which 
  //we're listening for three button presses has elapsed or not
  unsigned long currentMillis = millis();
  
  //if the current interval hasn't elapsed, 
  //check for more button presses to update the press counter
  if (currentMillis - startTime <= interval)
  {
    
   //if the button state has changed compared with the previous button state, 
   //the button has been pressed since the last time we checked
   if(currentButtonState != previousButtonState && pressCounter != 0)
   {
    
     pressCounter +=1;
     
   }
    
  }
  
  //if the interval has elapsed, it's either time for a taxi 
  //or pressCounter hasn't reached 3 so we reset the counter
  //it will start again the next time we press the button
  if (currentMillis - startTime > interval)
  {
   
    
    if(pressCounter < 3)
    {

      pressCounter =0;
      
    }
    
    if(pressCounter >= 3)
    {
      
      //we've got 3 presses within our interval
      //show that by lighting one of the LEDs
      digitalWrite(led1, HIGH);
      
      //Use 1Sheeld to find out where you are
      //so you know where the cab needs to get to!
      myString2 = (String)GPS.getLatitude();
      myString4 = (String)GPS.getLongitude();
      
      //We need to specify some things in the data we send with this http request
      //we're going to start building strings that will concatenate to give the 
      //full data we want to send in one go
      myString1 = F("{\"start_latitude\":");
      myString3 = F(", \"start_longitude\":");
      
      //this is the location the cab will take you to
      //at the moment, it's hard coded, because the shoes just take you home
      myString5 = F(", \"end_latitude\":<INSERT HOME LATITUDE>, \"end_longitude\":<INSERT HOME LONGITUDE>}");
      
      //this is a particular code related to your region that Uber needs
      //it's hardcoded to match the region for your hardcoded destination
      //see the extra notes/documentation with this code for more details
      myString6 = F("{\"product_id\": \"<INSERT PRODUCT CODE HERE>\"");
      
      //assemble all this info into one single string of data to send in the http request
      RideRequest.addRawData(&(myString1+myString2+myString3+myString4+myString5)[0]);
      
      //Make the ride request!
      Internet.performPost(RideRequest);  
  
  //wait a little moment for stability/to let the request get to Uber
  OneSheeld.delay(10000);
  
  //then get the status
  do{
  
  //Keep getting the status every 10 seconds 
  Internet.performGet(RideStatus);
  OneSheeld.delay(10000);
  //(the response is handled by one of the functions underneath)
  
  } while (thisResponse != 1);
      
      //don't forget to reset press counter after you've called a cab
      //or you'll call a taxi on every loop
      pressCounter = 0;
    }
    
  }
  
  //If we didn't call a taxi in this loop, what happened? 
  //check whether we need to start another interval 
  //- i.e. the button has been pressed and the counter is at zero
  if(currentButtonState != previousButtonState && pressCounter ==0)
  {
   //the button has been pressed and the counter is at zero, 
   //so this is the start of a new interval
   pressCounter += 1;
   startTime = millis();
   
  }
  
  //store the button state, ready to read it again at the start of the loop 
  //and compare it with previous loop
  previousButtonState = currentButtonState;
    
}

//functions for what to do when requests are successful or you get a response

void onSuccess (HttpResponse & response)
{
 
 // 
}

void statusSuccess (HttpResponse & response2)
{
  
  //we want to pick out the "status" field from the Json response
  //that the status request gives us
 RideStatus.getResponse()["status"].query();
  
}

void statusResponse (JsonKeyChain & hell,char * res)
{
 
 //now we want to compare the "status" we got in the response
 //to Uber's list of possible statuses
 //The shoes looked better with just three LEDs
 //So I chose to represent just a few statuses 
 //At the moment, if the driver cancelled etc. then all the lights just go out
 //I figured it was simpler than trying to remember the meanings 
 //of a million different combinations of lights
 
if (strcmp (res,"processing") == 0)
 {
  
   digitalWrite(led1, LOW);
   digitalWrite(led2, HIGH);
 }
if (strcmp (res,"accepted") == 0)
 {
  
   digitalWrite(led2, LOW);
   digitalWrite(led3, HIGH);
 }
 if (strcmp (res,"arriving") == 0)
 {
  
   digitalWrite(led1, HIGH);
   digitalWrite(led2, HIGH);
   digitalWrite(led3, HIGH);
   thisResponse =1;
 }
 else()
 {
   digitalWrite(led1, LOW);
   digitalWrite(led2, LOW);
   digitalWrite(led3, LOW);
 }
 }
  
}
