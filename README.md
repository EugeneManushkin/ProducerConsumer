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
#TODO

Implement multithread producer-consumer model. 
Class Stopper must be implemented and any synchronization must use Win32 API. Using C++11 is allowed and solution must work under Windows XP. 
Program must work 30 seconds then execution must be stopped so remaining requests are unhandled.
