/*------------------------------------------------------------------------*/
/*                                                                        */
/*  T120 implementation utility head file                                 */
/*  URL.H                                                                 */
/*                                                                        */
/*  History                                                               */
/*      03/05/2001     created by Jack Feng                               */
/*                                                                        */
/*  Copyright (C) 2002 by WebEx Communications Inc.                       */
/*  All rights reserved                                                   */
/*                                                                        */
/*------------------------------------------------------------------------*/

#ifndef __URL_H__
#define	__URL_H__

#include "platform.h"

typedef int32 URL_Error;

#define URL_SUCCESS                   0
#define URL_ERROR_BASE                7000
#define URL_ERROR_INVALID_PARAMETER   (URL_ERROR_BASE + 1)
#define URL_ERROR_LOW_MOMERY          (URL_ERROR_BASE + 2)
#define URL_ERROR_UNSPECIFIED         (URL_ERROR_BASE + 3)

extern "C"
{
	typedef void* URL_Handle;

	/*
	* URL call back function entry
	*/
	typedef URL_Error (*URL_Call_Back)(URL_Handle url_h, int32 result, 
		uint8* content, uint16 content_length);

    URL_Error URL_initialize(void* pGpcInterface, void* pGpcCookie);
    URL_Error URL_cleanup();

	/**
	 * Url get request.
	 *
	 * Input :
	 *     url : url need to get
	 *     secure : is secure url
	 *     call_back_entry: call back entry when this request finished
     *
	 * Output :
	 *     url_h : URL handle
	 *
	 * return : 0 - succeed
	 *          other - error code
	 */
	URL_Error URL_get(char* url, boolean secure, 
		URL_Call_Back call_back_entry, URL_Handle* url_h);

	/**
	 * Cancel Url.
	 *
	 * Input :
	 *     url_h : url handle, which need to ba canceled
     *
	 * return : 0 - succeed
	 *          other - error code
	 */
	URL_Error URL_cancel(URL_Handle url_h);

	/**
	 * Heart beat for URL module.
	 *
	 * Input :
	 *     none
     *
	 * return : 
	 *          none
	 */
	void URL_heartbeat();
}

#endif // __URL_H__
