#include "localMap.hpp"
#include <iostream>
#include <cmath>
LocalMap::LocalMap()
{

}




void LocalMap::updateObstaclePosition(int16_t dDist, int16_t dAngle)
{
    obsTemp.clear();
    float dtr = 3.1415926/70;//should be pi/180, but roomba returns deg/3
 dAngle = -dAngle;
    for (auto o: obstacles) {
        int16_t x = o.x * std::cos(dAngle*dtr)+o.y * sin(dAngle*dtr);
        int16_t y = -o.x * std::sin(dAngle*dtr)+o.y * cos(dAngle*dtr);
        y += 13*dDist*cos(dAngle*dtr);
        x += 13*dDist*sin(dAngle*dtr);
        obsTemp.insert(Point(x,y));
    }
    obstacles=obsTemp;

}

void LocalMap::addObstacles(uint8_t lightBumps)
{
    //    int16_t s1 = 31;
    //    int16_t s2 = -31;
    //    s1 = round32(s1);
    //    s2 = round32(s2);
    //    Point p(s1,s2);
    //    obstacles.insert(p);
    //    cout<<s1<<" "<<s2<<" obs size: "<<obstacles.size()<<"\n";

    if(lightBumps&LTB_L)
        obstacles.insert(Point(round32(241),round(65)));
    if(lightBumps&LTB_FL)
        obstacles.insert(Point(round32(176),round32(176)));
    if(lightBumps&LTB_CL)
        obstacles.insert(Point(round32(65),round32(241)));
    if(lightBumps&LTB_CR)
        obstacles.insert(Point(round32(-64),round32(241)));
    if(lightBumps&LTB_FR)
        obstacles.insert(Point(round32(-176),round32(176)));
    if(lightBumps&LTB_R)
        obstacles.insert(Point(round32(-241),round32(65)));

}



