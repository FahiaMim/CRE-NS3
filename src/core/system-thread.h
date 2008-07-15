/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage.inria.fr>
 */

#ifndef SYSTEM_THREAD_H
#define SYSTEM_THREAD_H

#include "callback.h"

namespace ns3 { 

class SystemThreadImpl;

/**
 * @brief A class which provides a relatively platform-independent thread
 * primitive.
 *
 * This class allows for creation of multiple threads of execution in a
 * process.  The exact implementation of the thread functionality is 
 * operating system dependent, but typically in ns-3 one is using an 
 * environment in which Posix Threads are supported (either navively or
 * in the case of Windows via Cygwin's implementation of pthreads on the 
 * Win32 API.  In either case we expect that these will be kernel-level
 * threads and therefore a system with multiple CPUs will see truly concurrent 
 * execution.
 *
 * Synchronization between threads is provided via the SystemMutex class.
 */
class SystemThread 
{
public:
  /**
   * @brief Create a SystemThread object.
   *
   * A system thread object is not created running.  A thread of execution
   * must be explicitly started by calling the Start method.  When the 
   * Start method is called, it will spawn a thread of execution and cause
   * that thread to call out into the callback function provided here as
   * a parameter.
   *
   * Like all ns-3 callbacks, the provided callback may refer to a function
   * or a method of an object depending on how the MakeCallback function is
   * used.
   *
   * The most common use is expected to be creating a thread of execution in
   * a method.  In this case you would use code similar to,
   *
   *   MyClass myObject;
   *   Ptr<SystemThread> st = Create<SystemThread> (
   *     MakeCallback (&MyClass::MyMethod, &myObject));
   *   st->Start ();
   *
   * The SystemThread is passed a callback that calls out to the function
   * MyClass::MyMethod.  When this function is called, it is called as an
   * object method on the myObject object.  Essentially what you are doing
   * is asking the SystemThread to call object->MyMethod () in a new thread
   * of execution.
   *
   * Remember that if you are invoking a callback on an object that is
   * managed by a smart pointer, you need to call PeekPointer.
   *
   *   Ptr<MyClass> myPtr = Create<MyClass> ();
   *   Ptr<SystemThread> st = Create<SystemThread> (
   *     MakeCallback (&MyClass::MyMethod, PeekPointer (myPtr)));
   *   st->Start ();
   *
   * Just like any thread, you can synchronize with its termination.  The 
   * method provided to do this is Join (). If you call Join() you will block
   * until the SystemThread run method returns.
   *
   * @warning I've made the system thread class look like a normal ns3 object
   * with smart pointers, and living in the heap.  This makes it very easy to
   * manage threads from a single master thread context.  You should be very
   * aware though that I have not made Ptr multithread safe!  This means that
   * if you pass Ptr<SystemThread> around in a multithreaded environment, it is
   * possible that the reference count will get messed up since it is not an
   * atomic operation.  CREATE AND MANAGE YOUR THREADS IN ONE PLACE -- LEAVE
   * THE PTR THERE.
   */
  SystemThread(Callback<void> callback);

  /**
   * @brief Destroy a SystemThread object.
   *
   */
  ~SystemThread();

  /**
   * Increment the reference count. This method should not be called
   * by user code. Object instances are expected to be used in conjunction
   * of the Ptr template which would make calling Ref unecessary and 
   * dangerous.
   */
  inline void Ref (void) const;

  /**
   * Decrement the reference count. This method should not be called
   * by user code. Object instances are expected to be used in conjunction
   * of the Ptr template which would make calling Ref unecessary and 
   * dangerous.
   */
  inline void Unref (void) const;

  /**
   * @brief Start a thread of execution, running the provided callback.
   */
  void Start (void);

  /**
   * @brief Suspend the caller until the thread of execution, running the 
   * provided callback, finishes.
   */
  void Join (void);

private:
  SystemThreadImpl * m_impl;
  mutable uint32_t m_count;
};

 void
SystemThread::Ref (void) const
{
  m_count++;
}

 void
SystemThread::Unref (void) const
{
  m_count--;
  if (m_count == 0)
    {
      delete this;
    }
}

} //namespace ns3

#endif /* SYSTEM_THREAD_H */


