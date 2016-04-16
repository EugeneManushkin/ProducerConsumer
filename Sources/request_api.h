#pragma once

#include <memory>

class Request
{
};

typedef void* Stopper;

Request* GetRequest(Stopper stopSignal);
void ProcessRequest(Request* request, Stopper stopSignal);
void DeleteRequest(Request* request);