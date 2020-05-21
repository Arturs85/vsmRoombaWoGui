#include "baseProtocol.hpp"

BaseProtocol::BaseProtocol(BaseCommunicationBehaviour *ownerBeh)
{
    this->behaviour = ownerBeh;
}

vector<int> BaseProtocol::stringTointVector(string s){
    vector<int> res;
    std::stringstream ss(s);
    int i;

    while(ss>>i){

        //if(ss>>i)
            res.push_back(i);

    }
    return  res;
}

string BaseProtocol::intVectorToString(vector<int> iv)
{
    std::stringstream ss;
    for(auto i:iv){
        ss<< i<<" ";
    }
    return ss.str();
}



bool BaseProtocol::tick()
{
    
}

void BaseProtocol::start()
{
    
}
