/***************************************************************************
 *   Copyright (C) 2013 by Star Marine <starmarine10@yahoo.com>            *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _NETWORK_PROTOCOL_H_
#define _NETWORK_PROTOCOL_H_

// Basic messages

/*
 * Sent by the client to identify itself
 */
#define HMM2_CLIENT_INFO        1

/*
 * Sent by the server to identify itself
 */
#define HMM2_SERVER_INFO        2

/*
 * Sent by the server to provide new server address
 */
#define HMM2_REDIRECT       3

/*
 * Sent by the client to login to the server
 */
#define HMM2_ACCESS_REQUEST 4

/*
 * Sent by client to create a new account
 */
#define HMM2_REGISTER_REQUEST   5

/*
 * Sent by client to change the password
 */
#define HMM2_CHANGEPWD_REQUEST  6

/*
 * Sent by the server to reject client's credentials
 */
#define HMM2_ACCESS_REJECT  7

/*
 * Sent by the server to allow the client in
 */
#define HMM2_ACCESS_ACCEPT  8

/*
 * Sent by the client to request an upgrade
 */
#define HMM2_UPGRADE_REQUEST    9

/*
 * Sent by the server to initiate an upgrade
 */
#define HMM2_UPGRADE_RESPONSE   10

/*
 * Fetch profile from server
 */
#define HMM2_GET_PROFILE        11

/*
 * Change profile details on the server
 */
#define HMM2_SET_PROFILE        12

/*
 * Sent by the server to accept profile changes
 */
#define HMM2_PROFILE_RESPONSE   13

/*
 * Sent by the server to deny profile changes
 */
#define HMM2_PROFILE_REJECT 14

/*
 * Sent by the server to deny client's identification
 */
#define HMM2_CLIENT_UNKNOWN 20

/*
 * Sent by the server to propose resourses upgrade
 */
#define HMM2_UPGRADE_RECOMMENDED    21

/*
 * Sent by the server to insist on resourses upgrade
 */
#define HMM2_UPGRADE_REQUIRED   22
#define HMM2_UNUSED_23  23

// Chat messages
#define HMM2_JOINCHAT_REQUEST    20
#define HMM2_JOINCHAT_RESPONSE   21
#define HMM2_JOINCHAT_REJECT 22
#define HMM2_CHATLIST_REQUEST    23
#define HMM2_CHATLIST_RESPONSE   24
#define HMM2_CHATLIST_REJECT 25
#define HMM2_MESSAGE        26

// Game messages
#define HMM2_CREATEGAME_REQUEST 30
#define HMM2_JOINGAME_REQUEST   31
#define HMM2_CLOSE_GAME     32
#define HMM2_START_GAME     33
#define HMM2_GAME_REPORT        34

#define HMM2_GAME_SETARMY       110
#define HMM2_GAME_SETARMY_NOTIFY        111
#define HMM2_GAME_SETCOLORS     112

#define HMM2_CREATE_GAME_RESPONSE   35
#define HMM2_CREATE_GAME_REJECT 36
#define HMM2_JOIN_GAME_RESPONSE 37
#define HMM2_JOIN_GAME_REJECT   38
#define HMM2_START_GAME_RESPONSE    39
#define HMM2_START_GAME_REJECT  40
#define HMM2_START_GAME_NOTIFY  46
#define HMM2_GAME_REPORT_RESPONSE   41
#define HMM2_CLOSE_GAME_RESPONSE    42

#define HMM2_GAMELIST_REQUEST   43
#define HMM2_GAMELIST_RESPONSE  44
#define HMM2_GAMELIST_REJECT    45

// Turn messages

/*
 * Sent by the server in order to indicate turn start to the corresponding client
 * The client must focus specified troop and wait for user input
 */
#define HMM2_TURN_START         50

/*
 * Sent by the client in order to submit an action to the server.
 */
#define HMM2_TURN_SUBMIT        51

/*
 * Sent by the server in order to announce an action to the client.
 * The client must perform specified action(s)
 */
#define HMM2_TURN_ACTION        52

/*
 * Sent by the client to the server in order to confirm that
 * the action has been performed.
 */
#define HMM2_TURN_COMPLETED        53

