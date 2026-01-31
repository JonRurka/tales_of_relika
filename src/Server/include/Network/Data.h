#pragma once

#include "../stdafx.h"

typedef unsigned int Protocal;

#define Protocal_Tcp (Protocal)0
#define Protocal_Udp (Protocal)1

#ifndef DECLARE_NET_CLIENT_COMMAND
#define DECLARE_NET_CLIENT_COMMAND(type, func_name) 									 \
	static void func_name##_cb(void* obj, Data data) {	                                     \
		type* cntrl = (type*)obj;													 \
		cntrl->func_name(data);												 \
	}																				 \
	void func_name(Data data); 
#endif

class Data {
public:
    Protocal Type = 0;
    uint8_t command = 0;
    std::vector<uint8_t> Buffer;
    std::string Input;

    Data(Protocal type, uint8_t cmd, std::vector<uint8_t> data) {
        Type = type;
        command = cmd;
        Buffer = data;
        Input = std::string(Buffer.begin(), Buffer.end());
    }

    Data(){

    }
};