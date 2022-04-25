/*
Adept MobileRobots Robotics Interface for Applications (ARIA)
Copyright (C) 2004-2005 ActivMedia Robotics LLC
Copyright (C) 2006-2010 MobileRobots Inc.
Copyright (C) 2011-2014 Adept Technology

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

If you wish to redistribute ARIA under different terms, contact 
Adept MobileRobots for information about a commercial version of ARIA at 
robots@mobilerobots.com or 
Adept MobileRobots, 10 Columbia Drive, Amherst, NH 03031; +1-603-881-7960
*/
/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.40
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.mobilerobots.Aria;

public class ArNetServer {
  /* (begin code from javabody typemap) */

  private long swigCPtr;
  protected boolean swigCMemOwn;

  /* for internal use by swig only */
  public ArNetServer(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  /* for internal use by swig only */
  public static long getCPtr(ArNetServer obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  /* (end code from javabody typemap) */

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        AriaJavaJNI.delete_ArNetServer(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public ArNetServer(boolean addAriaExitCB, boolean doNotAddShutdownServer, String name, ArNetServer childServer) {
    this(AriaJavaJNI.new_ArNetServer__SWIG_0(addAriaExitCB, doNotAddShutdownServer, name, ArNetServer.getCPtr(childServer), childServer), true);
  }

  public ArNetServer(boolean addAriaExitCB, boolean doNotAddShutdownServer, String name) {
    this(AriaJavaJNI.new_ArNetServer__SWIG_1(addAriaExitCB, doNotAddShutdownServer, name), true);
  }

  public ArNetServer(boolean addAriaExitCB, boolean doNotAddShutdownServer) {
    this(AriaJavaJNI.new_ArNetServer__SWIG_2(addAriaExitCB, doNotAddShutdownServer), true);
  }

  public ArNetServer(boolean addAriaExitCB) {
    this(AriaJavaJNI.new_ArNetServer__SWIG_3(addAriaExitCB), true);
  }

  public ArNetServer() {
    this(AriaJavaJNI.new_ArNetServer__SWIG_4(), true);
  }

  public boolean open(ArRobot robot, long port, String password, boolean multipleClients, String openOnIP) {
    return AriaJavaJNI.ArNetServer_open__SWIG_0(swigCPtr, this, ArRobot.getCPtr(robot), robot, port, password, multipleClients, openOnIP);
  }

  public boolean open(ArRobot robot, long port, String password, boolean multipleClients) {
    return AriaJavaJNI.ArNetServer_open__SWIG_1(swigCPtr, this, ArRobot.getCPtr(robot), robot, port, password, multipleClients);
  }

  public boolean open(ArRobot robot, long port, String password) {
    return AriaJavaJNI.ArNetServer_open__SWIG_2(swigCPtr, this, ArRobot.getCPtr(robot), robot, port, password);
  }

  public void close() {
    AriaJavaJNI.ArNetServer_close(swigCPtr, this);
  }

  public boolean addCommand(String command, SWIGTYPE_p_ArFunctor3T_char_pp_int_ArSocket_p_t functor, String help) {
    return AriaJavaJNI.ArNetServer_addCommand(swigCPtr, this, command, SWIGTYPE_p_ArFunctor3T_char_pp_int_ArSocket_p_t.getCPtr(functor), help);
  }

  public boolean remCommand(String command) {
    return AriaJavaJNI.ArNetServer_remCommand(swigCPtr, this, command);
  }

  public String getName() {
    return AriaJavaJNI.ArNetServer_getName(swigCPtr, this);
  }

  public void sendToAllClientsPlain(String str) {
    AriaJavaJNI.ArNetServer_sendToAllClientsPlain(swigCPtr, this, str);
  }

  public void sendToAllClientsNextCyclePlain(String str) {
    AriaJavaJNI.ArNetServer_sendToAllClientsNextCyclePlain(swigCPtr, this, str);
  }

  public boolean sendToAllClientsNextCyclePlainBool(String str) {
    return AriaJavaJNI.ArNetServer_sendToAllClientsNextCyclePlainBool(swigCPtr, this, str);
  }

  public void sendToClientPlain(ArSocket socket, String ipString, String str) {
    AriaJavaJNI.ArNetServer_sendToClientPlain(swigCPtr, this, ArSocket.getCPtr(socket), socket, ipString, str);
  }

  public boolean isOpen() {
    return AriaJavaJNI.ArNetServer_isOpen(swigCPtr, this);
  }

  public void setLoggingDataSent(boolean loggingData) {
    AriaJavaJNI.ArNetServer_setLoggingDataSent(swigCPtr, this, loggingData);
  }

  public boolean getLoggingDataSent() {
    return AriaJavaJNI.ArNetServer_getLoggingDataSent(swigCPtr, this);
  }

  public void setLoggingDataReceived(boolean loggingData) {
    AriaJavaJNI.ArNetServer_setLoggingDataReceived(swigCPtr, this, loggingData);
  }

  public boolean getLoggingDataReceived() {
    return AriaJavaJNI.ArNetServer_getLoggingDataReceived(swigCPtr, this);
  }

  public void setUseWrongEndChars(boolean useWrongEndChars) {
    AriaJavaJNI.ArNetServer_setUseWrongEndChars(swigCPtr, this, useWrongEndChars);
  }

  public boolean getUseWrongEndChars() {
    return AriaJavaJNI.ArNetServer_getUseWrongEndChars(swigCPtr, this);
  }

  public void runOnce() {
    AriaJavaJNI.ArNetServer_runOnce(swigCPtr, this);
  }

  public void internalGreeting(ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalGreeting(swigCPtr, this, ArSocket.getCPtr(socket), socket);
  }

  public void internalHelp(ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalHelp__SWIG_0(swigCPtr, this, ArSocket.getCPtr(socket), socket);
  }

  public void internalHelp(SWIGTYPE_p_p_char argv, int argc, ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalHelp__SWIG_1(swigCPtr, this, SWIGTYPE_p_p_char.getCPtr(argv), argc, ArSocket.getCPtr(socket), socket);
  }

  public void internalEcho(SWIGTYPE_p_p_char argv, int argc, ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalEcho(swigCPtr, this, SWIGTYPE_p_p_char.getCPtr(argv), argc, ArSocket.getCPtr(socket), socket);
  }

  public void internalQuit(SWIGTYPE_p_p_char argv, int argc, ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalQuit(swigCPtr, this, SWIGTYPE_p_p_char.getCPtr(argv), argc, ArSocket.getCPtr(socket), socket);
  }

  public void internalShutdownServer(SWIGTYPE_p_p_char argv, int argc, ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalShutdownServer(swigCPtr, this, SWIGTYPE_p_p_char.getCPtr(argv), argc, ArSocket.getCPtr(socket), socket);
  }

  public void parseCommandOnSocket(ArArgumentBuilder args, ArSocket socket, boolean allowLog) {
    AriaJavaJNI.ArNetServer_parseCommandOnSocket__SWIG_0(swigCPtr, this, ArArgumentBuilder.getCPtr(args), args, ArSocket.getCPtr(socket), socket, allowLog);
  }

  public void parseCommandOnSocket(ArArgumentBuilder args, ArSocket socket) {
    AriaJavaJNI.ArNetServer_parseCommandOnSocket__SWIG_1(swigCPtr, this, ArArgumentBuilder.getCPtr(args), args, ArSocket.getCPtr(socket), socket);
  }

  public void internalAddSocketToList(ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalAddSocketToList(swigCPtr, this, ArSocket.getCPtr(socket), socket);
  }

  public void internalAddSocketToDeleteList(ArSocket socket) {
    AriaJavaJNI.ArNetServer_internalAddSocketToDeleteList(swigCPtr, this, ArSocket.getCPtr(socket), socket);
  }

  public void squelchNormal() {
    AriaJavaJNI.ArNetServer_squelchNormal(swigCPtr, this);
  }

  public void setExtraString(String str) {
    AriaJavaJNI.ArNetServer_setExtraString(swigCPtr, this, str);
  }

  public String getExtraString() {
    return AriaJavaJNI.ArNetServer_getExtraString(swigCPtr, this);
  }

  public int lock() {
    return AriaJavaJNI.ArNetServer_lock(swigCPtr, this);
  }

  public int tryLock() {
    return AriaJavaJNI.ArNetServer_tryLock(swigCPtr, this);
  }

  public int unlock() {
    return AriaJavaJNI.ArNetServer_unlock(swigCPtr, this);
  }

}
