#ifndef EVENT_H
#define EVENT_H

//this is the event structure.
struct Event
{
  unsigned long timer;    //this will hold the millis() of the last time the "event" was triggered
  unsigned int frequency; //this is the amount of time in ms between event triggers.
  bool flag = false;      //this will initially have a false value when you make a new event
  bool runTimer = false;  //adds a second run flag to event for
};

#endif