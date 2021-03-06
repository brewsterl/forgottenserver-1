/**
 * The Forgotten Server - a server application for the MMORPG Tibia
 * Copyright (C) 2013  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __OTSERV_DEFINITIONS_H__
#define __OTSERV_DEFINITIONS_H__

#define STATUS_SERVER_NAME "The Forgotten Server"
#define STATUS_SERVER_VERSION "1.0"
#define STATUS_SERVER_DEVELOPERS "Mark Samman"

#if defined(__PROTOCOL_77__)
#define __PROTOCOL_76__
#define ITEMS_PATH std::string("77")
#define OTB_VERSION 2
#define CLIENT_VERSION_MIN 770
#define CLIENT_VERSION_MAX 772
#define CLIENT_VERSION_STR "7.7x"
#define STATUS_SERVER_PROTOCOL "7.7x"
#elif defined(__PROTOCOL_76__)
#define ITEMS_PATH std::string("76")
#define OTB_VERSION 2
#define CLIENT_VERSION_MIN 760
#define CLIENT_VERSION_MAX 760
#define CLIENT_VERSION_STR "7.60"
#define STATUS_SERVER_PROTOCOL "7.60"
#else
#define ITEMS_PATH std::string("74")
#define OTB_VERSION 1
#define CLIENT_VERSION_MIN 740
#define CLIENT_VERSION_MAX 740
#define CLIENT_VERSION_STR "7.40"
#define STATUS_SERVER_PROTOCOL "7.40"
#endif

#ifndef __FUNCTION__
#define	__FUNCTION__ __func__
#endif

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <cassert>
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WIN32_LEAN_AND_MEAN

#ifdef _MSC_VER
#ifdef NDEBUG
#define _SECURE_SCL 0
#define HAS_ITERATOR_DEBUGGING 0
#endif

#pragma warning(disable:4100) // unused parameters
#pragma warning(disable:4127) // conditional expression is constant
#pragma warning(disable:4244) // 'argument' : conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4250) // 'class1' : inherits 'class2::member' via dominance
#pragma warning(disable:4267) // 'var' : conversion from 'size_t' to 'type', possible loss of data
#pragma warning(disable:4351) // new behavior: elements of array will be default initialized
#endif

#define strcasecmp _stricmp
#define strncasecmp _strnicmp

//Windows 2000	0x0500
//Windows XP	0x0501
//Windows 2003	0x0502
//Windows Vista	0x0600
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0501
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef std::vector< std::pair<uint32_t, uint32_t> > IPList;

inline int64_t OTSYS_TIME()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

#endif
