/*******************************************************************************
*
* E M B E D D E D   W I Z A R D   P R O J E C T
*
*                                                Copyright (c) TARA Systems GmbH
*                                    written by Paul Banach and Manfred Schweyer
*
********************************************************************************
*
* This file was generated automatically by Embedded Wizard Studio.
*
* Please do not make any modifications of this file! The modifications are lost
* when the file is generated again by Embedded Wizard Studio!
*
* The template of this heading text can be found in the file 'head.ewt' in the
* directory 'Platforms' of your Embedded Wizard installation directory. If you
* wish to adapt this text, please copy the template file 'head.ewt' into your
* project directory and edit the copy only. Please avoid any modifications of
* the original template file!
*
* Version  : 11.00
* Profile  : SysBrenner
* Platform : STM.STM32.RGB565
*
*******************************************************************************/

#ifndef _ApplicationDeviceClass_H
#define _ApplicationDeviceClass_H

#ifdef __cplusplus
  extern "C"
  {
#endif

#include "ewrte.h"
#if EW_RTE_VERSION != 0x000B0000
  #error Wrong version of Embedded Wizard Runtime Environment.
#endif

#include "ewgfx.h"
#if EW_GFX_VERSION != 0x000B0000
  #error Wrong version of Embedded Wizard Graphics Engine.
#endif

#include "_TemplatesDeviceClass.h"

/* Forward declaration of the class Application::DeviceClass */
#ifndef _ApplicationDeviceClass_
  EW_DECLARE_CLASS( ApplicationDeviceClass )
#define _ApplicationDeviceClass_
#endif


/* Deklaration of class : 'Application::DeviceClass' */
EW_DEFINE_FIELDS( ApplicationDeviceClass, TemplatesDeviceClass )
  EW_PROPERTY( ReceiveCounter,  XUInt32 )
  EW_PROPERTY( ParamCounter,    XUInt32 )
  EW_PROPERTY( NoOfParameter,   XInt32 )
  EW_PROPERTY( CurrentParameter, XInt32 )
  EW_PROPERTY( ShowQA,          XBool )
EW_END_OF_FIELDS( ApplicationDeviceClass )

/* Virtual Method Table (VMT) for the class : 'Application::DeviceClass' */
EW_DEFINE_METHODS( ApplicationDeviceClass, TemplatesDeviceClass )
EW_END_OF_METHODS( ApplicationDeviceClass )

/* 'C' function for method : 'Application::DeviceClass.GUIInit()' */
void ApplicationDeviceClass_GUIInit( ApplicationDeviceClass _this );

/* This method is intended to be called by the device to notify the GUI application 
   about an alternation of its setting or state value. */
void ApplicationDeviceClass_UpdateReceiveCounter( ApplicationDeviceClass _this, 
  XUInt32 aNewValue );

/* Wrapper function for the non virtual method : 'Application::DeviceClass.UpdateReceiveCounter()' */
void ApplicationDeviceClass__UpdateReceiveCounter( void* _this, XUInt32 aNewValue );

/* The following define announces the presence of the method Application::DeviceClass.UpdateReceiveCounter(). */
#define _ApplicationDeviceClass__UpdateReceiveCounter_

/* This method is intended to be called by the device to notify the GUI application 
   about an alternation of its setting or state value. */
void ApplicationDeviceClass_UpdateParamCounter( ApplicationDeviceClass _this, XUInt32 
  aNewValue );

/* Wrapper function for the non virtual method : 'Application::DeviceClass.UpdateParamCounter()' */
void ApplicationDeviceClass__UpdateParamCounter( void* _this, XUInt32 aNewValue );

/* The following define announces the presence of the method Application::DeviceClass.UpdateParamCounter(). */
#define _ApplicationDeviceClass__UpdateParamCounter_

/* This method is intended to be called by the device to notify the GUI application 
   about an alternation of its setting or state value. */
void ApplicationDeviceClass_UpdateQACounter( ApplicationDeviceClass _this, XUInt32 
  aNewValue );

/* Wrapper function for the non virtual method : 'Application::DeviceClass.UpdateQACounter()' */
void ApplicationDeviceClass__UpdateQACounter( void* _this, XUInt32 aNewValue );

/* The following define announces the presence of the method Application::DeviceClass.UpdateQACounter(). */
#define _ApplicationDeviceClass__UpdateQACounter_

/* 'C' function for method : 'Application::DeviceClass.OnSetNoOfParameter()' */
void ApplicationDeviceClass_OnSetNoOfParameter( ApplicationDeviceClass _this, XInt32 
  value );

/* 'C' function for method : 'Application::DeviceClass.UpdateNoOfParameter()' */
void ApplicationDeviceClass_UpdateNoOfParameter( ApplicationDeviceClass _this, XInt32 
  value );

/* 'C' function for method : 'Application::DeviceClass.OnSetCurrentParameter()' */
void ApplicationDeviceClass_OnSetCurrentParameter( ApplicationDeviceClass _this, 
  XInt32 value );

/* 'C' function for method : 'Application::DeviceClass.UpdateCurrentParameter()' */
void ApplicationDeviceClass_UpdateCurrentParameter( ApplicationDeviceClass _this, 
  XInt32 value );

/* Default onget method for the property 'ReceiveCounter' */
XUInt32 ApplicationDeviceClass_OnGetReceiveCounter( ApplicationDeviceClass _this );

/* Default onset method for the property 'ReceiveCounter' */
void ApplicationDeviceClass_OnSetReceiveCounter( ApplicationDeviceClass _this, XUInt32 
  value );

/* Default onget method for the property 'ParamCounter' */
XUInt32 ApplicationDeviceClass_OnGetParamCounter( ApplicationDeviceClass _this );

/* Default onset method for the property 'ParamCounter' */
void ApplicationDeviceClass_OnSetParamCounter( ApplicationDeviceClass _this, XUInt32 
  value );

/* Default onget method for the property 'ShowQA' */
XBool ApplicationDeviceClass_OnGetShowQA( ApplicationDeviceClass _this );

/* Default onset method for the property 'ShowQA' */
void ApplicationDeviceClass_OnSetShowQA( ApplicationDeviceClass _this, XBool value );

/* Default onget method for the property 'NoOfParameter' */
XInt32 ApplicationDeviceClass_OnGetNoOfParameter( ApplicationDeviceClass _this );

/* Wrapper function for the non virtual method : 'Application::DeviceClass.OnGetNoOfParameter()' */
XInt32 ApplicationDeviceClass__OnGetNoOfParameter( void* _this );

/* The following define announces the presence of the method Application::DeviceClass.OnGetNoOfParameter(). */
#define _ApplicationDeviceClass__OnGetNoOfParameter_

/* Default onget method for the property 'CurrentParameter' */
XInt32 ApplicationDeviceClass_OnGetCurrentParameter( ApplicationDeviceClass _this );

/* Wrapper function for the non virtual method : 'Application::DeviceClass.OnGetCurrentParameter()' */
XInt32 ApplicationDeviceClass__OnGetCurrentParameter( void* _this );

/* The following define announces the presence of the method Application::DeviceClass.OnGetCurrentParameter(). */
#define _ApplicationDeviceClass__OnGetCurrentParameter_

#ifdef __cplusplus
  }
#endif

#endif /* _ApplicationDeviceClass_H */

/* Embedded Wizard */
