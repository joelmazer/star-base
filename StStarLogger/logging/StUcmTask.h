#ifndef STUCMTASK_H
#define STUCMTASK_H

#include "StStarLogger/logging/StUcmJobs.h"
namespace TxLogging {
class StUcmTask : public StUcmJobs {
public: 
   StUcmTask();
   virtual ~StUcmTask();
   const RecordList &getJobs() const;
};
}
#endif
