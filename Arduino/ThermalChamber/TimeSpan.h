#ifndef __TIME_SPAN_H
#define __TIME_SPAN_H

unsigned long calculateTimeSpan(unsigned long currentTime, unsigned long previousTime)
{
  if(currentTime>=previousTime)return currentTime-previousTime;
  else return 0xFFFFFFFF - previousTime + currentTime;
}


#endif
