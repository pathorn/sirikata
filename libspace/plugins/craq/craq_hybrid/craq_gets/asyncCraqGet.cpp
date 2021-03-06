/*  Sirikata
 *  asyncCraqGet.cpp
 *
 *  Copyright (c) 2010, Behram Mistree
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "asyncCraqGet.hpp"
#include <iostream>
#include <boost/bind.hpp>
#include <string.h>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/asio.hpp>


#include "asyncConnectionGet.hpp"
#include "../asyncCraqUtil.hpp"
#include <sirikata/core/network/IOStrandImpl.hpp>
#include <sirikata/core/network/Asio.hpp>

#define CRAQ_MAX_PUSH_GET 10

namespace Sirikata
{

  void AsyncCraqGet::stop()
  {
#ifdef ASYNC_CRAQ_GET_DEBUG
    std::cout<<"\n\nReceived a stop in async craq get\n";
#endif
    for (int s=0; s < (int) mConnections.size(); ++s)
      mConnectionsStrands[s]->post(std::tr1::bind(&AsyncConnectionGet::stop,mConnections[s]));
  }

  AsyncCraqGet::~AsyncCraqGet()
  {
    for (int s= 0;s < (int) mConnections.size(); ++s)
    {
      delete mConnections[s];
      delete mConnectionsStrands[s];
    }
    mConnections.clear();
    mConnectionsStrands.clear();

  }

  int AsyncCraqGet::getRespCount()
  {
    int returner = 0;
    for (int s=0; s < (int) mConnections.size(); ++s)
    {
      returner += mConnections[s]->getRespCount();
    }
    return returner;
  }


  //nothing to initialize
  AsyncCraqGet::AsyncCraqGet(SpaceContext* con, Network::IOStrand* strand_this_runs_on, Network::IOStrand* strand_to_post_results_to, CraqObjectSegmentation* parent_oseg_called)
   : ctx(con),
      mStrand(strand_this_runs_on),
      mResultsStrand(strand_to_post_results_to),
      mOSeg(parent_oseg_called)
  {
  }

  int AsyncCraqGet::runReQuery()
  {
    int returner = 0;
    for (int s=0; s < (int) mConnections.size(); ++s)
    {
      if (mConnections[s]->ready() == AsyncConnectionGet::READY)
      {
        returner += mConnections[s]->runReQuery();
      }
    }
    return returner;
  }

  void AsyncCraqGet::initialize(std::vector<CraqInitializeArgs> ipAddPort)
  {
    mIpAddPort = ipAddPort;

    Sirikata::Network::TCPResolver resolver((*ctx->ioService)); //a resolver can resolve a query into a series of endpoints.

    for (int s=0; s < STREAM_CRAQ_NUM_CONNECTIONS_GET; ++s)
    {

      Network::IOStrand* tmpStrand         = ctx->ioService->createStrand();
      mConnectionsStrands.push_back(tmpStrand);


      AsyncConnectionGet* tmpConn = new AsyncConnectionGet (ctx,             //space context
                                                            tmpStrand,       //strand specific for this connection
                                                            mStrand,         //use asyncCraqGet's strand to post errors back on.
                                                            mResultsStrand,  //strand to post results to.
                                                            this,            //tells connection to post errors back to this.
                                                            mOSeg,            //tells connection to post results back to mOSeg.
                                                            std::tr1::bind(&AsyncCraqGet::readyStateChanged,this,s));
      mConnections.push_back(tmpConn);
    }

    Sirikata::Network::TCPSocket* passSocket;

    if (((int)ipAddPort.size()) >= STREAM_CRAQ_NUM_CONNECTIONS_GET)
    {
      //just assign each connection a separate router (in order that they were provided).
      for (int s = 0; s < STREAM_CRAQ_NUM_CONNECTIONS_GET; ++s)
      {
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), ipAddPort[s].ipAdd.c_str(), ipAddPort[s].port.c_str());
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);  //creates a list of endpoints that we can try to connect to.

        passSocket   = new Sirikata::Network::TCPSocket((*ctx->ioService));
        mConnectionsStrands[s]->post(std::tr1::bind(&AsyncConnectionGet::initialize, mConnections[s],passSocket, iterator));
      }
    }
    else
    {
      int whichRouterServingPrevious = -1;
      int whichRouterServing;
      double percentageConnectionsServed;

      boost::asio::ip::tcp::resolver::iterator iterator;

      for (int s=0; s < STREAM_CRAQ_NUM_CONNECTIONS_GET; ++s)
      {
        percentageConnectionsServed = ((double)s)/((double) STREAM_CRAQ_NUM_CONNECTIONS_GET);
        whichRouterServing = (int)(percentageConnectionsServed*((double)ipAddPort.size()));

        if (whichRouterServing  != whichRouterServingPrevious)
        {
          boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), ipAddPort[whichRouterServing].ipAdd.c_str(), ipAddPort[whichRouterServing].port.c_str());

          iterator = resolver.resolve(query);  //creates a list of endpoints that we can try to connect to.

          whichRouterServingPrevious = whichRouterServing;
        }

        passSocket   = new Sirikata::Network::TCPSocket((*ctx->ioService));
        mConnectionsStrands[s]->post(std::tr1::bind(&AsyncConnectionGet::initialize, mConnections[s],passSocket, iterator));
      }
    }
  }


  /*
    Returns the size of the queue of operations to be processed
  */
  int AsyncCraqGet::queueSize()
  {
    return mQueue.size();
  }


  int AsyncCraqGet::numStillProcessing()
  {
    int returner = 0;

    for (int s=0; s < (int)mConnections.size(); ++s)
    {
      returner = returner + ((int)mConnections[s]->numStillProcessing());
    }

    return returner;
  }

  void AsyncCraqGet::get(const CraqDataSetGet& dataToGet, OSegLookupTraceToken* traceToken)
  {
//    Duration beginGetEnqueueManager  = Time::local() - Time::epoch();
//    traceToken->getManagerEnqueueBegin =
//    beginGetEnqueueManager.toMicroseconds();

      traceToken->stamp(OSegLookupTraceToken::OSEG_TRACE_GET_MANAGER_ENQUEUE_BEGIN);

    CraqDataSetGet* cdQuery = new CraqDataSetGet(dataToGet.dataKey,dataToGet.dataKeyValue,dataToGet.trackMessage,CraqDataSetGet::GET);
    QueueValue* qValue = new QueueValue;
    qValue->cdQuery = cdQuery;
    qValue->traceToken = traceToken;
    pushQueue(qValue);

    // Duration endGetEnqueueManager = Time::local() - Time::epoch();
    // traceToken->getManagerEnqueueEnd = endGetEnqueueManager.toMicroseconds();
    traceToken->stamp(OSegLookupTraceToken::OSEG_TRACE_GET_MANAGER_ENQUEUE_END);
    
    int numTries = 0;
    while((mQueue.size()!= 0) && (numTries < CRAQ_MAX_PUSH_GET))
    {
      ++numTries;
      int rand_connection = rand() % STREAM_CRAQ_NUM_CONNECTIONS_GET;
      checkConnections(rand_connection);
    }
  }


  void AsyncCraqGet::erroredSetValue(CraqOperationResult* cor)
  {
#ifdef ASYNC_CRAQ_GET_DEBUG
    std::cout<<"\n\nShould not received an errored set inside of asyncCraqGet.cpp\n\n";
#endif
    assert(false);
  }


  //will be posted to from different connections.  am responsible for deleting.
  void AsyncCraqGet::erroredGetValue(CraqOperationResult* errorRes)
  {
    if (errorRes->whichOperation == CraqOperationResult::GET)
    {
      QueueValue * qVal = new QueueValue;
      CraqDataSetGet* cdSG = new CraqDataSetGet (errorRes->objID,errorRes->servID,errorRes->tracking, CraqDataSetGet::GET);
      qVal->cdQuery = cdSG;
      qVal->traceToken = errorRes->traceToken;

      pushQueue(qVal);
    }
    else
    {
#ifdef ASYNC_CRAQ_GET_DEBUG
      std::cout<<"\n\nShould never be receiving an error result for a set in asyncCraqGet.cpp\n\n";
#endif
      assert(false);
    }

    delete errorRes;
  }
