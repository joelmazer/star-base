#ifndef STUCMTASKS_H
#define STUCMTASKS_H

#include "StStarLogger/logging/StUcmTask.h"
namespace TxLogging {
class Iterator;
class StUcmTasks : public StUcmTask{
public: 
   StUcmTasks();
   virtual ~StUcmTasks();
   RecordList &getTasks();
   const RecordList &getTasks() const;
   Iterator taskIterator();
};
}
#endif
