
 /**
 * PS Move API - An interface for the PS Move Motion Controller
 * Copyright (c) 2016 Thomas Perl <m@thp.io>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/



#include <stdio.h>
#include <stdlib.h>
#include <sstream>

#include "psmoveapi.h"
#include "zhelpers.h"

void *context;
void *publisher;
char str2[180];

struct ControlHandler : public psmoveapi::Handler {
    ControlHandler() : color{0.f, 0.f, 0.f} , bt_controllers(0), rumble(0.f), quit(false), interactive(false) {}

    virtual void connect(Controller *controller) {
        printf("Connect: %s\n", controller->serial);
        if (controller->bluetooth) {
            bt_controllers++;
        }
    }

    virtual void update(Controller *controller) {
        int traingle = 0;
        int square = 0;
        int circle = 0;
            
        if ((controller->buttons & Btn_TRIANGLE) != 0) {
            traingle = 1;
            printf("Triangle pressed \n");
        } else {
            traingle = 0;                   
        }

       if ((controller->buttons & Btn_SQUARE) != 0) {
            square = 1;
            printf("Square pressed \n");
       } else {
            square = 0;                   
       }

        if ((controller->buttons & Btn_CIRCLE) != 0) {
            circle = 1;
            printf("Circle pressed \n");
        } else {
            circle = 0;                   
        }

        printf("x:%.4f  \n", controller->gyroscope.x);

        sprintf(str2, "{'x':'%.4f','y':'%.4f', 'z':'%.4f', 'traingle':'%d', 'square':'%d', 'circle':'%d'}", 
                controller->gyroscope.x, controller->gyroscope.y, controller->gyroscope.z, traingle, square, circle);; 
        s_sendmore(publisher, "x");
        s_send(publisher, str2);;


        if ((controller->buttons & Btn_PS) != 0) {
            quit = true;
        }
    }

    virtual void disconnect(Controller *controller) {
        printf("Disconnect: %s\n", controller->serial);
        if (controller->bluetooth) {
            bt_controllers--;
        }
    }

    RGB color;
    int bt_controllers;
    float rumble;
    bool quit;
    bool interactive;
};

int
main(int argc, char* argv[])
{
    context = zmq_ctx_new();
    publisher = zmq_socket(context, ZMQ_PUB);
    zmq_bind(publisher, "tcp://*:5556");

    ControlHandler handler;
    psmoveapi::PSMoveAPI api(&handler);

    api.update();

    handler.interactive = true;
    while (handler.bt_controllers > 0 && !handler.quit) {
        api.update();
    }

    return 0;
}