// Tournament table messages
#define HMM2_TTABLE_REQUEST 60
#define HMM2_TTABLE_RESPONSE    61
#define HMM2_TTABLE_REJECT  62

// Auxillary messages
#define HMM2_DISCONNECT_REQUEST 100
#define HMM2_PING_REQUEST       101
#define HMM2_PING_RESPONSE  102

// Attributes
#define HMM2_CLIENT_TAG     10
#define HMM2_CLIENT_VERSION 11
#define HMM2_CLIENT_CDKEY       12
#define HMM2_CLIENT_OSNAME  13
#define HMM2_CLIENT_OSVERSION   14
#define HMM2_CLIENT_SOFTNAME    15
#define HMM2_CLIENT_SOFTVERSION 16

#define HMM2_SERVER_OSNAME  20
#define HMM2_SERVER_OSVERSION   21
#define HMM2_SERVER_SOFTNAME    22
#define HMM2_SERVER_SOFTVERSION 23
#define HMM2_SERVER_NAME        24

#define HMM2_USERNAME       31
#define HMM2_PASSWORD       32
#define HMM2_REJECT_REASON  33
#define HMM2_PROFILE_KEYS       34
#define HMM2_PROFILE_VALUES 35
#define HMM2_NEW_PASSWORD       36
#define HMM2_AUTHENTICATOR  37

#define HMM2_CHANNEL_NAME       40
#define HMM2_CHANNEL_TOPIC  41
#define HMM2_CHANNEL_LIST       42
#define HMM2_CHANNEL_PASSWORD   43
#define HMM2_CHANNEL_LIST_PAGE  44

#define HMM2_MESSAGE_TYPE       50
#define HMM2_MESSAGE_TEXT       51
#define HMM2_MESSAGE_SOURCE 52
#define HMM2_MESSAGE_FLAGS  53

#define HMM2_GAME_NAME      60
#define HMM2_GAME_PASSWORD  61
#define HMM2_GAME_TYPE      62
#define HMM2_GAME_VERSION       63
#define HMM2_GAME_ADDR      64
#define HMM2_GAME_PORT      65
#define HMM2_GAME_MAXPLAYERS    66
#define HMM2_GAME_MAXLEVEL  67
#define HMM2_GAME_LIST      68

#define HMM2_GAME_HERO  69
#define HMM2_GAME_COLOR 70
#define HMM2_GAME_PRIMARY_SKILLS 71
#define HMM2_GAME_TROOPS 73
#define HMM2_GAME_COUNTS 74
#define HMM2_GAME_ARTIFACTS 75
#define HMM2_GAME_SECONDARY_SKILLS 76
#define HMM2_GAME_SECONDARY_SKILL_LEVELS 77

#define HMM2_GAME_PLAYER_YOU    0
#define HMM2_GAME_PLAYER1_COLOR 1
#define HMM2_GAME_PLAYER2_COLOR 2

/*
 * Used in HMM2_START_GAME_NOTIFY
 */
#define HMM2_GAME_SECONDS_TO_START    78

// Turn attributes
#define HMM2_TURN_COLOR             0
#define HMM2_TURN_UID               1
#define HMM2_TURN_ACTIONS           2
#define HMM2_TURN_NUMBER            3

#define HMM2_TTABLE_ID      80
#define HMM2_TTABLE_PAGE        81
#define HMM2_TTABLE_DATA        82
#define HMM2_PROFILE_LOCATION   90
#define HMM2_PROFILE_DESCRIPTION    91
#define HMM2_PROFILE_AGE        92
#define HMM2_PROFILE_GENDER 93
#define HMM2_PROFILE_HEROTYPE   94
#define HMM2_PROFILE_HERO       95

#define HMM2_REDIRECT_HOST  100
#define HMM2_REDIRECT_PORT  101

// Values
// Reject-Reason
#define HMM2_LOGIN_FAILED       1
#define HMM2_USER_EXISTS        2
#define HMM2_PWD_NOT_MATCH  3

#define HMM2_CHANNEL_FULL       4
#define HMM2_CHANNEL_RESTRICTED 5
#define HMM2_CHANNEL_UNKNOWN    6

// Value
// Game-Type
#define HMM2_NORMAL     1
#define HMM2_TOURNAMENT     2

#endif