void AsyncCraqGet::readyStateChanged(int s) {
    mReadyConnections.push_back(s);
    while (checkConnections(s)) {
        if (mQueue.empty()) {
            break;
        }
    }
}

void AsyncCraqGet::pushQueue(QueueValue*qv) {
    mQueue.push(qv);
    while (!mReadyConnections.empty()) {
        unsigned int rnd=rand()%mReadyConnections.size();
        if (checkConnections(mReadyConnections[rnd])) {
            if (mQueue.empty()) {
                break;
            }
        }else {
            mReadyConnections[rnd]=mReadyConnections.back();
            mReadyConnections.pop_back();
        }
    }
}
/*
  This function checks connection s to see if it needs a new socket or if it's ready to accept another query.
*/
bool AsyncCraqGet::checkConnections(int s)
{
  if (s>=(int)mConnections.size()){
      return false;
  }
  assert(s<(int)mConnections.size());

  int numOperations = 0;

  if (mConnections[s]->ready() == AsyncConnectionGet::READY)
  {
    if (mQueue.size() != 0)
    {
      //need to put in another
      QueueValue* qVal  = mQueue.front();
      CraqDataSetGet* cdSG = qVal->cdQuery;
      mQueue.pop();

      ++numOperations;

      // Duration dequeueManager  = Time::local() - Time::epoch();
      // qVal->traceToken->getManagerDequeued = dequeueManager.toMicroseconds();
      qVal->traceToken->stamp(OSegLookupTraceToken::OSEG_TRACE_GET_MANAGER_DEQUEUED);
      
      if (cdSG->messageType == CraqDataSetGet::GET)
      {
        //perform a get in  connections.
        CraqObjectID tmpCraqID;
        memcpy(tmpCraqID.cdk, cdSG->dataKey, CRAQ_DATA_KEY_SIZE);
        mConnections[s]->setProcessing();
        mConnectionsStrands[s]->post(std::tr1::bind(&AsyncConnectionGet::getBound,mConnections[s],tmpCraqID, qVal->traceToken));

      }
      else if (cdSG->messageType == CraqDataSetGet::SET)
      {
#ifdef ASYNC_CRAQ_GET_DEBUG
        std::cout<<"\n\nShould be incapable of performing a set from asyncCraqGet\n\n";
#endif
        assert(false);
      }
      delete cdSG;
      delete qVal;
    }
  }
  else if (mConnections[s]->ready() == AsyncConnectionGet::NEED_NEW_SOCKET)
  {
    //need to create a new socket for the other
    reInitializeNode(s);
#ifdef ASYNC_CRAQ_GET_DEBUG
    std::cout<<"\n\nbftm debug: needed new connection.  How long will this take? \n\n";
#endif
  }
  else
  {
      return false;
  }
  return true;
}

