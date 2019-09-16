
#ifndef LOCALMAP_HPP
#define LOCALMAP_HPP
#include <stdio.h>
#include <pthread.h>
#include <cstring>
#include <unordered_set>
#include <stdint.h>

#include "roombaController.hpp"


using namespace std;

class LocalMap
{
    static constexpr int CELL_SIZE_MM = 50;
    static constexpr int MAP_SIZE_PX = 500;
    static constexpr int ROOMBA_RADIUS_MM = 160;
    static constexpr uint8_t LTB_L = 1;
    static constexpr uint8_t LTB_FL = 2;
    static constexpr uint8_t LTB_CL = 4;
    static constexpr uint8_t LTB_CR = 8;
    static constexpr uint8_t LTB_FR = 16;
    static constexpr uint8_t LTB_R = 32;

    static constexpr int SCALE = 10; //mm per pixel

    typedef struct Point {
        int16_t x; int16_t y;
        Point(int16_t x1, int16_t y1):x(x1),y(y1){}
    } Point;

    // Custom comparator
    struct PointsEqual {
    public:
        bool operator()(const Point & p1, const Point & p2) const {

            if (p1.x==p2.x && p1.y==p2.y)
                return true;
            else
                return false;
        }
    };

    // Custom Hash Functor that will compute the hash on the
    // passed string objects length
    struct PointHash {
    public:
        size_t operator()(const Point & p) const {
            int size = p.y*10000+p.x;
            return std::hash<int>()(size);
        }
    };

public:
    uint8_t lightBumps=255;
    uint8_t bat =0;
    LocalMap();


    void updateObstaclePosition(int16_t dDist,int16_t dAngle);
    void addObstacles(uint8_t lightBumps);

    static inline int16_t round32(int16_t d){return d&0xffe0;}

    private:
        //wxTimer timerForRefresh;
        unordered_set<Point,PointHash,PointsEqual> obstacles;
    unordered_set<Point,PointHash,PointsEqual> obsTemp;

    int16_t previousXmm;
    int16_t previousYmm;

};



#endif //LOCALMAP_HPP

