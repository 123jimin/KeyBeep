#pragma once

struct IMMDeviceEnumerator;

class KeyBeep;

class Beeper
{
public:
	Beeper(KeyBeep& owner);
	~Beeper();

protected:
	KeyBeep& mOwner;
	IMMDeviceEnumerator* mDeviceEnumerator = nullptr;
	IMMDevice* mDevice = nullptr;
};