//means that we need to connect a new socket to the service.
void AsyncCraqGet::reInitializeNode(int s)
{
  if (s >= (int)mConnections.size())
    return;


  Sirikata::Network::TCPSocket* passSocket;
  Sirikata::Network::TCPResolver resolver(*ctx->ioService);   //a resolver can resolve a query into a series of endpoints.

  if ( ((int)mIpAddPort.size()) >= STREAM_CRAQ_NUM_CONNECTIONS_GET)
  {
    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), mIpAddPort[s].ipAdd.c_str(), mIpAddPort[s].port.c_str());
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);  //creates a list of endpoints that we can try to connect to.

    passSocket   =  new Sirikata::Network::TCPSocket(*ctx->ioService);

    mConnectionsStrands[s]->post(std::tr1::bind(&AsyncConnectionGet::initialize, mConnections[s],passSocket, iterator));
  }
  else
  {

    boost::asio::ip::tcp::resolver::iterator iterator;

    double percentageConnectionsServed = ((double)s)/((double) STREAM_CRAQ_NUM_CONNECTIONS_GET);
    int whichRouterServing = (int)(percentageConnectionsServed*((double)mIpAddPort.size()));

    //    whichRouterServing = 0; //bftm debug

    boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), mIpAddPort[whichRouterServing].ipAdd.c_str(), mIpAddPort[whichRouterServing].port.c_str());
    iterator = resolver.resolve(query);  //creates a list of endpoints that we can try to connect to.

    passSocket   =  new Sirikata::Network::TCPSocket(*ctx->ioService);

    mConnectionsStrands[s]->post(std::tr1::bind(&AsyncConnectionGet::initialize, mConnections[s],passSocket, iterator));
  }
}



} //end namespace
