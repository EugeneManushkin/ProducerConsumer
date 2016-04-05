# ProducerConsumer
This is a test program for an IT interview. Given following api:
```
class Request
{
};

///@brief      Request factory. Can take some time to complete. User can stop execution using stopSignal object
///@return     0 if stopSignal triggered, pointer to Request object
Request* GetRequest(Stopper stopSignal);

///@brief      Handle request but not delete it. User can stop execution using stopSignal object.
void ProcessRequest(Request* request, Stopper stopSignal);

///@brief      Request deleter
void DeleteRequest(Request* request);
```
Implement class Stopper and multithreaded producer-consumer model with two or more threads that are using GetRequest to generate request objects and ProcessRequest to handle requests. Program must work 30 seconds then execution must be stopped so remaining requests should be unhandled. Any synchronization must be done with Win32 API however using C++11 is allowed. Program must work under Windows XP.

#Install
- Download latest released archive, extract it.
- In terminal cd to folder with extracted files, type 'ProducerConsumer.exe'

#Build
Use Visual Studio 2013 solution file VS/ProducerConsumer.sln to build program for Windows.
