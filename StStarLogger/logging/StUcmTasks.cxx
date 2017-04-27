#include "StStarLogger/logging/StUcmTasks.h"
#include "StStarLogger/logging/StRecordIterator.h"
using namespace TxLogging;
StUcmTasks::StUcmTasks()  {;}
StUcmTasks::~StUcmTasks() {;}
const RecordList &StUcmTasks::getTasks() const
{
   return getRecords();
}

RecordList &StUcmTasks::getTasks()
{
   return getRecords();
}
Iterator StUcmTasks::taskIterator()
{
   return getTasks().iterator();
}



