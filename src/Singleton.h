// by Mark Featherstone (C) 2019 All Rights Reserved.
#ifndef SINGLETON_H
#define SINGLETON_H

/*
We often want only one instance of an object, especially if we are creating manager classes
to hold onto groups of objects and manage their lifetime. A singleton is a "design pattern"
designed to do just that - ensure there is only ever one instance of a specific class.

As we'll use it again and again, we don't want to repeat ourselves, this template and the
two macros help us write less code to get the job done.
Example - declaration
---------------------
	#include "Singleton.h"
	class ModeManager : public Singleton<ModeManager>
	{
	public:
		//add all your functions and data
	};

Example - in use
----------------
	new ModeManager;
	....
	ModeManager::Get()->DoStuff();
	....
	delete GetModeManager();

*/

template<class T>
class Singleton
{
public:
    // Constructor: Ensures that only one instance of the class exists.
    Singleton()
    {
        assert(spSingleton == nullptr); // Ensure no other instance exists.
        spSingleton = static_cast<T*>(this); // Assign this instance to the static pointer.
    }

    // Destructor: Cleans up the singleton instance.
    virtual ~Singleton()
    {
        assert(spSingleton != nullptr); // Ensure the instance exists before destruction.
        spSingleton = nullptr; // Set the static instance pointer to nullptr.
    }

    // Get: Provides access to the singleton instance.
    static T& Get()
    {
        assert(spSingleton != nullptr); // Ensure the instance exists before accessing.
        return *spSingleton; // Return a reference to the singleton instance.
    }

private:
    static T* spSingleton; // Static pointer to the singleton instance.

    // Disable copy constructor and assignment operator.
    Singleton(Singleton const&) = delete;
    void operator=(Singleton const&) = delete;
};

// Initialize the static instance pointer to nullptr.
template<typename T>
T* Singleton<T>::spSingleton = 0;

#endif
