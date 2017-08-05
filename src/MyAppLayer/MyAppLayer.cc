//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <MyAppLayer/MyAppLayer.h>

Define_Module(MyAppLayer);

void MyAppLayer::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        //Initializing members and pointers of your application goes here
        //Getting the type of car
        this->myType = par("vehType").stringValue();
        std::cout << myId <<" my type is " << myType << endl;

        isParked = false;

        Veins::TraCIColor color(255,0,0,0);

        if(myType == "ambulance"){
            traciVehicle->setColor(color);
        }
    }
    else if (stage == 1) {
        //Initializing members that require initialized other modules goes here

    }
}

void MyAppLayer::finish() {
    BaseWaveApplLayer::finish();
    //statistics recording goes here

}

void MyAppLayer::onBSM(BasicSafetyMessage* bsm) {
    //Your application has received a beacon message from another car or RSU
    //code for handling the message goes here
    //std::cout << "My ID: " << myId << endl;
   // std::cout << "I just received one beacon from Vehicle: " << bsm->getSenderAddress() << endl;
    //std::cout << "This vehicle is of type: " << bsm->getVehType() << endl;
   // std::cout << bsm->getSenderSpeed() << endl;

    std::string bsmVehType;
    bsmVehType = bsm->getVehType();

    if(bsmVehType == "ambulance" and isParked == false){
        std::cout << "received." << endl;
        //Compute distance
        this->distance = mobility->getCurrentPosition().distance(bsm->getSenderPos());
        //Compute time parked to the ambulance overpass
        timeStopped = (this->distance / bsm->getSenderSpeed().x);
        //Compute more 10% of time
        timeStopped = timeStopped + (timeStopped*0.1);
        stoppedAtTime = simTime();
        traciVehicle->setSpeed(0.0);
        isParked = true;
    }
}

void MyAppLayer::onWSM(WaveShortMessage* wsm) {
    //Your application has received a data message from another car or RSU
    //code for handling the message goes here, see TraciDemo11p.cc for examples

}

void MyAppLayer::onWSA(WaveServiceAdvertisment* wsa) {
    //Your application has received a service advertisement from another car or RSU
    //code for handling the message goes here, see TraciDemo11p.cc for examples

}

void MyAppLayer::handleSelfMsg(cMessage* msg) {
    switch (msg->getKind()) {
        case SEND_BEACON_EVT: {

            if(isParked == true){
                if((simTime() - stoppedAtTime) > timeStopped){
                    traciVehicle->setSpeed(12.0);
                }
            }


            BasicSafetyMessage* bsm = new BasicSafetyMessage();
            //Put Veh Type on beacon before send it.
            bsm->setVehType(myType.c_str());
            //Put standard beacon parameters
            populateWSM(bsm);
            //send it to mac layer
            sendDown(bsm);
            //schedule time to next periodic beacon...
            scheduleAt(simTime() + beaconInterval, sendBeaconEvt);
            break;
        }
        case SEND_WSA_EVT:   {
            WaveServiceAdvertisment* wsa = new WaveServiceAdvertisment();
            populateWSM(wsa);
            sendDown(wsa);
            scheduleAt(simTime() + wsaInterval, sendWSAEvt);
            break;
        }
        default: {
            if (msg)
                DBG_APP << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
            break;
        }
    }

}

void MyAppLayer::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
    //the vehicle has moved. Code that reacts to new positions goes here.
    //member variables such as currentPosition and currentSpeed are updated in the parent class

}
