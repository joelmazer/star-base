/*
 * @file TxEventLogCollector.h
 * @author Valeri Fine
 *
 * @(#)cpp/api:$Id: TxEventLogCollector.h,v 1.2 2009/06/19 20:10:52 fine Exp $
 *
 * TxEventLogCollector provides an interface for applications so that they can send
 * event across of the Collector into a CEDPS formated messages.
 */
 
#ifndef TX_EVENT_LOG_COLLECTOR_H
#define TX_EVENT_LOG_COLLECTOR_H

#include "TxEventLogFile.h"

class TxUCMCollector;

namespace TxLogging {
  class TxEventLogCollector: public TxEventLogFile {
  private:
     TxUCMCollector *fCollector;
  protected:
    /**
     * Write down the prepared plain message using the Collector interface 
     * 
     * @param string message , Message to be written out
     *
     */
    virtual void writeDown(const std::string& message);
  
  public:
    /**
     * Constructor
     * - Gets hostname and sets nodeLocation for the running job.  
     *
     */    
    TxEventLogCollector();

    /**
     * Destructor: 
     * - Sets job state to finished (STAGE = END) 
     *
     */
    virtual ~TxEventLogCollector ();
  };
}
#endif