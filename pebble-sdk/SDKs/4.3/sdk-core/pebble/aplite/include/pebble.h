#pragma once

#include "pebble_fonts.h"
#include "message_keys.auto.h"
#include "src/resource_ids.auto.h"

#define PBL_APP_INFO(...) _Pragma("message \"\n\n \
  *** PBL_APP_INFO has been replaced with appinfo.json\n \
  Try updating your project with `pebble convert-project`\n \
  Visit our developer guides to learn more about appinfo.json:\n \
  http://developer.getpebble.com/guides/pebble-apps/\n \""); \
  _Pragma("GCC error \"PBL_APP_INFO has been replaced with appinfo.json\"");

#define PBL_APP_INFO_SIMPLE PBL_APP_INFO

#include <locale.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "pebble_warn_unsupported_functions.h"
#include "pebble_sdk_version.h"

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

//! Calculate the length of an array, based on the size of the element type.
//! @param array The array to be evaluated.
//! @return The length of the array.
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))

struct tm;
typedef struct tm tm;

//! Determine whether a variable is signed or not.
//! @param var The variable to evaluate.
//! @return true if the variable is signed.
#define IS_SIGNED(var) (__builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned char), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned short), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned int), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned long), false, \
  __builtin_choose_expr( \
  __builtin_types_compatible_p(__typeof__(var), unsigned long long), false, true))))) \
)

//! @addtogroup UI
//! @{

//! @addtogroup Clicks
//! \brief Handling button click interactions
//!
//! Each Pebble window handles Pebble's buttons while it is displayed. Raw button down and button
//! up events are transformed into click events that can be transferred to your app:
//!
//! * Single-click. Detects a single click, that is, a button down event followed by a button up event.
//! It also offers hold-to-repeat functionality (repeated click).
//! * Multi-click. Detects double-clicking, triple-clicking and other arbitrary click counts.
//! It can fire its event handler on all of the matched clicks, or just the last.
//! * Long-click. Detects long clicks, that is, press-and-hold.
//! * Raw. Simply forwards the raw button events. It is provided as a way to use both the higher level
//! "clicks" processing and the raw button events at the same time.
//!
//! To receive click events when a window is displayed, you must register a \ref ClickConfigProvider for
//! this window with \ref window_set_click_config_provider(). Your \ref ClickConfigProvider will be called every time
//! the window becomes visible with one context argument. By default this context is a pointer to the window but you can
//! change this with \ref window_set_click_config_provider_with_context().
//!
//! In your \ref ClickConfigProvider you call the \ref window_single_click_subscribe(), \ref window_single_repeating_click_subscribe(),
//! \ref window_multi_click_subscribe(), \ref window_long_click_subscribe() and \ref window_raw_click_subscribe() functions to register
//! a handler for each event you wish to receive.
//!
//! For convenience, click handlers are provided with a \ref ClickRecognizerRef and a user-specified context.
//!
//! The \ref ClickRecognizerRef can be used in combination with \ref click_number_of_clicks_counted(), \ref
//! click_recognizer_get_button_id() and \ref click_recognizer_is_repeating() to get more information about the click. This is
//! useful if you want different buttons or event types to share the same handler.
//!
//! The user-specified context is the context of your \ref ClickConfigProvider (see above). By default it points to the window.
//! You can override it for all handlers with \ref window_set_click_config_provider_with_context() or for a specific button with \ref
//! window_set_click_context().
//!
//! <h3>User interaction in watchfaces</h3>
//! Watchfaces cannot use the buttons to interact with the user. Instead, you can use the \ref AccelerometerService.
//!
//! <h3>About the Back button</h3>
//! By default, the Back button will always pop to the previous window on the \ref WindowStack (and leave the app if the current
//! window is the only window). You can override the default back button behavior with \ref window_single_click_subscribe() and
//! \ref window_multi_click_subscribe() but you cannot set a repeating, long or raw click handler on the back button because a long press
//! will always terminate the app and return to the main menu.
//!
//! <h3>Usage example</h3>
//! First associate a click config provider callback with your window:
//! \code{.c}
//! void app_init(void) {
//!   ...
//!   window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);
//!   ...
//! }
//! \endcode
//! Then in the callback, you set your desired configuration for each button:
//! \code{.c}
//! void config_provider(Window *window) {
//!  // single click / repeat-on-hold config:
//!   window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
//!   window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 1000, select_single_click_handler);
//!
//!   // multi click config:
//!   window_multi_click_subscribe(BUTTON_ID_SELECT, 2, 10, 0, true, select_multi_click_handler);
//!
//!   // long click config:
//!   window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click_handler, select_long_click_release_handler);
//! }
//! \endcode
//! Now you implement the handlers for each click you've subscribed to and set up:
//! \code{.c}
//! void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
//!   ... called on single click ...
//!   Window *window = (Window *)context;
//! }
//! void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
//!   ... called on single click, and every 1000ms of being held ...
//!   Window *window = (Window *)context;
//! }
//!
//! void select_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
//!   ... called for multi-clicks ...
//!   Window *window = (Window *)context;
//!   const uint16_t count = click_number_of_clicks_counted(recognizer);
//! }
//!
//! void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
//!   ... called on long click start ...
//!   Window *window = (Window *)context;
//! }
//!
//! void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
//!   ... called when long click is released ...
//!   Window *window = (Window *)context;
//! }
//! \endcode
//!
//! <h3>See also</h3>
//! Refer to the \htmlinclude UiFramework.html (chapter "Clicks") for a conceptual
//! overview of clicks and relevant code examples.
//!
//! @{

//! Button ID values
//! @see \ref click_recognizer_get_button_id()
typedef enum {
  //! Back button
  BUTTON_ID_BACK = 0,
  //! Up button
  BUTTON_ID_UP,
  //! Select (middle) button
  BUTTON_ID_SELECT,
  //! Down button
  BUTTON_ID_DOWN,
  //! Total number of buttons
  NUM_BUTTONS
} ButtonId;

//! @} // group Clicks

//! @} // group UI

//! @addtogroup Foundation
//! @{

//! @addtogroup Internationalization
//! \brief Internationalization & Localization APIs
//!
//! @{

//! Get the ISO locale name for the language currently set on the watch
//! @return A string containing the ISO locale name (e.g. "fr", "en_US", ...)
//! @note It is possible for the locale to change while your app is running.
//! And thus, two calls to i18n_get_system_locale may return different values.
const char *i18n_get_system_locale(void);

//! @} // group Internationalization

//! @addtogroup WatchInfo
//! \brief Provides information about the watch itself.
//!
//! This API provides access to information such as the watch model, watch color
//! and watch firmware version.
//! @{

//! The different watch models.
typedef enum {
  WATCH_INFO_MODEL_UNKNOWN, //!< Unknown model
  WATCH_INFO_MODEL_PEBBLE_ORIGINAL, //!< Original Pebble
  WATCH_INFO_MODEL_PEBBLE_STEEL, //!< Pebble Steel
  WATCH_INFO_MODEL_PEBBLE_TIME, //!< Pebble Time
  WATCH_INFO_MODEL_PEBBLE_TIME_STEEL, //!< Pebble Time Steel
  WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_14, //!< Pebble Time Round, 14mm lug size
  WATCH_INFO_MODEL_PEBBLE_TIME_ROUND_20, //!< Pebble Time Round, 20mm lug size
  WATCH_INFO_MODEL_PEBBLE_2_HR, //!< Pebble 2 HR
  WATCH_INFO_MODEL_PEBBLE_2_SE, //!< Pebble 2 SE
  WATCH_INFO_MODEL_PEBBLE_TIME_2, //!< Pebble Time 2

  WATCH_INFO_MODEL__MAX
} WatchInfoModel;

//! The different watch colors.
typedef enum {
  WATCH_INFO_COLOR_UNKNOWN = 0, //!< Unknown color
  WATCH_INFO_COLOR_BLACK = 1, //!< Black
  WATCH_INFO_COLOR_WHITE = 2, //!< White
  WATCH_INFO_COLOR_RED = 3, //!< Red
  WATCH_INFO_COLOR_ORANGE = 4, //!< Orange
  WATCH_INFO_COLOR_GRAY = 5, //!< Gray

  WATCH_INFO_COLOR_STAINLESS_STEEL = 6, //!< Stainless Steel
  WATCH_INFO_COLOR_MATTE_BLACK = 7, //!< Matte Black

  WATCH_INFO_COLOR_BLUE = 8, //!< Blue
  WATCH_INFO_COLOR_GREEN = 9, //!< Green
  WATCH_INFO_COLOR_PINK = 10, //!< Pink

  WATCH_INFO_COLOR_TIME_WHITE = 11, //!< Time White
  WATCH_INFO_COLOR_TIME_BLACK = 12, //!< Time Black
  WATCH_INFO_COLOR_TIME_RED = 13, //!< Time Red

  WATCH_INFO_COLOR_TIME_STEEL_SILVER = 14, //!< Time Steel Silver
  WATCH_INFO_COLOR_TIME_STEEL_BLACK = 15, //!< Time Steel Black
  WATCH_INFO_COLOR_TIME_STEEL_GOLD = 16, //!< Time Steel Gold

  WATCH_INFO_COLOR_TIME_ROUND_SILVER_14 = 17, //!< Time Round 14mm lug size, Silver
  WATCH_INFO_COLOR_TIME_ROUND_BLACK_14 = 18, //!< Time Round 14mm lug size, Black
  WATCH_INFO_COLOR_TIME_ROUND_SILVER_20 = 19, //!< Time Round 20mm lug size, Silver
  WATCH_INFO_COLOR_TIME_ROUND_BLACK_20 = 20, //!< Time Round 20mm lug size, Black
  WATCH_INFO_COLOR_TIME_ROUND_ROSE_GOLD_14 = 21, //!< Time Round 14mm lug size, Rose Gold

  WATCH_INFO_COLOR_PEBBLE_2_HR_BLACK = 25, //!< Pebble 2 HR, Black / Charcoal
  WATCH_INFO_COLOR_PEBBLE_2_HR_LIME = 27, //!< Pebble 2 HR, Charcoal / Sorbet Green
  WATCH_INFO_COLOR_PEBBLE_2_HR_FLAME = 28, //!< Pebble 2 HR, Charcoal / Red
  WATCH_INFO_COLOR_PEBBLE_2_HR_WHITE = 29, //!< Pebble 2 HR, White / Gray
  WATCH_INFO_COLOR_PEBBLE_2_HR_AQUA = 30, //!< Pebble 2 HR, White / Turquoise

  WATCH_INFO_COLOR_PEBBLE_2_SE_BLACK = 24, //!< Pebble 2 SE, Black / Charcoal
  WATCH_INFO_COLOR_PEBBLE_2_SE_WHITE = 26, //!< Pebble 2 SE, White / Gray

  WATCH_INFO_COLOR_PEBBLE_TIME_2_BLACK = 31, //!< Pebble Time 2, Black
  WATCH_INFO_COLOR_PEBBLE_TIME_2_SILVER = 32, //!< Pebble Time 2, Silver
  WATCH_INFO_COLOR_PEBBLE_TIME_2_GOLD = 33, //!< Pebble Time 2, Gold

  WATCH_INFO_COLOR__MAX
} WatchInfoColor;

//! Data structure containing the version of the firmware running on the watch.
//! The version of the firmware has the form X.[X.[X]]. If a version number is not present it will be 0.
//! For example: the version numbers of 2.4.1 are 2, 4, and 1. The version numbers of 2.4 are 2, 4, and 0.
typedef struct {
  uint8_t major; //!< Major version number
  uint8_t minor; //!< Minor version number
  uint8_t patch; //!< Patch version number
} WatchInfoVersion;

//! Provides the model of the watch.
//! @return {@link WatchInfoModel} representing the model of the watch.
WatchInfoModel watch_info_get_model(void);

//! Provides the version of the firmware running on the watch.
//! @return {@link WatchInfoVersion} representing the version of the firmware running on the watch.
WatchInfoVersion watch_info_get_firmware_version(void);

//! Provides the color of the watch.
//! @return {@link WatchInfoColor} representing the color of the watch.
WatchInfoColor watch_info_get_color(void);

//! @} // group WatchInfo

//! @addtogroup Math
//! @{

//! The largest value that can result from a call to \ref sin_lookup or \ref cos_lookup.
//! For a code example, see the detailed description at the top of this chapter: \ref Math
#define TRIG_MAX_RATIO 0xffff

//! Angle value that corresponds to 360 degrees or 2 PI radians
//! @see \ref sin_lookup
//! @see \ref cos_lookup
#define TRIG_MAX_ANGLE 0x10000

//! Converts from a fixed point value representation to the equivalent value in degrees
//! @see DEG_TO_TRIGANGLE
//! @see TRIG_MAX_ANGLE
#define TRIGANGLE_TO_DEG(trig_angle) (((trig_angle) * 360) / TRIG_MAX_ANGLE)

//! Converts from an angle in degrees to the equivalent fixed point value representation
//! @see TRIGANGLE_TO_DEG
//! @see TRIG_MAX_ANGLE
#define DEG_TO_TRIGANGLE(angle) (((angle) * TRIG_MAX_ANGLE) / 360)

//! Look-up the sine of the given angle from a pre-computed table.
//! @param angle The angle for which to compute the cosine.
//! The angle value is scaled linearly, such that a value of 0x10000 corresponds to 360 degrees or 2 PI radians.
int32_t sin_lookup(int32_t angle);

//! Look-up the cosine of the given angle from a pre-computed table.
//! This is equivalent to calling `sin_lookup(angle + TRIG_MAX_ANGLE / 4)`.
//! @param angle The angle for which to compute the cosine.
//! The angle value is scaled linearly, such that a value of 0x10000 corresponds to 360 degrees or 2 PI radians.
int32_t cos_lookup(int32_t angle);

//! Look-up the arctangent of a given x, y pair
//! The angle value is scaled linearly, such that a value of 0x10000 corresponds to 360 degrees or 2 PI radians.
int32_t atan2_lookup(int16_t y, int16_t x);

//! @} // group Math

//! @addtogroup WallTime Wall Time
//!   \brief Functions, data structures and other things related to wall clock time.
//!
//! This module contains utilities to get the current time and create strings with formatted
//! dates and times.
//! @{

//! Weekday values
typedef enum {
  TODAY = 0,  //!< Today
  SUNDAY,     //!< Sunday
  MONDAY,     //!< Monday
  TUESDAY,    //!< Tuesday
  WEDNESDAY,  //!< Wednesday
  THURSDAY,   //!< Thursday
  FRIDAY,     //!< Friday
  SATURDAY,   //!< Saturday
} WeekDay;

//! Copies a time string into the buffer, formatted according to the user's time display preferences (such as 12h/24h
//! time).
//! Example results: "7:30" or "15:00".
//! @note AM/PM are also outputted with the time if the user's preference is 12h time.
//! @param[out] buffer A pointer to the buffer to copy the time string into
//! @param size The maximum size of buffer
void clock_copy_time_string(char *buffer, uint8_t size);

//! Gets the user's 12/24h clock style preference.
//! @return `true` if the user prefers 24h-style time display or `false` if the
//! user prefers 12h-style time display.
bool clock_is_24h_style(void);

//! Converts a (day, hour, minute) specification to a UTC timestamp occurring in the future
//! Always returns a timestamp for the next occurring instance,
//! example: specifying TODAY@14:30 when it is 14:40 will return a timestamp for 7 days from
//! now at 14:30
//! @note This function does not support Daylight Saving Time (DST) changes, events scheduled
//! during a DST change will be off by an hour.
//! @param day WeekDay day of week including support for specifying TODAY
//! @param hour hour specified in 24-hour format [0-23]
//! @param minute minute [0-59]
time_t clock_to_timestamp(WeekDay day, int hour, int minute);

//! Checks if timezone is currently set, otherwise gmtime == localtime.
//! @return `true` if timezone has been set, false otherwise
bool clock_is_timezone_set(void);

//! The maximum length for a timezone full name (e.g. America/Chicago)
#define TIMEZONE_NAME_LENGTH 32

//! If timezone is set, copies the current timezone long name (e.g. America/Chicago)
//! to user-provided buffer.
//! @param timezone A pointer to the buffer to copy the timezone long name into
//! @param buffer_size Size of the allocated buffer to copy the timezone long name into
//! @note timezone buffer should be at least TIMEZONE_NAME_LENGTH bytes
void clock_get_timezone(char *timezone, const size_t buffer_size);

//! @} // group WallTime

//! @addtogroup Platform
//! @{

typedef enum PlatformType {
  PlatformTypeAplite,
  PlatformTypeBasalt,
  PlatformTypeChalk,
  PlatformTypeDiorite,
  PlatformTypeEmery,
} PlatformType;

#define PBL_PLATFORM_TYPE_CURRENT PlatformTypeAplite

#define PBL_PLATFORM_SWITCH_DEFAULT(PLAT, DEFAULT, APLITE, BASALT, CHALK, DIORITE, EMERY) (APLITE)

#define PBL_PLATFORM_SWITCH(PLAT, APLITE, BASALT, CHALK, DIORITE, EMERY) (APLITE)

//! @} // group Platform

//! @addtogroup EventService
//! @{

//! @addtogroup ConnectionService
//! \brief Determine what the Pebble watch is connected to
//!
//! The ConnectionService allows your app to learn about the apps the Pebble
//! watch is connected to. You can ask the system for this information at a
//! given time or you can register to receive events every time connection or
//! disconnection events occur.
//!
//! It allows you to determine whether the watch is connected to the Pebble
//! mobile app by subscribing to the pebble_app_connection_handler or by calling
//! the connection_service_peek_pebble_app_connection function.  Note that when
//! the Pebble app is connected, you can assume PebbleKit JS apps will also be
//! running correctly.
//!
//! The service also allows you to determine if the Pebble watch can establish
//! a connection to a PebbleKit companion app by subscribing to the
//! pebblekit_connection_handler or by calling the
//! connection_service_peek_pebblekit_connection function.  Today, due to
//! architectural differences between iOS and Android, this will return true
//! for Android anytime a connection with the Pebble mobile app is established
//! (since PebbleKit messages are routed through the Android app). For iOS,
//! this will return true when any PebbleKit companion app has established a
//! connection with the Pebble watch (since companion app messages are routed
//! directly to the watch)
//!
//! @{

typedef void (*ConnectionHandler)(bool connected);

typedef struct {
  //! callback to be executed when the connection state between the watch and
  //! the phone app has changed. Note, if the phone App is connected, PebbleKit JS apps
  //! will also be working correctly
  ConnectionHandler pebble_app_connection_handler;
  //! ID for callback to be executed on PebbleKit connection event
  ConnectionHandler pebblekit_connection_handler;
} ConnectionHandlers;

//! Query the bluetooth connection service for the current Pebble app connection status
//! @return true if the Pebble app is connected, false otherwise
bool connection_service_peek_pebble_app_connection(void);

//! Query the bluetooth connection service for the current PebbleKit connection status
//! @return true if a PebbleKit companion app is connected, false otherwise
bool connection_service_peek_pebblekit_connection(void);

//! Subscribe to the connection event service. Once subscribed, the appropriate
//! handler gets called based on the type of connection event and user provided
//! handlers
//! @param ConnectionHandlers A struct populated with the handlers to
//! be called when the specified connection event occurs. If a given handler is
//! NULL, no function will be called.
void connection_service_subscribe(ConnectionHandlers conn_handlers);

//! Unsubscribe from the bluetooth event service. Once unsubscribed, the previously registered
//! handler will no longer be called.
void connection_service_unsubscribe(void);

//! @deprecated Backwards compatibility typedef for ConnectionHandler. New code
//! should use ConnectionHandler directly.  This will be removed in a future
//! version of the Pebble SDK.
typedef ConnectionHandler BluetoothConnectionHandler;

//! @deprecated Backward compatibility function for
//! connection_service_peek_pebble_app_connection.  New code should use
//! connection_service_peek_pebble_app_connection directly. This will be
//! removed in a future version of the Pebble SDK
bool bluetooth_connection_service_peek(void);

//! @deprecated Backward compatibility function for
//! connection_service_subscribe.  New code should use
//! connection_service_subscribe directly. This will be removed in a future
//! version of the Pebble SDK
void bluetooth_connection_service_subscribe(ConnectionHandler handler);

//! @deprecated Backward compatibility function for
//! connection_service_unsubscribe.  New code should use
//! connection_service_unsubscribe directly. This will be removed in a future
//! version of the Pebble SDK
void bluetooth_connection_service_unsubscribe(void);

//! @} // group ConnectionService

//! @addtogroup AppFocusService
//!
//!
//! \brief Handling app focus
//! The AppFocusService allows developers to be notified when their apps become visible on the
//! screen. Common reasons your app may be running but  not be on screen are: it's still in the
//! middle of launching and being revealed by a system animation, or it is being covered by a system
//! window such as a notification. This service is useful for apps that require a high degree of
//! user interactivity, like a game where you'll want to pause when a notification covers your app
//! window. It can be also used for apps that want to sync up an intro animation to the end of the
//! system animation that occurs before your app is visible.
//!
//! @{

//! Callback type for focus events
//! @param in_focus True if the app is gaining focus, false otherwise.
typedef void (*AppFocusHandler)(bool in_focus);

//! There are two different focus events which take place when transitioning to and from an app
//! being in focus. Below is an example of when these events will occur:
//! 1) The app is launched. Once the system animation to the app has completed and the app is
//! completely in focus, the did_focus handler is called with in_focus set to true.
//! 2) A notification comes in and the animation to show the notification starts. The will_focus
//! handler is called with in_focus set to false.
//! 3) The animation completes and the notification is in focus, with the app being completely
//! covered. The did_focus hander is called with in_focus set to false.
//! 4) The notification is dismissed and the animation to return to the app starts. The will_focus
//! handler is called with in_focus set to true.
//! 5) The animation completes and the app is in focus. The did_focus handler is called with
//! in_focus set to true.
typedef struct {
  //! Handler which will be called right before an app will lose or gain focus.
  //! @note This will be called with in_focus set to true when a window which is covering the app is
  //! about to close and return focus to the app.
  //! @note This will be called with in_focus set to false when a window which will cover the app is
  //! about to open, causing the app to lose focus.
  AppFocusHandler will_focus;
  //! Handler which will be called when an animation finished which has put the app into focus or
  //! taken the app out of focus.
  //! @note This will be called with in_focus set to true when a window which was covering the app
  //! has closed and the app has gained focus.
  //! @note This will be called with in_focus set to false when a window has opened which is now
  //! covering the app, causing the app to lose focus.
  AppFocusHandler did_focus;
} AppFocusHandlers;

//! Subscribe to the focus event service. Once subscribed, the handlers get called every time the
//! app gains or loses focus.
//! @param handler Handlers which will be called on will-focus and did-focus events.
//! @see AppFocusHandlers
void app_focus_service_subscribe_handlers(AppFocusHandlers handlers);

//! Subscribe to the focus event service. Once subscribed, the handler
//! gets called every time the app focus changes.
//! @note Calling this function is equivalent to
//! \code{.c}
//! app_focus_service_subscribe_handlers((AppFocusHandlers){
//!   .will_focus = handler,
//! });
//! \endcode
//! @note Out focus events are triggered when a modal window is about to open and cover the app.
//! @note In focus events are triggered when a modal window which is covering the app is about to
//! close.
//! @param handler A callback to be called on will-focus events.
void app_focus_service_subscribe(AppFocusHandler handler);

//! Unsubscribe from the focus event service. Once unsubscribed, the previously
//! registered handlers will no longer be called.
void app_focus_service_unsubscribe(void);

//! @} // group AppFocusService

//! @addtogroup BatteryStateService
//!
//! \brief Determines when the battery state changes
//!
//! The BatteryStateService API lets you know when the battery state changes, that is,
//! its current charge level, whether it is plugged and charging. It uses the
//! BatteryChargeState structure to describe the current power state of Pebble.
//!
//! Refer to the <a href="https://github.com/pebble-examples/classio-battery-connection">
//! classio-battery-connection</a> example, which demonstrates using the battery state service
//! in a watchface.
//! @{

//! Structure for retrieval of the battery charge state
typedef struct {
  //! A percentage (0-100) of how full the battery is
  uint8_t charge_percent;
  //! True if the battery is currently being charged. False if not.
  bool is_charging;
  //! True if the charger cable is connected. False if not.
  bool is_plugged;
} BatteryChargeState;

//! Callback type for battery state change events
//! @param charge the state of the battery \ref BatteryChargeState
typedef void (*BatteryStateHandler)(BatteryChargeState charge);

//! Subscribe to the battery state event service. Once subscribed, the handler gets called
//! on every battery state change
//! @param handler A callback to be executed on battery state change event
void battery_state_service_subscribe(BatteryStateHandler handler);

//! Unsubscribe from the battery state event service. Once unsubscribed, the previously registered
//! handler will no longer be called.
void battery_state_service_unsubscribe(void);

//! Peek at the last known battery state.
//! @return a \ref BatteryChargeState containing the last known data
BatteryChargeState battery_state_service_peek(void);

//! @} // group BatteryStateService

//! @addtogroup AccelerometerService
//!
//! \brief Using the Pebble accelerometer
//!
//! The AccelerometerService enables the Pebble accelerometer to detect taps,
//! perform measures at a given frequency, and transmit samples in batches to save CPU time
//! and processing.
//!
//! For available code samples, see the
//! <a href="https://github.com/pebble-examples/feature-accel-discs/">feature-accel-discs</a>
//! example app.
//! @{

//! A single accelerometer sample for all three axes including timestamp and
//! vibration rumble status.
typedef struct __attribute__((__packed__)) AccelData {
  //! acceleration along the x axis
  int16_t x;
  //! acceleration along the y axis
  int16_t y;
  //! acceleration along the z axis
  int16_t z;

  //! true if the watch vibrated when this sample was collected
  bool did_vibrate;

  //! timestamp, in milliseconds
  uint64_t timestamp;
} AccelData;

//! A single accelerometer sample for all three axes
typedef struct __attribute__((__packed__)) {
  //! acceleration along the x axis
  int16_t x;
  //! acceleration along the y axis
  int16_t y;
  //! acceleration along the z axis
  int16_t z;
} AccelRawData;

//! Enumerated values defining the three accelerometer axes.
typedef enum {
  //! Accelerometer's X axis. The positive direction along the X axis goes
  //! toward the right of the watch.
  ACCEL_AXIS_X = 0,
  //! Accelerometer's Y axis. The positive direction along the Y axis goes
  //! toward the top of the watch.
  ACCEL_AXIS_Y = 1,
  //! Accelerometer's Z axis. The positive direction along the Z axis goes
  //! vertically out of the watchface.
  ACCEL_AXIS_Z = 2,
} AccelAxisType;

//! Callback type for accelerometer data events
//! @param data Pointer to the collected accelerometer samples.
//! @param num_samples the number of samples stored in data.
typedef void (*AccelDataHandler)(AccelData *data, uint32_t num_samples);

//! Callback type for accelerometer raw data events
//! @param data Pointer to the collected accelerometer samples.
//! @param num_samples the number of samples stored in data.
//! @param timestamp the timestamp, in ms, of the first sample.
typedef void (*AccelRawDataHandler)(AccelRawData *data, uint32_t num_samples, uint64_t timestamp);

//! Callback type for accelerometer tap events
//! @param axis the axis on which a tap was registered (x, y, or z)
//! @param direction the direction (-1 or +1) of the tap
typedef void (*AccelTapHandler)(AccelAxisType axis, int32_t direction);

//! Valid accelerometer sampling rates, in Hz
typedef enum {
  //! 10 HZ sampling rate
  ACCEL_SAMPLING_10HZ = 10,
  //! 25 HZ sampling rate [Default]
  ACCEL_SAMPLING_25HZ = 25,
  //! 50 HZ sampling rate
  ACCEL_SAMPLING_50HZ = 50,
  //! 100 HZ sampling rate
  ACCEL_SAMPLING_100HZ = 100,
} AccelSamplingRate;

//! Peek at the last recorded reading.
//! @param[out] data a pointer to a pre-allocated AccelData item
//! @note Cannot be used when subscribed to accelerometer data events.
//! @return -1 if the accel is not running
//! @return -2 if subscribed to accelerometer events.
int accel_service_peek(AccelData *data);

//! Change the accelerometer sampling rate.
//! @param rate The sampling rate in Hz (10Hz, 25Hz, 50Hz, and 100Hz possible)
int accel_service_set_sampling_rate(AccelSamplingRate rate);

//! Change the number of samples buffered between each accelerometer data event
//! @param num_samples the number of samples to buffer, between 0 and 25.
int accel_service_set_samples_per_update(uint32_t num_samples);

//! Subscribe to the accelerometer data event service. Once subscribed, the handler
//! gets called every time there are new accelerometer samples available.
//! @note Cannot use \ref accel_service_peek() when subscribed to accelerometer data events.
//! @param handler A callback to be executed on accelerometer data events
//! @param samples_per_update the number of samples to buffer, between 0 and 25.
void accel_data_service_subscribe(uint32_t samples_per_update, AccelDataHandler handler);

//! Unsubscribe from the accelerometer data event service. Once unsubscribed,
//! the previously registered handler will no longer be called.
void accel_data_service_unsubscribe(void);

//! Subscribe to the accelerometer tap event service. Once subscribed, the handler
//! gets called on every tap event emitted by the accelerometer.
//! @param handler A callback to be executed on tap event
void accel_tap_service_subscribe(AccelTapHandler handler);

//! Unsubscribe from the accelerometer tap event service. Once unsubscribed,
//! the previously registered handler will no longer be called.
void accel_tap_service_unsubscribe(void);

//! Subscribe to the accelerometer raw data event service. Once subscribed, the handler
//! gets called every time there are new accelerometer samples available.
//! @note Cannot use \ref accel_service_peek() when subscribed to accelerometer data events.
//! @param handler A callback to be executed on accelerometer data events
//! @param samples_per_update the number of samples to buffer, between 0 and 25.
void accel_raw_data_service_subscribe(uint32_t samples_per_update, AccelRawDataHandler handler);

//! @} // group AccelerometerService

//! @addtogroup CompassService
//!
//!     \brief The Compass Service combines information from Pebble's accelerometer and
//!     magnetometer to automatically calibrate
//!     the compass and transform the raw magnetic field information into a \ref CompassHeading,
//!     that is an angle to north. It also
//!     provides magnetic north and information about its status and accuracy through the \ref
//!     CompassHeadingData structure. The API is designed to also provide true north in a future
//!     release.
//!
//!     Note that not all platforms have compasses. To check for the presence of a compass at
//!     compile time for the current platform use the `PBL_COMPASS` define.
//!
//!     To learn more about the Compass Service and how to use it, read the
//!     <a href="https://developer.getpebble.com/guides/pebble-apps/sensors/magnetometer/">
//!     Determining Direction</a> guide.
//!
//!     For available code samples, see the
//!     <a href="https://github.com/pebble-examples/feature-compass">feature-compass</a> example.
//!
//! @{

//! Enum describing the current state of the Compass Service calibration
typedef enum {
  //! The Compass Service is unavailable.
  CompassStatusUnavailable = -1,
  //! Compass is calibrating: data is invalid and should not be used
  //! Data will become valid once calibration is complete
  CompassStatusDataInvalid = 0,
  //! Compass is calibrating: the data is valid but the calibration is still being refined
  CompassStatusCalibrating,
  //! Compass data is valid and the calibration has completed
  CompassStatusCalibrated
} CompassStatus;

//! Represents an angle relative to get to a reference direction, e.g. (magnetic) north.
//! The angle value is scaled linearly, such that a value of TRIG_MAX_ANGLE
//! corresponds to 360 degrees or 2 PI radians.
//! Thus, if heading towards north, north is 0, west is TRIG_MAX_ANGLE/4,
//! south is TRIG_MAX_ANGLE/2, and so on.
typedef int32_t CompassHeading;

//! Structure containing a single heading towards magnetic and true north.
typedef struct {
  //! Measured angle that increases counter-clockwise from magnetic north
  //! (use `int clockwise_heading = TRIG_MAX_ANGLE - heading_data.magnetic_heading;`
  //! for example to find your heading clockwise from magnetic north).
  CompassHeading magnetic_heading;
  //! Currently same value as magnetic_heading (reserved for future implementation).
  CompassHeading true_heading;
  //! Indicates the current state of the Compass Service calibration.
  CompassStatus compass_status;
  //! Currently always false (reserved for future implementation).
  bool is_declination_valid;
} CompassHeadingData;

//! Callback type for compass heading events
//! @param heading copy of last recorded heading
typedef void (*CompassHeadingHandler)(CompassHeadingData heading);

//! Set the minimum angular change required to generate new compass heading events.
//! The angular distance is measured relative to the last delivered heading event.
//! Use 0 to be notified of all movements.
//! Negative values and values > TRIG_MAX_ANGLE / 2 are not valid.
//! The default value of this property is TRIG_MAX_ANGLE / 360.
//! @return 0, success.
//! @return Non-Zero, if filter is invalid.
//! @see compass_service_subscribe
int compass_service_set_heading_filter(CompassHeading filter);

//! Subscribe to the compass heading event service. Once subscribed, the handler
//! gets called every time the angular distance relative to the previous value
//! exceeds the configured filter.
//! @param handler A callback to be executed on heading events
//! @see compass_service_set_heading_filter
//! @see compass_service_unsubscribe
void compass_service_subscribe(CompassHeadingHandler handler);

//! Unsubscribe from the compass heading event service. Once unsubscribed,
//! the previously registered handler will no longer be called.
//! @see compass_service_subscribe
void compass_service_unsubscribe(void);

//! Peek at the last recorded reading.
//! @param[out] data a pointer to a pre-allocated CompassHeadingData
//! @return Always returns 0 to indicate success.
int compass_service_peek(CompassHeadingData *data);

//! @} // group CompassService

//! @addtogroup TickTimerService
//! \brief Handling time components
//!
//! The TickTimerService allows your app to be called every time one Time component has changed.
//! This is extremely important for watchfaces. Your app can choose on which time component
//! change a tick should occur. Time components are defined by a \ref TimeUnits enum bitmask.
//! @{

//! Time unit flags that can be used to create a bitmask for use in \ref tick_timer_service_subscribe().
//! This will also be passed to \ref TickHandler.
typedef enum {
  //! Flag to represent the "seconds" time unit
  SECOND_UNIT = 1 << 0,
  //! Flag to represent the "minutes" time unit
  MINUTE_UNIT = 1 << 1,
  //! Flag to represent the "hours" time unit
  HOUR_UNIT = 1 << 2,
  //! Flag to represent the "days" time unit
  DAY_UNIT = 1 << 3,
  //! Flag to represent the "months" time unit
  MONTH_UNIT = 1 << 4,
  //! Flag to represent the "years" time unit
  YEAR_UNIT = 1 << 5
} TimeUnits;

//! Callback type for tick timer events
//! @param tick_time the time at which the tick event was triggered
//! @param units_changed which unit change triggered this tick event
typedef void (*TickHandler)(struct tm *tick_time, TimeUnits units_changed);

//! Subscribe to the tick timer event service. Once subscribed, the handler gets called
//! on every requested unit change.
//! Calling this function multiple times will override the units and handler (i.e., only 
//! the last tick_units and handler passed will be used).
//! @param handler The callback to be executed on tick events
//! @param tick_units a bitmask of all the units that have changed
void tick_timer_service_subscribe(TimeUnits tick_units, TickHandler handler);

//! Unsubscribe from the tick timer event service. Once unsubscribed, the previously registered
//! handler will no longer be called.
void tick_timer_service_unsubscribe(void);

//! @} // group TickTimerService

//! @addtogroup HealthService
//!
//! \brief Get access to health information like step count, sleep totals, etc.
//!
//! The HealthService provides your app access to the step count and sleep activity of the user.
//!
//! @{

//! Health metric values used to retrieve health data.
//! For example, using \ref health_service_sum().
typedef enum {
  //! The number of steps counted.
  HealthMetricStepCount,
  //! The number of seconds spent active (i.e. not resting).
  HealthMetricActiveSeconds,
  //! The distance walked, in meters.
  HealthMetricWalkedDistanceMeters,
  //! The number of seconds spent sleeping.
  HealthMetricSleepSeconds,
  //! The number of sleep seconds in the 'restful' or deep sleep state.
  HealthMetricSleepRestfulSeconds,
  //! The number of kcal (Calories) burned while resting due to resting metabolism.
  HealthMetricRestingKCalories,
  //! The number of kcal (Calories) burned while active.
  HealthMetricActiveKCalories,
  //! The heart rate, in beats per minute.
  HealthMetricHeartRateBPM,
  //! The resting heart rate, in beats per minute.
  HealthMetricRestingHeartRateBPM,
} HealthMetric;

//! Type used as a handle to a registered metric alert (returned by
//! \ref health_service_register_metric_alert)
typedef struct HealthMetricAlert* HealthMetricAlert;

//! Type used to represent HealthMetric values
typedef int32_t HealthValue;

//! Return the sum of a \ref HealthMetric's values over a time range.
//! The `time_start` and `time_end` parameters define the range of time you want the sum for.
//! @note The value returned will be an average since midnight, weighted for the length of the
//! specified time range. This may change in the future.
//! @param metric The metric to query for data.
//! @param time_start UTC time of the earliest data item to incorporate into the sum.
//! @param time_end UTC time of the most recent data item to incorporate into the sum.
//! @return The sum of that metric over the given time range, if available.
HealthValue health_service_sum(HealthMetric metric, time_t time_start, time_t time_end);

//! Convenience wrapper for \ref health_service_sum() that returns the sum for today.
//! @param metric The metric to query for data.
//! @return The sum of that metric's data for today, if available.
HealthValue health_service_sum_today(HealthMetric metric);

//! Convenience function for peeking at the current value of a metric. This is useful for metrics
//! like \ref HealthMetricHeartRateBPM that represent instantaneous values. It is NOT applicable for
//! metrics like \ref HealthMetricStepCount that must be accumulated over time (it will return 0 if
//! passed that type of metric). This call is equivalent to calling
//! `health_service_aggregate_averaged(metric, time(NULL), time(NULL), HealthAggregateAvg,
//! HealthServiceTimeScopeOnce)`
//! @param metric The metric to query.
//! @return The current value of that metric, if available.
#define health_service_peek_current_value(metric) (0)

//! Used by \ref health_service_sum_averaged() to specify how the average is computed.
typedef enum {
  //! No average computed. The result is the same as calling \ref health_service_sum().
  HealthServiceTimeScopeOnce,
  //! Compute average using the same day from each week. For example, every Monday if the passed
  //! in time range falls on a Monday.
  HealthServiceTimeScopeWeekly,
  //! Compute average using either weekdays (Monday to Friday) or weekends (Saturday and Sunday),
  //! depending on which day the passed in time range falls.
  HealthServiceTimeScopeDailyWeekdayOrWeekend,
  //! Compute average across all days of the week.
  HealthServiceTimeScopeDaily,
} HealthServiceTimeScope;

//! Return the average value of a metric's sum over a given time range between `time_start`
//! and `time_end`. Using this call you can specify the time range that you are interested in
//! getting the average for, as well as a `scope` specifier on how to compute that average. For
//! example, if you want to get the average number of steps taken from 12 AM (midnight) to 9 AM
//! across all days you would specify:
//! `time_t time_start = time_start_of_today()`,
//! `time_t time_end = time_start + (9 * SECONDS_PER_HOUR)`,
//! and `HealthServiceTimeScope scope = HealthServiceTimeScopeDaily`.
//! If you want the average number of steps taken on a weekday (Monday to Friday) and today is a
//! Monday (in the local timezone) you would specify: `time_start = time_start_of_today()`,
//! `time_end = time_start + SECONDS_PER_DAY`, and
//! `scope = HealthServiceTimeScopeDailyWeekdayOrWeekend`.
//! @param metric Which \ref HealthMetric to query.
//! @param time_start UTC time of the start of the query interval.
//! @param time_end UTC time of the end of the query interval.
//! @param scope \ref HealthServiceTimeScope value describing how the average should be computed.
//! @return The average of the sum of the given metric over the given time range, if available.
HealthValue health_service_sum_averaged(HealthMetric metric, time_t time_start, time_t time_end,
                                        HealthServiceTimeScope scope);

//! Used by \ref health_service_aggregate_averaged() to specify what type of aggregation to perform.
//! This aggregation is applied to the metric before the average is computed.
typedef enum {
  //! Sum the metric. The result is the same as calling \ref health_service_sum_averaged(). This
  //! operation is only applicable for metrics that accumulate, like HealthMetricStepCount,
  //! HealthMetricActiveSeconds, etc.
  HealthAggregationSum,
  //! Use the average of the metric. This is only applicable for metrics that measure instantaneous
  //! values, like HealthMetricHeartRateBPM
  HealthAggregationAvg,
  //! Use the minimum value of the metric. This is only applicable for metrics that measure
  //! instantaneous values, like HealthMetricHeartRateBPM
  HealthAggregationMin,
  //! Use the maximum value of the metric. This is only applicable for metrics that measure
  //! instantaneous values, like HealthMetricHeartRateBPM
  HealthAggregationMax
} HealthAggregation;

//! Return the value of an aggregated metric over a given time range. This call is more
//! flexible than health_service_sum_averaged because it lets you specify which aggregation function
//! to perform.
//!
//! The aggregation function `aggregation` is applied to the metric `metric` over the given time
//! range `time_start` to `time_end` first, and then an average is computed based on the passed in
//! `scope`.
//!
//! For example, if you want to get the average number of steps taken from 12 AM (midnight) to 9 AM
//! across all days you would specify:
//! `time_t time_start = time_start_of_today();`
//! `time_t time_end = time_start + (9 * SECONDS_PER_HOUR);`
//! `HealthValue value = health_service_aggregate_averaged(HealthMetricStepCount, time_start,
//!    time_end, HealthAggregationSum, HealthServiceTimeScopeDaily);`
//!
//! If you want to compute the average heart rate on Mondays and today is a Monday, you would
//! specify:
//! `time_t time_start = time_start_of_today()`,
//! `time_t time_end = time_start + SECONDS_PER_DAY`,
//! `HealthValue value = health_service_aggregate_averaged(HealthMetricHeartRateBPM, time_start,
//!    time_end, HealthAggregationAvg, HealthServiceTimeScopeWeekly);`
//! To get the average of the minimum heart rate seen on Mondays for example, you would instead
//! pass in `HealthAggregationMin`
//!
//! Certain HealthAggregation operations are only applicable to certain types of metrics. See the
//! notes above on \ref HealthAggregation for details. Use
//! \ref health_service_metric_aggregate_averaged_accessible to check for applicability at run
//! time.
//!
//! @param metric Which \ref HealthMetric to query.
//! @param time_start UTC time of the start of the query interval.
//! @param time_end UTC time of the end of the query interval.
//! @param aggregation the aggregation function to perform on the metric. This operation is
//!   performed across the passed in time range `time_start` to `time_end`.
//! @param scope \ref HealthServiceTimeScope value describing how the average should be computed.
//!  Use `HealthServiceTimeScopeOnce` to not compute an average.
//! @return The average of the aggregation performed on the given metric over the given time range,
//!  if available.
#define health_service_aggregate_averaged(metric, time_start, time_end, aggregation, scope) (0)

//! Expresses a set of \ref HealthActivity values as a bitmask.
typedef uint32_t HealthActivityMask;

//! A mask value representing all available activities
#define HealthActivityMaskAll ((HealthActivityRun << 1) - 1)

//! Health-related activities that can be accessed
//! using \ref health_service_peek_current_activities()
//! and \ref health_service_activities_iterate().
typedef enum {
  //! No special activity.
  HealthActivityNone = 0,
  //! The 'sleeping' activity.
  HealthActivitySleep = 1 << 0,
  //! The 'restful sleeping' activity.
  HealthActivityRestfulSleep = 1 << 1,
  //! The 'walk' activity.
  HealthActivityWalk = 1 << 2,
  //! The 'run' activity.
  HealthActivityRun = 1 << 3,
} HealthActivity;

//! Return a \ref HealthActivityMask containing a set of bits, one set for each
//! activity that is currently active.
//! @return A bitmask with zero or more \ref HealthActivityMask bits set as appropriate.
HealthActivityMask health_service_peek_current_activities(void);

//! Callback used by \ref health_service_activities_iterate().
//! @param activity Which activity the caller is being informed about.
//! @param time_start Start UTC time of the activity.
//! @param time_end End UTC time of the activity.
//! @param context The `context` parameter initially passed
//!     to \ref health_service_activities_iterate().
//! @return `true` if you are interested in more activities, or `false` to stop iterating.
typedef bool (*HealthActivityIteratorCB)(HealthActivity activity,
                                         time_t time_start, time_t time_end,
                                         void *context);

//! Iteration direction, passed to \ref health_service_activities_iterate().
//! When iterating backwards (`HealthIterationDirectionPast`), activities that have a greater value
//! for `time_end` come first.
//! When iterating forward (`HealthIterationDirectionFuture`), activities that have a smaller value
//! for `time_start` come first.
typedef enum {
  //! Iterate into the past.
  HealthIterationDirectionPast,
  //! Iterate into the future.
  HealthIterationDirectionFuture,
} HealthIterationDirection;

//! Iterates backwards or forward within a given time span to list all recorded activities.
//! For example, this can be used to find the last recorded sleep phase or all deep sleep phases in
//! a given time range. Any activity that overlaps with `time_start` and `time_end` will be
//! included, even if the start time starts before `time_start` or end time ends after `time_end`.
//! @param activity_mask A bitmask containing set of activities you are interested in.
//! @param time_start UTC time of the earliest time you are interested in.
//! @param time_end UTC time of the latest time you are interested in.
//! @param direction The direction in which to iterate.
//! @param callback Developer-supplied callback that is called for each activity iterated over.
//! @param context Developer-supplied context pointer that is passed to the callback.
void health_service_activities_iterate(HealthActivityMask activity_mask,
                                       time_t time_start, time_t time_end,
                                       HealthIterationDirection direction,
                                       HealthActivityIteratorCB callback, void *context);

//! Possible values returned by \ref health_service_metric_accessible().
//! The values are used in combination as a bitmask.
//! For example, to check if any data is available for a given request use:
//! bool any_data_available = value & HealthServiceAccessibilityMaskAvailable;
typedef enum {
  //! Return values are available and represent the collected health information.
  HealthServiceAccessibilityMaskAvailable = 1 << 0,
  //! The user hasn't granted permission.
  HealthServiceAccessibilityMaskNoPermission = 1 << 1,
  //! The queried combination of time span and \ref HealthMetric or \ref HealthActivityMask
  //! is currently unsupported.
  HealthServiceAccessibilityMaskNotSupported = 1 << 2,
  //! No samples were recorded for the given time span.
  HealthServiceAccessibilityMaskNotAvailable = 1 << 3,
} HealthServiceAccessibilityMask;

//! Check if a certain combination of metric and time span is accessible by returning a
//! value of \ref HealthServiceAccessibilityMask. Developers should check if the return value is
//! \ref HealthServiceAccessibilityMaskAvailable before calling any other HealthService APIs that
//! involve the given metric.
//! @param metric The metric to query for data.
//! @param time_start Earliest UTC time you are interested in.
//! @param time_end Latest UTC time you are interested in.
//! @return A \ref HealthServiceAccessibilityMask representing the accessible metrics
//! in this time range.
HealthServiceAccessibilityMask health_service_metric_accessible(
    HealthMetric metric, time_t time_start, time_t time_end);

//! Check if a certain combination of metric, time span, and scope is accessible for calculating
//! averaged data by returning a value of \ref HealthServiceAccessibilityMask. Developers should
//! check if the return value is \ref HealthServiceAccessibilityMaskAvailable before calling any
//! other HealthService APIs that involve the given metric with the given scope
//! (like \ref health_service_sum_averaged).
//! @param metric The metric to query for averaged data.
//! @param time_start Earliest UTC time you are interested in.
//! @param time_end Latest UTC time you are interested in.
//! @param scope \ref HealthServiceTimeScope value describing how the average should be computed.
//! @return A \HealthServiceAccessibilityMask value decribing whether averaged data is available.
HealthServiceAccessibilityMask health_service_metric_averaged_accessible(
  HealthMetric metric, time_t time_start, time_t time_end, HealthServiceTimeScope scope);

//! Check if a certain combination of metric, time span, aggregation operation, and scope is
//! accessible for calculating aggregated, averaged data by returning a value of
//! \ref HealthServiceAccessibilityMask. Developers should check if the return value is
//! \ref HealthServiceAccessibilityMaskAvailable before calling
//! \ref health_service_aggregate_averaged.
//! @param metric The metric to query for averaged data.
//! @param time_start Earliest UTC time you are interested in.
//! @param time_end Latest UTC time you are interested in.
//! @param aggregation The aggregation to perform
//! @param scope \ref HealthServiceTimeScope value describing how the average should be computed.
//! @return A \HealthServiceAccessibilityMask value decribing whether averaged data is available.
#define health_service_metric_aggregate_averaged_accessible( \
    metric, time_start, time_end, aggregation, scope) (HealthServiceAccessibilityMaskNotSupported)

//! Check if a certain combination of metric, \ref HealthActivityMask and time span is
//! accessible. Developers should check if the return value is
//! \ref HealthServiceAccessibilityMaskAvailable before calling any other HealthService APIs that
//! involve the given activities.
//! @param activity_mask A bitmask of activities you are interested in.
//! @param time_start Earliest UTC time you are interested in.
//! @param time_end Latest UTC time you are interested in.
//! @return A \ref HealthServiceAccessibilityMask representing which of the
//! passed \ref HealthActivityMask values are available under the given constraints.
HealthServiceAccessibilityMask health_service_any_activity_accessible(
    HealthActivityMask activity_mask, time_t time_start, time_t time_end);

//! Health event enum. Passed into the \ref HealthEventHandler.
typedef enum {
  //! All data is considered as outdated and apps should re-read all health data.
  //! This happens after an app is subscribed via \ref health_service_events_subscribe(),
  //! on a change of the day, or in other cases that significantly change the underlying data.
  HealthEventSignificantUpdate = 0,
  //! Recent values around \ref HealthMetricStepCount, \ref HealthMetricActiveSeconds,
  //! or \ref HealthMetricWalkedDistanceMeters have changed.
  HealthEventMovementUpdate = 1,
  //! Recent values around \ref HealthMetricSleepSeconds, \ref HealthMetricSleepRestfulSeconds,
  //! \ref HealthActivitySleep, and \ref HealthActivityRestfulSleep changed.
  HealthEventSleepUpdate = 2,
  //! A metric has either entered or exited the range set by
  //! \ref health_service_register_metric_alert.
  HealthEventMetricAlert = 3,
  //! Recent values around \ref HealthMetricHeartRateBPM or \ref HealthMetricRestingHeartRateBPM
  //! have changed.
  HealthEventHeartRateUpdate = 4,
} HealthEventType;

//! Developer-supplied event handler, called when a health-related event occurs after subscribing
//! via \ref health_service_events_subscribe();
//! @param event The type of health-related event that occured.
//! @param context The developer-supplied context pointer.
typedef void (*HealthEventHandler)(HealthEventType event, void *context);

//! Subscribe to HealthService events. This allocates a cache on the application's heap of up
//! to 2048 bytes that will be de-allocated if you call \ref health_service_events_unsubscribe().
//! If there's not enough heap available, this function will return `false` and will not
//! subscribe to any events.
//! @param handler Developer-supplied event handler function.
//! @param context Developer-supplied context pointer.
//! @return `true` on success, `false` on failure.
bool health_service_events_subscribe(HealthEventHandler handler, void *context);

//! Unsubscribe from HealthService events.
//! @return `true` on success, `false` on failure.
bool health_service_events_unsubscribe(void);

//! Set the desired sampling period for heart rate readings. Normally, the system will sample the
//! heart rate using a sampling period that is automatically chosen to provide useful information
//! without undue battery drain (it automatically samples more often during periods of intense
//! activity, and less often when the user is idle). If desired though, an application can request a
//! specific sampling period using this call. The system will use this as a suggestion, but does not
//! guarantee that the requested period will be used. The actual sampling period may be greater or
//! less due to system needs or heart rate sensor reading quality issues. Each time a new
//! heart rate reading becomes available, a `HealthEventHeartRateUpdate` event will be sent to the
//! application's `HealthEventHandler`. The sample period request will remain in effect the entire
//! time the app is running unless it is explicity cancelled (by calling this method again with 0 as
//! the desired interval). If the app exits without first cancelling the request, it will remain in
//! effect even for a limited time afterwards. To determine how long it will remain active
//! after the app exits, use `health_service_get_heart_rate_sample_period_expiration_sec`. Unless
//! the app explicitly needs to access to historical high-resolution heart rate data, it is best
//! practice to always cancel the sample period request before exiting in order to maximize battery
//! life. Historical heart rate data can be accessed using the `health_service_get_minute_history`
//! call.
//! @note The fastest sampling rate that will be acknowledged by the system is 60 seconds. Values
//!      passed in that are smaller than 60 seconds will be changed to 60 seconds. The end result
//!      of this is that a `HealthEventHeartRateUpdate` will be sent to the application's
//!      `HealthEventHandler` no more frequently than 60 seconds.
//! @param interval_sec desired interval between heart rate reading updates. Pass 0 to
//!   go back to automatically chosen intervals.
//! @return `true` on success, `false` on failure
#define health_service_set_heart_rate_sample_period(interval_sec) (false)

//! Return how long a heart rate sample period request (sent via
//! `health_service_set_heart_rate_sample_period`) will remain active after the app exits. If
//! there is no current request by this app, this call will return 0.
//! @return The number of seconds the heart rate sample period request will remain active after
//! the app exits, or 0 if there is no active request by this app.
#define health_service_get_heart_rate_sample_period_expiration_sec() (0)

//! Register for an alert when a metric crosses a given threshold. When the metric crosses this
//! threshold (either goes above or below it), a \ref HealthEventMetricRange event will be
//! generated. To cancel this registration, pass the returned \ref HealthMetricAlert value to
//! \ref health_service_cancel_metric_alert. The only metrics currently supported by this call are
//! \ref HealthMetricHeartRateBPM and \ref HealthMetricRestingHeartRateBPM, but future versions may
//! support additional metrics. To see if a specific metric is supported by this call, use
//! `time_t now = time(NULL); health_service_metric_aggregate_averaged_accessible(metric, now,
//! now, HealthAggregateAvg, HealthServiceTimeScopeOnce)`
//! @param threshold the threshold value
//! @return handle to the alert registration on success, NULL on failure
#define health_service_register_metric_alert(metric, threshold) (NULL)

//! Cancel an metric alert previously created with \ref health_service_register_metric_alert.
//! @param alert the \ref HealthMetricAlert previously returned by
//!  \ref health_service_register_metric_alert
//! @return `true` on success, `false` on failure
#define health_service_cancel_metric_alert(alert) (false)

//! Light level enum
typedef enum AmbientLightLevel {
  AmbientLightLevelUnknown = 0,
  AmbientLightLevelVeryDark,
  AmbientLightLevelDark,
  AmbientLightLevelLight,
  AmbientLightLevelVeryLight,
} AmbientLightLevel;

//! Structure representing a single minute data record returned
//! by \ref health_service_get_minute_history().
//! The `orientation` field encodes the angle of the watch in the x-y plane (the "yaw") in the
//! lower 4 bits (360 degrees linearly mapped to 1 of 16 different values) and the angle to the
//! z axis (the "pitch") in the upper 4 bits.
//! The `vmc` value is a measure of the total amount of movement seen by the watch. More vigorous
//! movement yields higher VMC values.
typedef struct {
  uint8_t steps;              //!< Number of steps taken in this minute.
  uint8_t orientation;        //!< Quantized average orientation.
  uint16_t vmc;               //!< Vector Magnitude Counts (vmc).
  bool is_invalid: 1;         //!< `true` if the item doesn't represents actual data
                              //!< and should be ignored.
  AmbientLightLevel light: 3; //!< Instantaneous light level during this minute.
  uint8_t padding: 4;
  uint8_t heart_rate_bpm;     //!< heart rate in beats per minute
  uint8_t reserved[6];        //!< Reserved for future use.
} HealthMinuteData;

//! Return historical minute data records. This fills in the `minute_data` array parameter with
//! minute by minute statistics of the user's steps, average watch orientation, etc. The data is
//! returned in time order, with the oldest minute data returned at `minute_data[0]`.
//! @param minute_data Pointer to an array of \ref HealthMinuteData records that will be filled
//!      in with the historical minute data.
//! @param max_records The maximum number of records the `minute_data` array can hold.
//! @param[in,out] time_start On entry, the UTC time of the first requested record. On exit,
//!      the UTC time of the first second of the first record actually returned.
//!      If `time_start` on entry is somewhere in the middle of a minute interval, this function
//!      behaves as if the caller passed in the start of that minute.
//! @param[in,out] time_end On entry, the UTC time of the end of the requested range of records. On
//!      exit, the UTC time of the end of the last record actually returned (i.e. start time of last
//!      record + 60). If `time_end` on entry is somewhere in the middle of a minute interval, this
//!      function behaves as if the caller passed in the end of that minute.
//! @return Actual number of records returned. May be less then the maximum requested.
//! @note If the return value is zero, `time_start` and `time_end` are meaningless.
//!      It's not guaranteed that all records contain valid data, even if the return value is
//!      greater than zero. Check `HealthMinuteData.is_invalid` to see if a given record contains
//!      valid data.
uint32_t
health_service_get_minute_history(HealthMinuteData *minute_data, uint32_t max_records,
                                           time_t *time_start, time_t *time_end);

//! Convenience macro to switch between two expressions depending on health support.
//! On platforms with health support the first expression will be chosen, the second otherwise.
#define PBL_IF_HEALTH_ELSE(if_true, if_false) (if_false)

//! Types of measurement system a \ref HealthMetric may be measured in.
typedef enum {
  //! The measurement system is unknown, or does not apply to the chosen metric.
  MeasurementSystemUnknown,
  //! The metric measurement system.
  MeasurementSystemMetric,
  //! The imperial measurement system.
  MeasurementSystemImperial,
} MeasurementSystem;

//! Get the preferred measurement system for a given \ref HealthMetric, if the user has chosen
//! a preferred system and it is applicable to that metric.
//! @param metric A metric value chosen from \ref HealthMetric.
//! @return A value from \ref MeasurementSystem if applicable, else \ref MeasurementSystemUnknown.
MeasurementSystem health_service_get_measurement_system_for_display(HealthMetric metric);

//! @} // group HealthService

//! @} // group EventService

//! @addtogroup DataLogging
//! \brief Enables logging data asynchronously to a mobile app
//!
//! In Pebble OS, data logging is a data storage and transfer subsystem that allows watchapps to
//! save data on non-volatile storage devices when the phone is not available to process it. The
//! API provides your watchapp with a mechanism for short-term data buffering for asynchronous data
//! transmission to a mobile app.
//!
//! Using this API, your Pebble watchapp can create an arbitrary number of logs, but you’re
//! limited in the amount of storage space you can use. Note that approximately 640K is available
//! for data logging, which is shared among all watchapps that use it. This value is subject to
//! change in the future. When the data spool is full, an app will start overwriting its own data.
//! An app cannot overwrite another apps's data. However, the other app might have 0 bytes for data
//! logging.
//!
//! Your app can log data to a session, either creating, adding or deleting data to that session.
//! The data is then sent to the associated phone application at the earliest convenience.
//! If a phone is available, the data is sent directly to the phone. Otherwise, it is saved to the
//! watch storage until the watch is connected to a phone.
//!
//!
//! For example:
//!
//! To create a data logging session for 4-byte unsigned integers with a tag of 0x1234, you would
//! do this: \code{.c}
//!
//! DataLoggingSessionRef logging_session = data_logging_create(0x1234, DATA_LOGGING_UINT, 4,
//!                                                             false);
//!
//! // Fake creating some data and logging it to the session.
//! uint32_t data[] = { 1, 2, 3};
//! data_logging_log(logging_session, &data, 3);
//!
//! // Fake creating more data and logging that as well.
//! uint32_t data2[] = { 1, 2 };
//! data_logging_log(logging_session, &data, 2);
//!
//! // When we don't need to log anything else, we can close off the session.
//! data_logging_finish(logging_session);
//! \endcode
//!
//! @{

typedef void *DataLoggingSessionRef;

//! The different types of session data that Pebble supports. This type describes the type of a
//! singular item in the data session. Every item in a given session is the same type and size.
typedef enum {
  //! Array of bytes. Remember that this is the type of a single item in the logging session, so
  //! using this type means you'll be logging multiple byte arrays (each a fixed length described
  //! by item_length) for the duration of the session.
  DATA_LOGGING_BYTE_ARRAY = 0,
  //! Unsigned integer. This may be a 1, 2, or 4 byte integer depending on the item_length parameter
  DATA_LOGGING_UINT = 2,
  //! Signed integer. This may be a 1, 2, or 4 byte integer depending on the item_length parameter
  DATA_LOGGING_INT = 3,
} DataLoggingItemType;

//! Enumerated values describing the possible outcomes of data logging operations
typedef enum {
  DATA_LOGGING_SUCCESS = 0, //!< Successful operation
  DATA_LOGGING_BUSY, //!< Someone else is writing to this logging session
  DATA_LOGGING_FULL, //!< No more space to save data
  DATA_LOGGING_NOT_FOUND, //!< The logging session does not exist
  DATA_LOGGING_CLOSED, //!< The logging session was made inactive
  DATA_LOGGING_INVALID_PARAMS, //!< An invalid parameter was passed to one of the functions
  DATA_LOGGING_INTERNAL_ERR //!< An internal error occurred
} DataLoggingResult;

//! Create a new data logging session.
//!
//! @param tag A tag associated with the logging session.
//! @param item_type The type of data stored in this logging session
//! @param item_length The size of a single data item in bytes
//! @param resume True if we want to look for a logging session of the same tag and
//!   resume logging to it. If this is false and a session with the specified tag exists, that
//!   session will be closed and a new session will be opened.
//! @return An opaque reference to the data logging session
DataLoggingSessionRef data_logging_create(uint32_t tag, DataLoggingItemType item_type,
                                          uint16_t item_length, bool resume);

//! Finish up a data logging_session. Logging data is kept until it has successfully been
//! transferred over to the phone, but no data may be added to the session after this function is
//! called.
//!
//! @param logging_session a reference to the data logging session previously allocated using
//!   data_logging_create
void data_logging_finish(DataLoggingSessionRef logging_session);

//! Add data to the data logging session. If a phone is available, the data is sent directly
//! to the phone. Otherwise, it is saved to the watch storage until the watch is connected to a
//! phone.
//!
//! @param logging_session a reference to the data logging session you want to add the data to
//! @param data a pointer to the data buffer that contains multiple items
//! @param num_items the number of items to log. This means data must be at least
//!    (num_items * item_length) long in bytes
//! @return
//! DATA_LOGGING_SUCCESS on success
//!
//! @return
//! DATA_LOGGING_NOT_FOUND if the logging session is invalid
//!
//! @return
//! DATA_LOGGING_CLOSED if the sesion is not active
//!
//! @return
//! DATA_LOGGING_BUSY if the sesion is not available for writing
//!
//! @return
//! DATA_LOGGING_INVALID_PARAMS if num_items is 0 or data is NULL
DataLoggingResult data_logging_log(DataLoggingSessionRef logging_session, const void *data,
                                   uint32_t num_items);

//! @} // group DataLogging

//! @addtogroup DataStructures
//! @{

//! @addtogroup UUID
//! @{

typedef struct __attribute__((__packed__)) {
  uint8_t byte0;
  uint8_t byte1;
  uint8_t byte2;
  uint8_t byte3;
  uint8_t byte4;
  uint8_t byte5;
  uint8_t byte6;
  uint8_t byte7;
  uint8_t byte8;
  uint8_t byte9;
  uint8_t byte10;
  uint8_t byte11;
  uint8_t byte12;
  uint8_t byte13;
  uint8_t byte14;
  uint8_t byte15;
} Uuid;

#define UUID_SIZE 16

//! Make a Uuid object from sixteen bytes.
//! @return A Uuid structure representing the bytes p0 to p15.
#define UuidMake(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15) ((Uuid) {p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15})

//! Creates a Uuid from an array of bytes with 16 bytes in Big Endian order.
//! @return The created Uuid
#define UuidMakeFromBEBytes(b) ((Uuid) { b[0], b[1], b[2], b[3], \
                                         b[4], b[5], b[6], b[7], \
                                         b[8], b[9], b[10], b[11], \
                                         b[12], b[13], b[14], b[15] })

//! Creates a Uuid from an array of bytes with 16 bytes in Little Endian order.
//! @return The created Uuid
#define UuidMakeFromLEBytes(b) ((Uuid) { b[15], b[14], b[13], b[12], \
                                         b[11], b[10], b[9], b[8], \
                                         b[7], b[6], b[5], b[4], \
                                         b[3], b[2], b[1], b[0] })

//! Compares two UUIDs.
//! @return True if the two UUIDs are equal, false if they are not.
bool uuid_equal(const Uuid *uu1, const Uuid *uu2);

//! Writes UUID in a string form into buffer that looks like the following...
//! {12345678-1234-5678-1234-567812345678} or {NULL UUID} if NULL was passed.
//! @param uuid The Uuid to write into the buffer as human-readable string
//! @param buffer Memory to write the string to. Must be at least \ref UUID_STRING_BUFFER_LENGTH bytes long.
void uuid_to_string(const Uuid *uuid, char *buffer);

//! The minimum required length of a string used to hold a uuid (including null).
#define UUID_STRING_BUFFER_LENGTH (32 + 4 + 2 + 1)

//! @} // group UUID

//! @} // group DataStructures

//! @addtogroup Logging Logging
//!   \brief Functions related to logging from apps.
//!
//! This module contains the functions necessary to log messages through
//! Bluetooth.
//! @note It is no longer necessary to enable app logging output from the "settings->about" menu on the Pebble for
//! them to be transmitted!  Instead use the "pebble logs" command included with the SDK to activate logs.  The logs
//! will appear right in your console. Logging
//! over Bluetooth is a fairly power hungry operation that non-developers will
//! not need when your apps are distributed.
//! @{

//! Log an app message.
//! @param log_level
//! @param src_filename The source file where the log originates from
//! @param src_line_number The line number in the source file where the log originates from
//! @param fmt A C formatting string
//! @param ... The arguments for the formatting string
//! @param log_level
//! \sa snprintf for details about the C formatting string.
void app_log(uint8_t log_level, const char* src_filename, int src_line_number, const char* fmt, ...)
    __attribute__((format(printf, 4, 5)));

//! A helper macro that simplifies the use of the app_log function
//! @param level The log level to log output as
//! @param fmt A C formatting string
//! @param args The arguments for the formatting string
#define APP_LOG(level, fmt, args...)                                \
  app_log(level, __FILE_NAME__, __LINE__, fmt, ## args)

//! Suggested log level values
typedef enum {
  //! Error level log message
  APP_LOG_LEVEL_ERROR = 1,
  //! Warning level log message
  APP_LOG_LEVEL_WARNING = 50,
  //! Info level log message
  APP_LOG_LEVEL_INFO = 100,
  //! Debug level log message
  APP_LOG_LEVEL_DEBUG = 200,
  //! Verbose Debug level log message
  APP_LOG_LEVEL_DEBUG_VERBOSE = 255,
} AppLogLevel;

//! @} // group Logging

//! @addtogroup Dictionary
//! \brief Data serialization utilities
//!
//!
//! Data residing in different parts of Pebble memory (RAM) may need to be gathered and assembled into
//! a single continuous block for transport over the network via Bluetooth. The process of gathering
//! and assembling this continuous block of data is called serialization.
//!
//! You use data serialization utilities, like Dictionary, Tuple and Tuplet data structures and accompanying
//! functions, to accomplish this task. No transformations are performed on the actual data, however.
//! These Pebble utilities simply help assemble the data into one continuous buffer according to a
//! specific format.
//!
//! \ref AppMessage uses these utilities--in particular, Dictionary--to send information between mobile
//! and Pebble watchapps.
//!
//! <h3>Writing key/value pairs</h3>
//! To write two key/value pairs, without using Tuplets, you would do this:
//! \code{.c}
//! // Byte array + key:
//! static const uint32_t SOME_DATA_KEY = 0xb00bf00b;
//! static const uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
//!
//! // CString + key:
//! static const uint32_t SOME_STRING_KEY = 0xabbababe;
//! static const char *string = "Hello World";
//!
//! // Calculate the buffer size that is needed for the final Dictionary:
//! const uint8_t key_count = 2;
//! const uint32_t size = dict_calc_buffer_size(key_count, sizeof(data),
//!                                             strlen(string) + 1);
//!
//! // Stack-allocated buffer in which to create the Dictionary:
//! uint8_t buffer[size];
//!
//! // Iterator variable, keeps the state of the creation serialization process:
//! DictionaryIterator iter;
//!
//! // Begin:
//! dict_write_begin(&iter, buffer, sizeof(buffer));
//! // Write the Data:
//! dict_write_data(&iter, SOME_DATA_KEY, data, sizeof(data));
//! // Write the CString:
//! dict_write_cstring(&iter, SOME_STRING_KEY, string);
//! // End:
//! const uint32_t final_size = dict_write_end(&iter);
//!
//! // buffer now contains the serialized information
//!
//! \endcode
//!
//! <h3>Reading key/value pairs</h3>
//! To iterate over the key/value pairs in the dictionary that
//! was created in the previous example code, you would do this:
//!
//! \code{.c}
//! Tuple *tuple = dict_read_begin_from_buffer(&iter, buffer, final_size);
//! while (tuple) {
//!   switch (tuple->key) {
//!     case SOME_DATA_KEY:
//!       foo(tuple->value->data, tuple->length);
//!       break;
//!     case SOME_STRING_KEY:
//!       bar(tuple->value->cstring);
//!       break;
//!   }
//!   tuple = dict_read_next(&iter);
//! }
//! \endcode
//!
//! <h3>Tuple and Tuplet data structures</h3>
//! To understand the difference between Tuple and Tuplet data structures:
//! Tuple is the header for a serialized key/value pair, while Tuplet is a helper
//! data structure that references the value you want to serialize. This data
//! structure exists to make the creation of a Dictionary easier to write.
//! Use this mnemonic to remember the difference: TupleT(emplate), the Tuplet being
//! a template to create a Dictionary with Tuple structures.
//!
//! For example:
//! \code{.c}
//! Tuplet pairs[] = {
//!   TupletInteger(WEATHER_ICON_KEY, (uint8_t) 1),
//!   TupletCString(WEATHER_TEMPERATURE_KEY, "1234 Fahrenheit"),
//! };
//! uint8_t buffer[256];
//! uint32_t size = sizeof(buffer);
//! dict_serialize_tuplets_to_buffer(pairs, ARRAY_LENGTH(pairs), buffer, &size);
//!
//! // buffer now contains the serialized information
//! \endcode
//! @{

//! Return values for dictionary write/conversion functions.
typedef enum {
  //! The operation returned successfully
  DICT_OK = 0,
  //! There was not enough backing storage to complete the operation
  DICT_NOT_ENOUGH_STORAGE = 1 << 1,
  //! One or more arguments were invalid or uninitialized
  DICT_INVALID_ARGS = 1 << 2,
  //! The lengths and/or count of the dictionary its tuples are inconsistent
  DICT_INTERNAL_INCONSISTENCY = 1 << 3,
  //! A requested operation required additional memory to be allocated, but
  //! the allocation failed, likely due to insufficient remaining heap memory.
  DICT_MALLOC_FAILED = 1 << 4,
} DictionaryResult;

//! Values representing the type of data that the `value` field of a Tuple contains
typedef enum {
  //! The value is an array of bytes
  TUPLE_BYTE_ARRAY = 0,
  //! The value is a zero-terminated, UTF-8 C-string
  TUPLE_CSTRING = 1,
  //! The value is an unsigned integer. The tuple's `.length` field is used to
  //! determine the size of the integer (1, 2, or 4 bytes).
  TUPLE_UINT = 2,
  //! The value is a signed integer. The tuple's `.length` field is used to
  //! determine the size of the integer (1, 2, or 4 bytes).
  TUPLE_INT = 3,
} TupleType;

//! Data structure for one serialized key/value tuple
//! @note The structure is variable length! The length depends on the value data that the tuple
//! contains.
typedef struct __attribute__((__packed__)) {
  //! The key
  uint32_t key;
  //! The type of data that the `.value` fields contains.
  TupleType type:8;
  //! The length of `.value` in bytes
  uint16_t length;
  //! @brief The value itself.
  //!
  //! The different union fields are provided for convenience, avoiding the need for manual casts.
  //! @note The array length is of incomplete length on purpose, to facilitate
  //! variable length data and because a data length of zero is valid.
  //! @note __Important: The integers are little endian!__
  union {
    //! The byte array value. Valid when `.type` is \ref TUPLE_BYTE_ARRAY.
    uint8_t data[0];
    //! The C-string value. Valid when `.type` is \ref TUPLE_CSTRING.
    char cstring[0];
    //! The 8-bit unsigned integer value. Valid when `.type` is \ref TUPLE_UINT
    //! and `.length` is 1 byte.
    uint8_t uint8;
    //! The 16-bit unsigned integer value. Valid when `.type` is \ref TUPLE_UINT
    //! and `.length` is 2 byte.
    uint16_t uint16;
    //! The 32-bit unsigned integer value. Valid when `.type` is \ref TUPLE_UINT
    //! and `.length` is 4 byte.
    uint32_t uint32;
    //! The 8-bit signed integer value. Valid when `.type` is \ref TUPLE_INT
    //! and `.length` is 1 byte.
    int8_t int8;
    //! The 16-bit signed integer value. Valid when `.type` is \ref TUPLE_INT
    //! and `.length` is 2 byte.
    int16_t int16;
    //! The 32-bit signed integer value. Valid when `.type` is \ref TUPLE_INT
    //! and `.length` is 4 byte.
    int32_t int32;
  } value[];
} Tuple;

struct Dictionary;
typedef struct Dictionary Dictionary;

//! An iterator can be used to iterate over the key/value
//! tuples in an existing dictionary, using \ref dict_read_begin_from_buffer(),
//! \ref dict_read_first() and \ref dict_read_next().
//! An iterator can also be used to append key/value tuples to a dictionary,
//! for example using \ref dict_write_data() or \ref dict_write_cstring().
typedef struct {
  Dictionary *dictionary;  //!< The dictionary being iterated
  const void *end;  //!< Points to the first memory address after the last byte of the dictionary
  //! Points to the next Tuple in the dictionary. Given the end of the
  //! Dictionary has not yet been reached: when writing, the next key/value
  //! pair will be written at the cursor. When reading, the next call
  //! to \ref dict_read_next() will return the cursor.
  Tuple *cursor;
} DictionaryIterator;

//! Calculates the number of bytes that a dictionary will occupy, given
//! one or more value lengths that need to be stored in the dictionary.
//! @note The formula to calculate the size of a Dictionary in bytes is:
//! <pre>1 + (n * 7) + D1 + ... + Dn</pre>
//! Where `n` is the number of Tuples in the Dictionary and `Dx` are the sizes
//! of the values in the Tuples. The size of the Dictionary header is 1 byte.
//! The size of the header for each Tuple is 7 bytes.
//! @param tuple_count The total number of key/value pairs in the dictionary.
//! @param ... The sizes of each of the values that need to be
//! stored in the dictionary.
//! @return The total number of bytes of storage needed.
uint32_t dict_calc_buffer_size(const uint8_t tuple_count, ...);

//! Calculates the size of data that has been written to the dictionary.
//! AKA, the "dictionary size". Note that this is most likely different
//! than the size of the backing storage/backing buffer.
//! @param iter The dictionary iterator
//! @return The total number of bytes which have been written to the dictionary.
uint32_t dict_size(DictionaryIterator* iter);

//! Initializes the dictionary iterator with a given buffer and size,
//! resets and empties it, in preparation of writing key/value tuples.
//! @param iter The dictionary iterator
//! @param buffer The storage of the dictionary
//! @param size The storage size of the dictionary
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @see dict_calc_buffer_size
//! @see dict_write_end
DictionaryResult dict_write_begin(DictionaryIterator *iter, uint8_t * const buffer, const uint16_t size);

//! Adds a key with a byte array value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param data Pointer to the byte array
//! @param size Length of the byte array
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note The data will be copied into the backing storage of the dictionary.
//! @note There is _no_ checking for duplicate keys.
DictionaryResult dict_write_data(DictionaryIterator *iter, const uint32_t key, const uint8_t * const data, const uint16_t size);

//! Adds a key with a C string value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param cstring Pointer to the zero-terminated C string
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note The string will be copied into the backing storage of the dictionary.
//! @note There is _no_ checking for duplicate keys.
DictionaryResult dict_write_cstring(DictionaryIterator *iter, const uint32_t key, const char * const cstring);

//! Adds a key with an integer value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param integer Pointer to the integer value
//! @param width_bytes The width of the integer value
//! @param is_signed Whether the integer's type is signed or not
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note There is _no_ checking for duplicate keys. dict_write_int() is only for serializing a single
//! integer. width_bytes can only be 1, 2, or 4.
DictionaryResult dict_write_int(DictionaryIterator *iter, const uint32_t key, const void *integer, const uint8_t width_bytes, const bool is_signed);

//! Adds a key with an unsigned, 8-bit integer value pair to the dictionary.
//! @param iter The dictionary iterator
//! @param key The key
//! @param value The unsigned, 8-bit integer value
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
//! @note There is _no_ checking for duplicate keys.
//! @note There are counterpart functions for different signedness and widths,
//! `dict_write_uint16()`, `dict_write_uint32()`, `dict_write_int8()`,
//! `dict_write_int16()` and `dict_write_int32()`. The documentation is not
//! repeated for brevity's sake.
DictionaryResult dict_write_uint8(DictionaryIterator *iter, const uint32_t key, const uint8_t value);

DictionaryResult dict_write_uint16(DictionaryIterator *iter, const uint32_t key, const uint16_t value);

DictionaryResult dict_write_uint32(DictionaryIterator *iter, const uint32_t key, const uint32_t value);

DictionaryResult dict_write_int8(DictionaryIterator *iter, const uint32_t key, const int8_t value);

DictionaryResult dict_write_int16(DictionaryIterator *iter, const uint32_t key, const int16_t value);

DictionaryResult dict_write_int32(DictionaryIterator *iter, const uint32_t key, const int32_t value);

//! End a series of writing operations to a dictionary.
//! This must be called before reading back from the dictionary.
//! @param iter The dictionary iterator
//! @return The size in bytes of the finalized dictionary, or 0 if the parameters were invalid.
uint32_t dict_write_end(DictionaryIterator *iter);

//! Initializes the dictionary iterator with a given buffer and size,
//! in preparation of reading key/value tuples.
//! @param iter The dictionary iterator
//! @param buffer The storage of the dictionary
//! @param size The storage size of the dictionary
//! @return The first tuple in the dictionary, or NULL in case the dictionary was empty or if there was a parsing error.
Tuple * dict_read_begin_from_buffer(DictionaryIterator *iter, const uint8_t * const buffer, const uint16_t size);

//! Progresses the iterator to the next key/value pair.
//! @param iter The dictionary iterator
//! @return The next tuple in the dictionary, or NULL in case the end has been reached or if there was a parsing error.
Tuple * dict_read_next(DictionaryIterator *iter);

//! Resets the iterator back to the same state as a call to \ref dict_read_begin_from_buffer() would do.
//! @param iter The dictionary iterator
//! @return The first tuple in the dictionary, or NULL in case the dictionary was empty or if there was a parsing error.
Tuple * dict_read_first(DictionaryIterator *iter);

//! Non-serialized, template data structure for a key/value pair.
//! For strings and byte arrays, it only has a pointer to the actual data.
//! For integers, it provides storage for integers up to 32-bits wide.
//! The Tuplet data structure is useful when creating dictionaries from values
//! that are already stored in arbitrary buffers.
//! See also \ref Tuple, with is the header of a serialized key/value pair.
typedef struct Tuplet {
  //! The type of the Tuplet. This determines which of the struct fields in the
  //! anonymomous union are valid.
  TupleType type;
  //! The key.
  uint32_t key;
  //! Anonymous union containing the reference to the Tuplet's value, being
  //! either a byte array, c-string or integer. See documentation of `.bytes`,
  //! `.cstring` and `.integer` fields.
  union {
    //! Valid when `.type.` is \ref TUPLE_BYTE_ARRAY
    struct {
      //! Pointer to the data
      const uint8_t *data;
      //! Length of the data
      const uint16_t length;
    } bytes;
    //! Valid when `.type.` is \ref TUPLE_CSTRING
    struct {
      //! Pointer to the c-string data
      const char *data;
      //! Length of the c-string, including terminating zero.
      const uint16_t length;
    } cstring;
    //! Valid when `.type.` is \ref TUPLE_INT or \ref TUPLE_UINT
    struct {
      //! Actual storage of the integer.
      //! The signedness can be derived from the `.type` value.
      uint32_t storage;
      //! Width of the integer.
      const uint16_t width;
    } integer;
  }; //!< See documentation of `.bytes`, `.cstring` and `.integer` fields.
} Tuplet;

//! Macro to create a Tuplet with a byte array value
//! @param _key The key
//! @param _data Pointer to the bytes
//! @param _length Length of the buffer
#define TupletBytes(_key, _data, _length) \
((const Tuplet) { .type = TUPLE_BYTE_ARRAY, .key = _key, .bytes = { .data = _data, .length = _length }})

//! Macro to create a Tuplet with a c-string value
//! @param _key The key
//! @param _cstring The c-string value
#define TupletCString(_key, _cstring) \
((const Tuplet) { .type = TUPLE_CSTRING, .key = _key, .cstring = { .data = _cstring, .length = _cstring ? strlen(_cstring) + 1 : 0 }})

//! Macro to create a Tuplet with an integer value
//! @param _key The key
//! @param _integer The integer value
#define TupletInteger(_key, _integer) \
((const Tuplet) { .type = IS_SIGNED(_integer) ? TUPLE_INT : TUPLE_UINT, .key = _key, .integer = { .storage = _integer, .width = sizeof(_integer) }})

//! Callback for \ref dict_serialize_tuplets() utility.
//! @param data The data of the serialized dictionary
//! @param size The size of data
//! @param context The context pointer as passed in to \ref dict_serialize_tuplets()
//! @see dict_serialize_tuplets
typedef void (*DictionarySerializeCallback)(const uint8_t * const data, const uint16_t size, void *context);

//! Utility function that takes a list of Tuplets from which a dictionary
//! will be serialized, ready to transmit or store.
//! @note The callback will be called before the function returns, so the data that
//! that `context` points to, can be stack allocated.
//! @param callback The callback that will be called with the serialized data of the generated dictionary.
//! @param context Pointer to any application specific data that gets passed into the callback.
//! @param tuplets An array of Tuplets that need to be serialized into the dictionary.
//! @param tuplets_count The number of tuplets that follow.
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_serialize_tuplets(DictionarySerializeCallback callback, void *context, const Tuplet * const tuplets, const uint8_t tuplets_count);

//! Utility function that takes an array of Tuplets and serializes them into
//! a dictionary with a given buffer and size.
//! @param tuplets The array of tuplets
//! @param tuplets_count The number of tuplets in the array
//! @param buffer The buffer in which to write the serialized dictionary
//! @param [in] size_in_out The available buffer size in bytes
//! @param [out] size_in_out The number of bytes written
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_serialize_tuplets_to_buffer(const Tuplet * const tuplets, const uint8_t tuplets_count, uint8_t *buffer, uint32_t *size_in_out);

//! Serializes an array of Tuplets into a dictionary with a given buffer and size.
//! @param iter The dictionary iterator
//! @param tuplets The array of tuplets
//! @param tuplets_count The number of tuplets in the array
//! @param buffer The buffer in which to write the serialized dictionary
//! @param [in] size_in_out The available buffer size in bytes
//! @param [out] size_in_out The number of bytes written
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_serialize_tuplets_to_buffer_with_iter(DictionaryIterator *iter, const Tuplet * const tuplets, const uint8_t tuplets_count, uint8_t *buffer, uint32_t *size_in_out);

//! Serializes a Tuplet and writes the resulting Tuple into a dictionary.
//! @param iter The dictionary iterator
//! @param tuplet The Tuplet describing the key/value pair to write
//! @return \ref DICT_OK, \ref DICT_NOT_ENOUGH_STORAGE or \ref DICT_INVALID_ARGS
DictionaryResult dict_write_tuplet(DictionaryIterator *iter, const Tuplet * const tuplet);

//! Calculates the number of bytes that a dictionary will occupy, given
//! one or more Tuplets that need to be stored in the dictionary.
//! @note See \ref dict_calc_buffer_size() for the formula for the calculation.
//! @param tuplets An array of Tuplets that need to be stored in the dictionary.
//! @param tuplets_count The total number of Tuplets that follow.
//! @return The total number of bytes of storage needed.
//! @see Tuplet
uint32_t dict_calc_buffer_size_from_tuplets(const Tuplet * const tuplets, const uint8_t tuplets_count);

//! Type of the callback used in \ref dict_merge()
//! @param key The key that is being updated.
//! @param new_tuple The new tuple. The tuple points to the actual, updated destination dictionary or NULL_TUPLE
//! in case there was an error (e.g. backing buffer was too small).
//! Therefore the Tuple can be used after the callback returns, until the destination dictionary
//! storage is free'd (by the application itself).
//! @param old_tuple The values that will be replaced with `new_tuple`. The key, value and type will be
//! equal to the previous tuple in the old destination dictionary, however the `old_tuple points
//! to a stack-allocated copy of the old data.
//! @param context Pointer to application specific data
//! The storage backing `old_tuple` can only be used during the callback and
//! will no longer be valid after the callback returns.
//! @see dict_merge
typedef void (*DictionaryKeyUpdatedCallback)(const uint32_t key, const Tuple *new_tuple, const Tuple *old_tuple, void *context);

//! Merges entries from another "source" dictionary into a "destination" dictionary.
//! All Tuples from the source are written into the destination dictionary, while
//! updating the exsting Tuples with matching keys.
//! @param dest The destination dictionary to update
//! @param [in,out] dest_max_size_in_out In: the maximum size of buffer backing `dest`. Out: the final size of the updated dictionary.
//! @param source The source dictionary of which its Tuples will be used to update dest.
//! @param update_existing_keys_only Specify True if only the existing keys in `dest` should be updated.
//! @param key_callback The callback that will be called for each Tuple in the merged destination dictionary.
//! @param context Pointer to app specific data that will get passed in when `update_key_callback` is called.
//! @return \ref DICT_OK, \ref DICT_INVALID_ARGS, \ref DICT_NOT_ENOUGH_STORAGE
DictionaryResult dict_merge(DictionaryIterator *dest, uint32_t *dest_max_size_in_out,
                             DictionaryIterator *source,
                             const bool update_existing_keys_only,
                             const DictionaryKeyUpdatedCallback key_callback, void *context);

//! Tries to find a Tuple with specified key in a dictionary
//! @param iter Iterator to the dictionary to search in.
//! @param key The key for which to find a Tuple
//! @return Pointer to a found Tuple, or NULL if there was no Tuple with the specified key.
Tuple *dict_find(const DictionaryIterator *iter, const uint32_t key);

//! @} // group Dictionary

//! @addtogroup Dictation
//! @{

typedef struct DictationSession DictationSession;

typedef enum {
  //! Transcription successful, with a valid result
  DictationSessionStatusSuccess,

  //! User rejected transcription and exited UI
  DictationSessionStatusFailureTranscriptionRejected,

  //! User exited UI after transcription error
  DictationSessionStatusFailureTranscriptionRejectedWithError,

  //! Too many errors occurred during transcription and the UI exited
  DictationSessionStatusFailureSystemAborted,

  //! No speech was detected and UI exited
  DictationSessionStatusFailureNoSpeechDetected,

  //! No BT or internet connection
  DictationSessionStatusFailureConnectivityError,

  //! Voice transcription disabled for this user
  DictationSessionStatusFailureDisabled,

  //! Voice transcription failed due to internal error
  DictationSessionStatusFailureInternalError,

  //! Cloud recognizer failed to transcribe speech (only possible if error dialogs disabled)
  DictationSessionStatusFailureRecognizerError,
} DictationSessionStatus;

//! Dictation status callback. Indicates success or failure of the dictation session and, if
//! successful, passes the transcribed string to the user of the dictation session. The transcribed
//! string will be freed after this call returns, so the string should be copied if it needs to be
//! retained afterwards.
//! @param session        dictation session from which the status was received
//! @param status         dictation status
//! @param transcription  transcribed string
//! @param context        callback context specified when starting the session
typedef void (*DictationSessionStatusCallback)(DictationSession *session,
                                               DictationSessionStatus status, char *transcription,
                                               void *context);

//! Create a dictation session. The session object can be used more than once to get a
//! transcription. When a transcription is received a buffer will be allocated to store the text in
//! with a maximum size specified by \ref buffer_size. When a transcription and accepted by the user
//! or a failure of some sort occurs, the callback specified will be called with the status and the
//! transcription if one was accepted.
//! @param buffer_size       size of buffer to allocate for the transcription text; text will be
//!                          truncated if it is longer than the maximum size specified; a size of 0
//!                          will allow the session to allocate as much as it needs and text will
//!                          not be truncated
//! @param callback          dictation session status handler (must be valid)
//! @param callback_context  context pointer for status handler
//! @return handle to the dictation session or NULL if the phone app is not connected or does not
//! support voice dictation, if this is called on a platform that doesn't support voice dictation,
//! or if an internal error occurs.
DictationSession *dictation_session_create(uint32_t buffer_size,
                                           DictationSessionStatusCallback callback,
                                           void *callback_context);

//! Destroy the dictation session and free its memory. Will terminate a session in progress.
//! @param session  dictation session to be destroyed
void dictation_session_destroy(DictationSession *session);

//! Start the dictation session. The dictation UI will be shown. When the user accepts a
//! transcription or exits the UI, or, when the confirmation dialog is disabled and a status is
//! received, the status callback will be called. Can only be called when no session is in progress.
//! The session can be restarted multiple times after the UI is exited or the session is stopped.
//! @param session  dictation session to start or restart
//! @return true if session was started, false if session is already in progress or is invalid.
DictationSessionStatus dictation_session_start(DictationSession *session);

//! Stop the current dictation session. The UI will be hidden and no status callbacks will be
//! received after the session is stopped.
//! @param session  dictation session to stop
//! @return true if session was stopped, false if session was not started or is invalid
DictationSessionStatus dictation_session_stop(DictationSession *session);

//! Enable or disable user confirmation of transcribed text, which allows the user to accept or
//! reject (and restart) the transcription. Must be called before the session is started.
//! @param session      dictation session to modify
//! @param is_enabled   set to true to enable user confirmation of transcriptions (default), false
//! to disable
void dictation_session_enable_confirmation(DictationSession *session, bool is_enabled);

//! Enable or disable error dialogs when transcription fails. Must be called before the session
//! is started. Disabling error dialogs will also disable automatic retries if transcription fails.
//! @param session      dictation session to modify
//! @param is_enabled   set to true to enable error dialogs (default), false to disable
void dictation_session_enable_error_dialogs(DictationSession *session, bool is_enabled);

//! Convenience macro to switch between two expressions depending on mic support.
//! On platforms with a mic the first expression will be chosen, the second otherwise.
#define PBL_IF_MICROPHONE_ELSE(if_true, if_false) (if_false)

//! @} // group Dictation

//! @addtogroup AppMessage
//!
//!
//!
//! \brief Bi-directional communication between phone apps and Pebble watchapps
//!
//! AppMessage is a bi-directional messaging subsystem that enables communication between phone apps
//! and Pebble watchapps. This is accomplished by allowing phone and watchapps to exchange arbitrary
//! sets of key/value pairs. The key/value pairs are stored in the form of a Dictionary, the layout
//! of which is left for the application developer to define.
//!
//! AppMessage implements a push-oriented messaging protocol, enabling your app to call functions and
//! methods to push messages from Pebble to phone and vice versa. The protocol is symmetric: both Pebble
//! and the phone can send messages. All messages are acknowledged. In this context, there is no
//! client-server model, as such.
//!
//! During the sending phase, one side initiates the communication by transferring a dictionary over the air.
//! The other side then receives this message and is given an opportunity to perform actions on that data.
//! As soon as possible, the other side is expected to reply to the message with a simple acknowledgment
//! that the message was received successfully.
//!
//! PebbleKit JavaScript provides you with a set of standard JavaScript APIs that let your app receive messages
//! from the watch, make HTTP requests, and send new messages to the watch. AppMessage APIs are used to send and
//! receive data. A Pebble watchapp can use the resources of the connected phone to fetch information from web services,
//! send information to web APIs, or store login credentials. On the JavaScript side, you communicate
//! with Pebble via a Pebble object exposed in the namespace.
//!
//! Messages always need to get either ACKnowledged or "NACK'ed," that is, not acknowledged.
//! If not, messages will result in a time-out failure. The AppMessage subsystem takes care of this implicitly.
//! In the phone libraries, this step is a bit more explicit.
//!
//! The Pebble watch interfaces make a distinction between the Inbox and the Outbox calls. The Inbox
//! receives messages from the phone on the watch; the Outbox sends messages from the watch to the phone.
//! These two buffers can be managed separately.
//!
//! <h4>Warning</h4>
//! A critical constraint of AppMessage is that messages are limited in size. An ingoing (outgoing) message
//! larger than the inbox (outbox) will not be transmitted and will generate an error. You can choose your
//! inbox and outbox size when you call app_message_open().
//!
//! Pebble SDK provides a static minimum guaranteed size (APP_MESSAGE_INBOX_SIZE_MINIMUM and APP_MESSAGE_OUTBOX_SIZE_MINIMUM).
//! Requesting a buffer of the minimum guaranteed size (or smaller) is always guaranteed to succeed on all
//! Pebbles in this SDK version or higher, and with every phone.
//!
//! In some context, Pebble might be able to provide your application with larger inbox/outbox.
//! You can call app_message_inbox_size_maximum() and app_message_outbox_size_maximum() in your code to get
//! the largest possible value you can use.
//!
//! To always get the largest buffer available, follow this best practice:
//!
//! app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum())
//!
//! AppMessage uses your application heap space. That means that the sizes you pick for the AppMessage
//! inbox and outbox buffers are important in optimizing your app’s performance. The more you use for
//! AppMessage, the less space you’ll have for the rest of your app.
//!
//! To register callbacks, you should call app_message_register_inbox_received(), app_message_register_inbox_dropped(),
//! app_message_register_outbox_sent(), app_message_register_outbox_failed().
//!
//! Pebble recommends that you call them before app_message_open() to ensure you do not miss a message
//! arriving between starting AppMessage and registering the callback. You can set a context that will be passed
//! to all the callbacks with app_message_set_context().
//!
//! In circumstances that may not be ideal, when using AppMessage several types of errors may occur.
//! For example:
//!
//! * The send can’t start because the system state won't allow for a success. Several reasons
//!  you're unable to perform a send: A send() is already occurring (only one is possible at a time) or Bluetooth
//!  is not enabled or connected.
//! * The send and receive occur, but the receiver can’t accept the message. For instance, there is no app
//!   that receives such a message.
//! * The send occurs, but the receiver either does not actually receive the message or can’t handle it
//!   in a timely fashion.
//! * In the case of a dropped message, the phone sends a message to the watchapp, while there is still
//!   an unprocessed message in the Inbox.
//!
//! Other errors are possible and described by AppMessageResult. A client of the AppMessage interface
//! should use the result codes to be more robust in the face of communication problems either in the field or while debugging.
//!
//! Refer to the \htmlinclude app-phone-communication.html for a conceptual overview and code usage.
//!
//! For code examples, refer to the SDK Examples that directly use App Message. These include:
//!   * <a href="https://github.com/pebble-examples/pebblekit-js-weather">
//!     pebblekit-js-weather</a>
//!   * <a href="https://github.com/pebble-examples/pebblekit-js-quotes">
//!     pebblekit-js-quotes</a>
//! @{

//! AppMessage result codes.
typedef enum {
  //! (0) All good, operation was successful.
  APP_MSG_OK = 0,

  //! (2) The other end did not confirm receiving the sent data with an (n)ack in time.
  APP_MSG_SEND_TIMEOUT = 1 << 1,

  //! (4) The other end rejected the sent data, with a "nack" reply.
  APP_MSG_SEND_REJECTED = 1 << 2,

  //! (8) The other end was not connected.
  APP_MSG_NOT_CONNECTED = 1 << 3,

  //! (16) The local application was not running.
  APP_MSG_APP_NOT_RUNNING = 1 << 4,

  //! (32) The function was called with invalid arguments.
  APP_MSG_INVALID_ARGS = 1 << 5,

  //! (64) There are pending (in or outbound) messages that need to be processed first before
  //! new ones can be received or sent.
  APP_MSG_BUSY = 1 << 6,

  //! (128) The buffer was too small to contain the incoming message.
  APP_MSG_BUFFER_OVERFLOW = 1 << 7,

  //! (512) The resource had already been released.
  APP_MSG_ALREADY_RELEASED = 1 << 9,

  //! (1024) The callback was already registered.
  APP_MSG_CALLBACK_ALREADY_REGISTERED = 1 << 10,

  //! (2048) The callback could not be deregistered, because it had not been registered before.
  APP_MSG_CALLBACK_NOT_REGISTERED = 1 << 11,

  //! (4096) The system did not have sufficient application memory to
  //! perform the requested operation.
  APP_MSG_OUT_OF_MEMORY = 1 << 12,

  //! (8192) App message was closed.
  APP_MSG_CLOSED = 1 << 13,

  //! (16384) An internal OS error prevented AppMessage from completing an operation.
  APP_MSG_INTERNAL_ERROR = 1 << 14,

  //! (32768) The function was called while App Message was not in the appropriate state.
  APP_MSG_INVALID_STATE = 1 << 15,
} AppMessageResult;

//! Open AppMessage to transfers.
//!
//! Use \ref dict_calc_buffer_size_from_tuplets() or \ref dict_calc_buffer_size() to estimate the size you need.
//!
//! \param[in] size_inbound The required size for the Inbox buffer
//! \param[in] size_outbound The required size for the Outbox buffer
//!
//! \return A result code such as \ref APP_MSG_OK or \ref APP_MSG_OUT_OF_MEMORY.
//!
//! \note It is recommended that if the Inbox will be used, that at least the Inbox callbacks should be registered
//!   before this call.  Otherwise it is possible for an Inbox message to be NACK'ed without being seen by the
//!   application.
//!
AppMessageResult app_message_open(const uint32_t size_inbound, const uint32_t size_outbound);

//! Deregisters all callbacks and their context.
//!
void app_message_deregister_callbacks(void);

//! Called after an incoming message is received.
//!
//! \param[in] iterator
//!   The dictionary iterator to the received message.  Never NULL.  Note that the iterator cannot be modified or
//!   saved off.  The library may need to re-use the buffered space where this message is supplied.  Returning from
//!   the callback indicates to the library that the received message contents are no longer needed or have already
//!   been externalized outside its buffering space and iterator.
//!
//! \param[in] context
//!   Pointer to application data as specified when registering the callback.
//!
typedef void (*AppMessageInboxReceived)(DictionaryIterator *iterator, void *context);

//! Called after an incoming message is dropped.
//!
//! \param[in] result
//!   The reason why the message was dropped.  Some possibilities include \ref APP_MSG_BUSY and
//!   \ref APP_MSG_BUFFER_OVERFLOW.
//!
//! \param[in] context
//!   Pointer to application data as specified when registering the callback.
//!
//! Note that you can call app_message_outbox_begin() from this handler to prepare a new message.
//! This will invalidate the previous dictionary iterator; do not use it after calling app_message_outbox_begin().
//!
typedef void (*AppMessageInboxDropped)(AppMessageResult reason, void *context);

//! Called after an outbound message has been sent and the reply has been received.
//!
//! \param[in] iterator
//!   The dictionary iterator to the sent message.  The iterator will be in the final state that was sent.  Note that
//!   the iterator cannot be modified or saved off as the library will re-open the dictionary with dict_begin() after
//!   this callback returns.
//!
//! \param[in] context
//!   Pointer to application data as specified when registering the callback.
//!
typedef void (*AppMessageOutboxSent)(DictionaryIterator *iterator, void *context);

//! Called after an outbound message has not been sent successfully.
//!
//! \param[in] iterator
//!   The dictionary iterator to the sent message.  The iterator will be in the final state that was sent.  Note that
//!   the iterator cannot be modified or saved off as the library will re-open the dictionary with dict_begin() after
//!   this callback returns.
//!
//! \param[in] result
//!   The result of the operation.  Some possibilities for the value include \ref APP_MSG_SEND_TIMEOUT,
//!   \ref APP_MSG_SEND_REJECTED, \ref APP_MSG_NOT_CONNECTED, \ref APP_MSG_APP_NOT_RUNNING, and the combination
//!   `(APP_MSG_NOT_CONNECTED | APP_MSG_APP_NOT_RUNNING)`.
//!
//! \param context
//!   Pointer to application data as specified when registering the callback.
//!
//! Note that you can call app_message_outbox_begin() from this handler to prepare a new message.
//! This will invalidate the previous dictionary iterator; do not use it after calling app_message_outbox_begin().
//!
typedef void (*AppMessageOutboxFailed)(DictionaryIterator *iterator, AppMessageResult reason, void *context);

//! Gets the context that will be passed to all AppMessage callbacks.
//!
//! \return The current context on record.
//!
void *app_message_get_context(void);

//! Sets the context that will be passed to all AppMessage callbacks.
//!
//! \param[in] context The context that will be passed to all AppMessage callbacks.
//!
//! \return The previous context that was on record.
//!
void *app_message_set_context(void *context);

//! Registers a function that will be called after any Inbox message is received successfully.
//!
//! Only one callback may be registered at a time.  Each subsequent call to this function will replace the previous
//! callback.  The callback is optional; setting it to NULL will deregister the current callback and no function will
//! be called anymore.
//!
//! \param[in] received_callback The callback that will be called going forward; NULL to not have a callback.
//!
//! \return The previous callback (or NULL) that was on record.
//!
AppMessageInboxReceived app_message_register_inbox_received(AppMessageInboxReceived received_callback);

//! Registers a function that will be called after any Inbox message is received but dropped by the system.
//!
//! Only one callback may be registered at a time.  Each subsequent call to this function will replace the previous
//! callback.  The callback is optional; setting it to NULL will deregister the current callback and no function will
//! be called anymore.
//!
//! \param[in] dropped_callback The callback that will be called going forward; NULL to not have a callback.
//!
//! \return The previous callback (or NULL) that was on record.
//!
AppMessageInboxDropped app_message_register_inbox_dropped(AppMessageInboxDropped dropped_callback);

//! Registers a function that will be called after any Outbox message is sent and an ACK reply occurs in a timely
//! fashion.
//!
//! Only one callback may be registered at a time.  Each subsequent call to this function will replace the previous
//! callback.  The callback is optional; setting it to NULL will deregister the current callback and no function will
//! be called anymore.
//!
//! \param[in] sent_callback The callback that will be called going forward; NULL to not have a callback.
//!
//! \return The previous callback (or NULL) that was on record.
//!
AppMessageOutboxSent app_message_register_outbox_sent(AppMessageOutboxSent sent_callback);

//! Registers a function that will be called after any Outbox message is not sent with a timely ACK reply.
//! The call to \ref app_message_outbox_send() must have succeeded.
//!
//! Only one callback may be registered at a time.  Each subsequent call to this function will replace the previous
//! callback.  The callback is optional; setting it to NULL will deregister the current callback and no function will
//! be called anymore.
//!
//! \param[in] failed_callback The callback that will be called going forward; NULL to not have a callback.
//!
//! \return The previous callback (or NULL) that was on record.
//!
AppMessageOutboxFailed app_message_register_outbox_failed(AppMessageOutboxFailed failed_callback);

//! Programatically determine the inbox size maximum in the current configuration.
//!
//! \return The inbox size maximum on this firmware.
//!
//! \sa APP_MESSAGE_INBOX_SIZE_MINIMUM
//! \sa app_message_outbox_size_maximum()
//!
uint32_t app_message_inbox_size_maximum(void);

//! Programatically determine the outbox size maximum in the current configuration.
//!
//! \return The outbox size maximum on this firmware.
//!
//! \sa APP_MESSAGE_OUTBOX_SIZE_MINIMUM
//! \sa app_message_inbox_size_maximum()
//!
uint32_t app_message_outbox_size_maximum(void);

//! Begin writing to the Outbox's Dictionary buffer.
//!
//! \param[out] iterator Location to write the DictionaryIterator pointer.  This will be NULL on failure.
//!
//! \return A result code, including but not limited to \ref APP_MSG_OK, \ref APP_MSG_INVALID_ARGS or
//!   \ref APP_MSG_BUSY.
//!
//! \note After a successful call, one can add values to the dictionary using functions like \ref dict_write_data()
//!   and friends.
//!
//! \sa Dictionary
//!
AppMessageResult app_message_outbox_begin(DictionaryIterator **iterator);

//! Sends the outbound dictionary.
//!
//! \return A result code, including but not limited to \ref APP_MSG_OK or \ref APP_MSG_BUSY.  The APP_MSG_OK code does
//!         not mean that the message was sent successfully, but only that the start of processing was successful.
//!         Since this call is asynchronous, callbacks provide the final result instead.
//!
//! \sa AppMessageOutboxSent
//! \sa AppMessageOutboxFailed
//!
AppMessageResult app_message_outbox_send(void);

//! As long as the firmware maintains its current major version, inboxes of this size or smaller will be allowed.
//!
//! \sa app_message_inbox_size_maximum()
//! \sa APP_MESSAGE_OUTBOX_SIZE_MINIMUM
//!
#define APP_MESSAGE_INBOX_SIZE_MINIMUM 124

//! As long as the firmware maintains its current major version, outboxes of this size or smaller will be allowed.
//!
//! \sa app_message_outbox_size_maximum()
//! \sa APP_MESSAGE_INBOX_SIZE_MINIMUM
//!
#define APP_MESSAGE_OUTBOX_SIZE_MINIMUM 636

//! @} // group AppMessage

//! @addtogroup AppSync
//! \brief UI synchronization layer for AppMessage
//!
//! AppSync is a convenience layer that resides on top of \ref AppMessage, and serves
//! as a UI synchronization layer for AppMessage. In so doing, AppSync makes it easier
//! to drive the information displayed in the watchapp UI with messages sent by a phone app.
//!
//! AppSync maintains and updates a Dictionary, and provides your app with a callback
//! (AppSyncTupleChangedCallback) routine that is called whenever the Dictionary changes
//! and the app's UI is updated. Note that the app UI is not updated automatically.
//! To update the UI, you need to implement the callback.
//!
//! Pebble OS provides support for data serialization utilities, like Dictionary,
//! Tuple and Tuplet data structures and their accompanying functions. You use Tuplets to create
//! a Dictionary with Tuple structures.
//!
//! AppSync manages the storage and bookkeeping chores of the current Tuple values. AppSync copies
//! incoming AppMessage Tuples into this "current" Dictionary, so that the key/values remain
//! available for the UI to use. For example, it is safe to use a C-string value provided by AppSync
//! and use it directly in a text_layer_set_text() call.
//!
//! Your app needs to supply the buffer that AppSync uses for the "current" Dictionary when
//! initializing AppSync.
//!
//! Refer to the
//! <a href="https://developer.getpebble.com/guides/pebble-apps/communications/appsync/">
//! Synchronizing App UI</a>
//! guide for a conceptual overview and code usage.
//! @{

//! Called whenever a Tuple changes. This does not necessarily mean the value in
//! the Tuple has changed. When the internal "current" dictionary gets updated,
//! existing Tuples might get shuffled around in the backing buffer, even though
//! the values stay the same. In this callback, the client code gets the chance
//! to remove the old reference and start using the new one.
//! In this callback, your application MUST clean up any references to the
//! `old_tuple` of a PREVIOUS call to this callback (and replace it with the
//! `new_tuple` that is passed in with the current call).
//! @param key The key for which the Tuple was changed.
//! @param new_tuple The new tuple. The tuple points to the actual, updated
//! "current" dictionary, as backed by the buffer internal to the AppSync
//! struct. Therefore the Tuple can be used after the callback returns, until
//! the AppSync is deinited. In case there was an error (e.g. storage shortage),
//! this `new_tuple` can be `NULL_TUPLE`.
//! @param old_tuple The values that will be replaced with `new_tuple`. The key,
//! value and type will be equal to the previous tuple in the old destination
//! dictionary; however, the `old_tuple` points to a stack-allocated copy of the
//! old data. This value will be `NULL_TUPLE` when the initial values are
//! being set.
//! @param context Pointer to application specific data, as set using
//! \ref app_sync_init()
//! @see \ref app_sync_init()
typedef void (*AppSyncTupleChangedCallback)(const uint32_t key, const Tuple *new_tuple,
                                            const Tuple *old_tuple, void *context);

//! Called whenever there was an error.
//! @param dict_error The dictionary result error code, if the error was
//! dictionary related.
//! @param app_message_error The app_message result error code, if the error
//! was app_message related.
//! @param context Pointer to application specific data, as set using
//! \ref app_sync_init()
//! @see \ref app_sync_init()
typedef void (*AppSyncErrorCallback)(DictionaryResult dict_error,
                                     AppMessageResult app_message_error, void *context);

typedef struct AppSync {
  DictionaryIterator current_iter;
  union {
    Dictionary *current;
    uint8_t *buffer;
  };
  uint16_t buffer_size;
  struct {
    AppSyncTupleChangedCallback value_changed;
    AppSyncErrorCallback error;
    void *context;
  } callback;
} AppSync;

//! Initialized an AppSync system with specific buffer size and initial keys and
//! values. The `callback.value_changed` callback will be called
//! __asynchronously__ with the initial keys and values, as to avoid duplicating
//! code to update your app's UI.
//! @param s The AppSync context to initialize
//! @param buffer The buffer that AppSync should use
//! @param buffer_size The size of the backing storage of the "current"
//! dictionary. Use \ref dict_calc_buffer_size_from_tuplets() to estimate the
//! size you need.
//! @param keys_and_initial_values An array of Tuplets with the initial keys and
//! values.
//! @param count The number of Tuplets in the `keys_and_initial_values` array.
//! @param tuple_changed_callback The callback that will handle changed
//! key/value pairs
//! @param error_callback The callback that will handle errors
//! @param context Pointer to app specific data that will get passed into calls
//! to the callbacks
//! @note Only updates for the keys specified in this initial array will be
//! accepted by AppSync, updates for other keys that might come in will just be
//! ignored.
void app_sync_init(struct AppSync *s, uint8_t *buffer, const uint16_t buffer_size,
                   const Tuplet * const keys_and_initial_values, const uint8_t count,
                   AppSyncTupleChangedCallback tuple_changed_callback,
                   AppSyncErrorCallback error_callback, void *context);

//! Cleans up an AppSync system.
//! It frees the buffer allocated by an \ref app_sync_init() call and
//! deregisters itself from the \ref AppMessage subsystem.
//! @param s The AppSync context to deinit.
void app_sync_deinit(struct AppSync *s);

//! Updates key/value pairs using an array of Tuplets.
//! @note The call will attempt to send the updated keys and values to the
//! application on the other end.
//! Only after the other end has acknowledged the update, the `.value_changed`
//! callback will be called to confirm the update has completed and your
//! application code can update its user interface.
//! @param s The AppSync context
//! @param keys_and_values_to_update An array of Tuplets with the keys and
//! values to update. The data in the Tuplets are copied during the call, so the
//! array can be stack-allocated.
//! @param count The number of Tuplets in the `keys_and_values_to_update` array.
//! @return The result code from the \ref AppMessage subsystem.
//! Can be \ref APP_MSG_OK, \ref APP_MSG_BUSY or \ref APP_MSG_INVALID_ARGS
AppMessageResult app_sync_set(struct AppSync *s, const Tuplet * const keys_and_values_to_update,
                              const uint8_t count);

//! Finds and gets a tuple in the "current" dictionary.
//! @param s The AppSync context
//! @param key The key for which to find a Tuple
//! @return Pointer to a found Tuple, or NULL if there was no Tuple with the
//! specified key.
const Tuple * app_sync_get(const struct AppSync *s, const uint32_t key);

//! @} // group AppSync

//! @addtogroup Resources
//! \brief Managing application resources
//!
//! Resources are data files that are bundled with your application binary and can be
//! loaded at runtime. You use resources to embed images or custom fonts in your app,
//! but also to embed any data file. Resources are always read-only.
//!
//! Resources are stored on Pebble’s flash memory and only loaded in RAM when you load
//! them. This means that you can have a large number of resources embedded inside your app,
//! even though Pebble’s RAM memory is very limited.
//!
//! See \htmlinclude UsingResources.html for information on how to embed
//! resources into your app's bundle.
//!
//! @{

//! @addtogroup FileFormats File Formats
//! @{

//! @addtogroup PNGFileFormat PNG8 File Format
//!
//! Pebble supports both a PBIs (uncompressed bitmap images) as well as PNG8 images.
//! PNG images are compressed allowing for storage savings up to 90%.
//! PNG8 is a PNG that uses palette-based or grayscale images with 1, 2, 4 or 8 bits per pixel.
//! For palette-based images the pixel data represents the index into the palette, such
//! that each pixel only needs to be large enough to represent the palette size, so
//! \li \c 1-bit supports up to 2 colors,
//! \li \c 2-bit supports up to 4 colors,
//! \li \c 4-bit supports up to 16 colors,
//! \li \c 8-bit supports up to 256 colors.
//!
//! There are 2 parts to the palette: the RGB24 color-mapping palette ("PLTE"), and the optional
//! 8-bit transparency palette ("tRNs").  A pixel's color index maps to both tables, combining to
//! allow the pixel to have both color as well as transparency.
//!
//! For grayscale images, the pixel data represents the luminosity (or shade of gray).
//! \li \c 1-bit supports black and white
//! \li \c 2-bit supports black, dark_gray, light_gray and white
//! \li \c 4-bit supports black, white and 14 shades of gray
//! \li \c 8-bit supports black, white and 254 shades of gray
//!
//! Optionally, grayscale images allow for 1 fully transparent color, which is removed from
//! the fully-opaque colors above (e.g. a 2 bit grayscale image can have black, white, dark_gray
//! and a transparent color).
//!
//! The Basalt Platform provides for 2-bits per color channel, so images are optimized by the
//! SDK tooling when loaded as a resource-type "png" to the Pebble's 64-colors with 4 levels
//! of transparency.  This optimization also handles mapping unsupported colors to the nearest
//! supported color, and reducing the pixel depth to the number of bits required to support
//! the optimized number of colors.  PNG8 images from other sources are supported, with the colors
//! truncated to match supported colors at runtime.
//!
//! @see \ref gbitmap_create_from_png_data
//! @see \ref gbitmap_create_with_resource
//!
//! @{

//! @} // group PNGFileFormat

//! @addtogroup PBIFileFormat PBI File Format
//!
//! PBIs are uncompressed bitmap images with support for color-mapping palettes.
//! PBIs store images either as raw image pixels (1-bit black and white, or 8-bit ARGB) or as
//! palette-based images with 1, 2, or 4 bits per pixel.
//! For palette-based images the pixel data represents the index into the palette, such
//! that each pixel only needs to be large enough to represent the palette size, so
//! \li \c 1-bit supports up to 2 colors,
//! \li \c 2-bit supports up to 4 colors,
//! \li \c 4-bit supports up to 16 colors.
//!
//! The metadata describes how long each row of pixels is in the buffer (the stride).
//! The following restrictions on stride are in place for different formats:
//!
//! - \ref GBitmapFormat1Bit:
//!   Each row must be a multiple of 32 pixels (4 bytes). Using the `bounds` field,
//!   the area that is actually relevant can be specified.
//!   For example, when the image is 29 by 5 pixels
//!   (width by height) and the first bit of image data is the pixel at (0, 0),
//!   then the bounds.size would be `GSize(29, 5)` and bounds.origin would be `GPoint(0, 0)`.
//!   ![](gbitmap.png)
//!   In the illustration each pixel is a representated as a square. The white
//!   squares are the bits that are used, the gray squares are the padding bits, because
//!   each row of image data has to be a multiple of 4 bytes (32 bits).
//!   The numbers in the column in the left are the offsets (in bytes) from the `*addr`
//!   field of the GBitmap.
//!   Each pixel in a bitmap is represented by 1 bit. If a bit is set (`1` or `true`),
//!   it will result in a white pixel, and vice versa, if a bit is cleared (`0` or `false`),
//!   it will result in a black pixel.
//!   ![](pixel_bit_values.png)
//!
//! - \ref GBitmapFormat8Bit:
//!   Each pixel in the bitmap is represented by 1 byte. The color value of that byte correspends to
//!   a GColor.argb value.
//!   There is no restriction on row_size_bytes / stride.
//!
//! - \ref GBitmapFormat1BitPalette, \ref GBitmapFormat2BitPalette, \ref GBitmapFormat4BitPalette:
//!   Each pixel in the bitmap is represented by the number of bits the format specifies. Pixels
//!   must be packed.
//!   For example, in GBitmapFormat2BitPalette, each pixel uses 2 bits. This means 4 pixels / byte.
//!   Rows need to be byte-aligned, meaning that there can be up to 3 unused pixels at the end of
//!   each line. If the image is 5 pixels wide and 4 pixels tall, row_size_bytes = 2,
//!   and each row in the bitmap must take 2 bytes, so the bitmap data is 8 bytes in total.
//!
//! Palettized bitmaps also need to have a palette. The palette must be of the correct size, which
//! is specified by the format. For example, \ref GBitmapFormat4BitPalette uses 4 bits per pixel,
//! meaning that there must be 2^4 = 16 colors in the palette.
//!
//! The Basalt Platform provides for 2-bits per color channel, so images are optimized by the
//! SDK tooling when loaded as a resource-type "pbi" to the Pebble's 64-colors with 4 levels
//! of transparency.  This optimization also handles mapping unsupported colors to the nearest
//! supported color, and reducing the pixel depth to the number of bits required to support
//! the optimized number of colors.
//!
//! @see \ref gbitmap_create_with_data
//! @see \ref gbitmap_create_with_resource
//!
//! @{

//! @} // group PBIFileFormat

//! @} // group FileFormats

//! Opaque reference to a resource.
//! @see \ref resource_get_handle()
typedef void * ResHandle;

#define RESOURCE_ID_FONT_FALLBACK RESOURCE_ID_GOTHIC_14

//! Gets the resource handle for a file identifier.
//! @param resource_id The resource ID
//!
//! The resource IDs are auto-generated by the Pebble build process, based
//! on the `appinfo.json`. The "name" field of each resource is prefixed
//! by `RESOURCE_ID_` and made visible to the application (through the
//! `build/src/resource_ids.auto.h` header which is automatically included).
//!
//! For example, given the following fragment of `appinfo.json`:
//! \code{.json}
//!   ...
//!   "resources" : {
//!     "media": [
//!        {
//!           "name": "MY_ICON",
//!           "file": "img/icon.png",
//!           "type": "png",
//!        },
//!    ...
//! \endcode
//! The generated file identifier for this resource is `RESOURCE_ID_MY_ICON`.
//! To get a resource handle for that resource write:
//! \code{.c}
//! ResHandle rh = resource_get_handle(RESOURCE_ID_MY_ICON);
//! \endcode
ResHandle resource_get_handle(uint32_t resource_id);

//! Gets the size of the resource given a resource handle.
//! @param h The handle to the resource
//! @return The size of the resource in bytes
size_t resource_size(ResHandle h);

//! Copies the bytes for the resource with a given handle from flash storage into a given buffer.
//! @param h The handle to the resource
//! @param buffer The buffer to load the resource data into
//! @param max_length The maximum number of bytes to copy
//! @return The number of bytes actually copied
size_t resource_load(ResHandle h, uint8_t *buffer, size_t max_length);

//! Copies a range of bytes from a resource with a given handle into a given buffer.
//! @param h The handle to the resource
//! @param start_offset The offset in bytes at which to start reading from the resource
//! @param buffer The buffer to load the resource data into
//! @param num_bytes The maximum number of bytes to copy
//! @return The number of bytes actually copied
size_t resource_load_byte_range(
    ResHandle h, uint32_t start_offset, uint8_t *buffer, size_t num_bytes);

//! @} // group Resources

//! @addtogroup App
//! @{

//! The event loop for apps, to be used in app's main(). Will block until the app is ready to exit.
void app_event_loop(void);

//! @} // group App

//! @addtogroup AppWorker
//!   \brief Runs in the background, and can communicate with the foreground app.
//! @{

//! Possible error codes from app_worker_launch, app_worker_kill
typedef enum {
  //! Success
  APP_WORKER_RESULT_SUCCESS= 0,
  //! No worker found for the current app
  APP_WORKER_RESULT_NO_WORKER = 1,
  //! A worker for a different app is already running
  APP_WORKER_RESULT_DIFFERENT_APP = 2,
  //! The worker is not running
  APP_WORKER_RESULT_NOT_RUNNING = 3,
  //! The worker is already running
  APP_WORKER_RESULT_ALREADY_RUNNING = 4,
  //! The user will be asked for confirmation
  APP_WORKER_RESULT_ASKING_CONFIRMATION = 5,
} AppWorkerResult;

//! Generic structure of a worker message that can be sent between an app and its worker
typedef struct {
  uint16_t data0;
  uint16_t data1;
  uint16_t data2;
} AppWorkerMessage;

//! Determine if the worker for the current app is running
//! @return true if running
bool app_worker_is_running(void);

//! Launch the worker for the current app. Note that this is an asynchronous operation, a result code
//! of APP_WORKER_RESULT_SUCCESS merely means that the request was successfully queued up.
//! @return result code
AppWorkerResult app_worker_launch(void);

//! Kill the worker for the current app. Note that this is an asynchronous operation, a result code
//! of APP_WORKER_RESULT_SUCCESS merely means that the request was successfully queued up.
//! @return result code
AppWorkerResult app_worker_kill(void);

//! Callback type for worker messages. Messages can be sent from worker to app or vice versa.
//! @param type An application defined message type
//! @param data pointer to message data. The receiver must know the structure of the data provided by the sender.
typedef void (*AppWorkerMessageHandler)(uint16_t type, AppWorkerMessage *data);

//! Subscribe to worker messages. Once subscribed, the handler gets called on every message emitted by the other task
//! (either worker or app).
//! @param handler A callback to be executed when the event is received
//! @return true on success
bool app_worker_message_subscribe(AppWorkerMessageHandler handler);

//! Unsubscribe from worker messages. Once unsubscribed, the previously registered handler will no longer be called.
//! @return true on success
bool app_worker_message_unsubscribe(void);

//! Send a message to the other task (either worker or app).
//! @param type An application defined message type
//! @param data the message data structure
void app_worker_send_message(uint8_t type, AppWorkerMessage *data);

//! @} // group AppWorker

//! @addtogroup AppComm App Communication
//!   \brief API for interacting with the Pebble communication subsystem.
//!
//! @note To send messages to a remote device, see the \ref AppMessage or
//! \ref AppSync modules.
//! @{

//! Intervals during which the Bluetooth module may enter a low power mode.
//! The sniff interval defines the period during which the Bluetooth module may
//! not exchange (ACL) packets. The longer the sniff interval, the more time the
//! Bluetooth module may spend in a low power mode.
//! It may be necessary to reduce the sniff interval if an app requires reduced
//! latency when sending messages.
//! @note These settings have a dramatic effect on the Pebble's energy
//! consumption. Use the normal sniff interval whenever possible.
//! Note, however, that switching between modes increases power consumption
//! during the process. Frequent switching between modes is thus
//! discouraged. Ensure you do not drop to normal frequently. The Bluetooth module
//! is a major consumer of the Pebble's energy.
typedef enum {
  //! Set the sniff interval to normal (power-saving) mode
  SNIFF_INTERVAL_NORMAL = 0,
  //! Reduce the sniff interval to increase the responsiveness of the radio at
  //! the expense of increasing Bluetooth energy consumption by a multiple of 2-5
  //! (very significant)
  SNIFF_INTERVAL_REDUCED = 1,
} SniffInterval;

//! Set the Bluetooth module's sniff interval.
//! The sniff interval will be restored to normal by the OS after the app's
//! de-init handler is called. Set the sniff interval to normal whenever
//! possible.
void app_comm_set_sniff_interval(const SniffInterval interval);

//! Get the Bluetooth module's sniff interval
//! @return The SniffInterval value corresponding to the current interval
SniffInterval app_comm_get_sniff_interval(void);

//! @} // group AppComm

//! @addtogroup Timer
//!   \brief Can be used to execute some code at some point in the future.
//! @{

//! Waits for a certain amount of milliseconds
//! @param millis The number of milliseconds to wait for
void psleep(int millis);

struct AppTimer;
typedef struct AppTimer AppTimer;

//! The type of function which can be called when a timer fires.  The argument will be the @p callback_data passed to
//! @ref app_timer_register().
typedef void (*AppTimerCallback)(void* data);

//! Registers a timer that ends up in callback being called some specified time in the future.
//! @param timeout_ms The expiry time in milliseconds from the current time
//! @param callback The callback that gets called at expiry time
//! @param callback_data The data that will be passed to callback
//! @return A pointer to an `AppTimer` that can be used to later reschedule or cancel this timer
AppTimer* app_timer_register(uint32_t timeout_ms, AppTimerCallback callback, void* callback_data);

//! Reschedules an already running timer for some point in the future.
//! @param timer_handle The timer to reschedule
//! @param new_timeout_ms The new expiry time in milliseconds from the current time
//! @return true if the timer was rescheduled, false if the timer has already elapsed
bool app_timer_reschedule(AppTimer *timer_handle, uint32_t new_timeout_ms);

//! Cancels an already registered timer.
//! Once cancelled the handle may no longer be used for any purpose.
void app_timer_cancel(AppTimer *timer_handle);

//! @} // group Timer

//! @addtogroup MemoryManagement Memory Management
//!   \brief Utility functions for managing an application's memory.
//!
//! @{

//! Calculates the number of bytes of heap memory \a not currently being used by the application.
//! @return The number of bytes on the heap not currently being used.
size_t heap_bytes_free(void);

//! Calculates the number of bytes of heap memory currently being used by the application.
//! @return The number of bytes on the heap currently being used.
size_t heap_bytes_used(void);

//! Flushes the data cache and invalidates the instruction cache for the given region of memory,
//! if necessary. This is only required when your app is loading or modifying code in memory and
//! intends to execute it. On some platforms, code executed may be cached internally to improve
//! performance. After writing to memory, but before executing, this function must be called in
//! order to avoid undefined behavior. On platforms without caching, this performs no operation.
//! @param start The beginning of the buffer to flush
//! @param size How many bytes to flush
#define memory_cache_flush(start, size) do {} while (0)

//! @} // group MemoryManagement

//! @addtogroup Storage
//! \brief A mechanism to store persistent application data and state
//!
//! The Persistent Storage API provides you with a mechanism for performing a variety of tasks,
//! like saving user settings, caching data from the phone app, or counting high scores for
//! Pebble watchapp games.
//!
//! In Pebble OS, storage is defined by a collection of fields that you can create, modify or delete.
//! In the API, a field is specified as a key with a corresponding value.
//!
//! Using the Storage API, every app is able to get its own persistent storage space. Each value
//! in that space is associated with a uint32_t key.
//!
//! Storage supports saving integers, strings and byte arrays. The maximum size of byte arrays and
//! strings is defined by PERSIST_DATA_MAX_LENGTH (currently set to 256 bytes). You call the function
//! persist_exists(key), which returns a boolean indicating if the key exists or not.
//! The Storage API enables your app to save its state, and when compared to using \ref AppMessage to
//! retrieve values from the phone, it provides you with a much faster way to restore state.
//! In addition, it draws less power from the battery.
//!
//! Note that the size of all persisted values cannot exceed 4K per app.
//! @{

//! The maximum size of a persist value in bytes
#define PERSIST_DATA_MAX_LENGTH 256

//! The maximum size of a persist string in bytes including the NULL terminator
#define PERSIST_STRING_MAX_LENGTH PERSIST_DATA_MAX_LENGTH

//! Status codes. See \ref status_t
typedef enum StatusCode {
  //! Operation completed successfully.
  S_SUCCESS = 0,

  //! An error occurred (no description).
  E_ERROR = -1,

  //! No idea what went wrong.
  E_UNKNOWN = -2,

  //! There was a generic internal logic error.
  E_INTERNAL = -3,

  //! The function was not called correctly.
  E_INVALID_ARGUMENT = -4,

  //! Insufficient allocatable memory available.
  E_OUT_OF_MEMORY = -5,

  //! Insufficient long-term storage available.
  E_OUT_OF_STORAGE = -6,

  //! Insufficient resources available.
  E_OUT_OF_RESOURCES = -7,

  //! Argument out of range (may be dynamic).
  E_RANGE = -8,

  //! Target of operation does not exist.
  E_DOES_NOT_EXIST = -9,

  //! Operation not allowed (may depend on state).
  E_INVALID_OPERATION = -10,

  //! Another operation prevented this one.
  E_BUSY = -11,

  //! Operation not completed; try again.
  E_AGAIN = -12,

  //! Equivalent of boolean true.
  S_TRUE = 1,

  //! Equivalent of boolean false.
  S_FALSE = 0,

  //! For list-style requests.  At end of list.
  S_NO_MORE_ITEMS = 2,

  //! No action was taken as none was required.
  S_NO_ACTION_REQUIRED = 3,

} StatusCode;

//! Return value for system operations. See \ref StatusCode for possible values.
typedef int32_t status_t;

//! Checks whether a value has been set for a given key in persistent storage.
//! @param key The key of the field to check.
//! @return true if a value exists, otherwise false.
bool persist_exists(const uint32_t key);

//! Gets the size of a value for a given key in persistent storage.
//! @param key The key of the field to lookup the data size.
//! @return The size of the value in bytes or \ref E_DOES_NOT_EXIST
//! if there is no field matching the given key.
int persist_get_size(const uint32_t key);

//! Reads a bool value for a given key from persistent storage.
//! If the value has not yet been set, this will return false.
//! @param key The key of the field to read from.
//! @return The bool value of the key to read from.
bool persist_read_bool(const uint32_t key);

//! Reads an int value for a given key from persistent storage.
//! @note The int is a signed 32-bit integer.
//! If the value has not yet been set, this will return 0.
//! @param key The key of the field to read from.
//! @return The int value of the key to read from.
int32_t persist_read_int(const uint32_t key);

//! Reads a blob of data for a given key from persistent storage into a given buffer.
//! If the value has not yet been set, the given buffer is left unchanged.
//! @param key The key of the field to read from.
//! @param buffer The pointer to a buffer to be written to.
//! @param buffer_size The maximum size of the given buffer.
//! @return The number of bytes written into the buffer or \ref E_DOES_NOT_EXIST
//! if there is no field matching the given key.
int persist_read_data(const uint32_t key, void *buffer, const size_t buffer_size);

//! Reads a string for a given key from persistent storage into a given buffer.
//! The string will be null terminated.
//! If the value has not yet been set, the given buffer is left unchanged.
//! @param key The key of the field to read from.
//! @param buffer The pointer to a buffer to be written to.
//! @param buffer_size The maximum size of the given buffer. This includes the null character.
//! @return The number of bytes written into the buffer or \ref E_DOES_NOT_EXIST
//! if there is no field matching the given key.
int persist_read_string(const uint32_t key, char *buffer, const size_t buffer_size);

//! Writes a bool value flag for a given key into persistent storage.
//! @param key The key of the field to write to.
//! @param value The boolean value to write.
//! @return The number of bytes written if successful, a value from \ref StatusCode otherwise.
status_t persist_write_bool(const uint32_t key, const bool value);

//! Writes an int value for a given key into persistent storage.
//! @note The int is a signed 32-bit integer.
//! @param key The key of the field to write to.
//! @param value The int value to write.
//! @return The number of bytes written if successful, a value from \ref StatusCode otherwise.
status_t persist_write_int(const uint32_t key, const int32_t value);

//! Writes a blob of data of a specified size in bytes for a given key into persistent storage.
//! The maximum size is \ref PERSIST_DATA_MAX_LENGTH
//! @param key The key of the field to write to.
//! @param data The pointer to the blob of data.
//! @param size The size in bytes.
//! @return The number of bytes written if successful, a value from \ref StatusCode otherwise.
int persist_write_data(const uint32_t key, const void *data, const size_t size);

//! Writes a string a given key into persistent storage.
//! The maximum size is \ref PERSIST_STRING_MAX_LENGTH including the null terminator.
//! @param key The key of the field to write to.
//! @param cstring The pointer to null terminated string.
//! @return The number of bytes written if successful, a value from \ref StatusCode otherwise.
int persist_write_string(const uint32_t key, const char *cstring);

//! Deletes the value of a key from persistent storage.
//! @param key The key of the field to delete from.
status_t persist_delete(const uint32_t key);

//! @} // group Storage

//! @addtogroup Wakeup
//!   \brief Allows applications to schedule to be launched even if they are not running.
//! @{

//! WakeupId is an identifier for a wakeup event
typedef int32_t WakeupId;

//! The type of function which can be called when a wakeup event occurs.  
//! The arguments will be the id of the wakeup event that occurred, 
//! as well as the scheduled cookie provided to \ref wakeup_schedule.
typedef void (*WakeupHandler)(WakeupId wakeup_id, int32_t cookie);

//! Registers a WakeupHandler to be called when wakeup events occur.
//! @param handler The callback that gets called when the wakeup event occurs
void wakeup_service_subscribe(WakeupHandler handler);

//! Registers a wakeup event that triggers a callback at the specified time.
//! Applications may only schedule up to 8 wakeup events.
//! Wakeup events are given a 1 minute duration window, in that no application may schedule a 
//! wakeup event with 1 minute of a currently scheduled wakeup event.
//! @param timestamp The requested time (UTC) for the wakeup event to occur
//! @param cookie The application specific reason for the wakeup event
//! @param notify_if_missed On powering on Pebble, will alert user when 
//! notifications were missed due to Pebble being off.
//! @return negative values indicate errors (StatusCode)
//! E_RANGE if the event cannot be scheduled due to another event in that period.
//! E_INVALID_ARGUMENT if the time requested is in the past.
//! E_OUT_OF_RESOURCES if the application has already scheduled all 8 wakeup events.
//! E_INTERNAL if a system error occurred during scheduling.
WakeupId wakeup_schedule(time_t timestamp, int32_t cookie, bool notify_if_missed);

//! Cancels a wakeup event.
//! @param wakeup_id Wakeup event to cancel
void wakeup_cancel(WakeupId wakeup_id);

//! Cancels all wakeup event for the app.
void wakeup_cancel_all(void);

//! Retrieves the wakeup event info for an app that was launched
//! by a wakeup_event (ie. \ref launch_reason() === APP_LAUNCH_WAKEUP)
//! so that an app may display information regarding the wakeup event
//! @param wakeup_id WakeupId for the wakeup event that caused the app to wakeup
//! @param cookie App provided reason for the wakeup event
//! @return True if app was launched due to a wakeup event, false otherwise
bool wakeup_get_launch_event(WakeupId *wakeup_id, int32_t *cookie);

//! Checks if the current WakeupId is still scheduled and therefore valid
//! @param wakeup_id Wakeup event to query for validity and scheduled time
//! @param timestamp Optionally points to an address of a time_t variable to
//! store the time that the wakeup event is scheduled to occur.
//! (The time is in UTC, but local time when \ref clock_is_timezone_set
//! returns false).
//! You may pass NULL instead if you do not need it.
//! @return True if WakeupId is still scheduled, false if it doesn't exist or has
//! already occurred
bool wakeup_query(WakeupId wakeup_id, time_t *timestamp);

//! @} // group Wakeup

//! @addtogroup LaunchReason Launch Reason
//!   \brief API for checking what caused the application to launch.
//!
//!   This includes the system, launch by user interaction (User selects the
//!   application from the launcher menu),
//!   launch by the mobile or a mobile companion application,
//!   or launch by a scheduled wakeup event for the specified application.
//!
//! @{

//! AppLaunchReason is used to inform the application about how it was launched
//! @note New launch reasons may be added in the future. As a best practice, it
//! is recommended to only handle the cases that the app needs to know about,
//! rather than trying to handle all possible launch reasons.
typedef enum {
  APP_LAUNCH_SYSTEM = 0,  //!< App launched by the system
  APP_LAUNCH_USER,        //!< App launched by user selection in launcher menu
  APP_LAUNCH_PHONE,       //!< App launched by mobile or companion app
  APP_LAUNCH_WAKEUP,      //!< App launched by wakeup event
  APP_LAUNCH_WORKER,      //!< App launched by worker calling worker_launch_app()
  APP_LAUNCH_QUICK_LAUNCH, //!< App launched by user using quick launch
  APP_LAUNCH_TIMELINE_ACTION,  //!< App launched by user opening it from a pin
} AppLaunchReason;

//! Provides the method used to launch the current application.
//! @return The method or reason the current application was launched
AppLaunchReason launch_reason(void);

//! Get the argument passed to the app when it was launched.
//! @note Currently the only way to pass arguments to apps is by using an openWatchApp action
//! on a pin.
//! @return The argument passed to the app, or 0 if the app wasn't launched from a Launch App action
uint32_t launch_get_args(void);

//! @} // group LaunchReason

//! @addtogroup ExitReason Exit Reason
//!   \brief API for the application to notify the system of the reason it will exit.
//!
//!   If the application has not specified an exit reason before it exits, then the exit reason will
//!   default to APP_EXIT_NOT_SPECIFIED.
//!
//!   Only an application can set its exit reason. The system will not modify it.
//!
//! @{

//! AppExitReason is used to notify the system of the reason of an application exiting, which may
//! affect the part of the system UI that is presented after the application terminates.
typedef enum AppExitReason {
  APP_EXIT_NOT_SPECIFIED = 0,                    //!< Exit reason not specified
  APP_EXIT_ACTION_PERFORMED_SUCCESSFULLY,        //!< Application performed an action when it exited

  NUM_EXIT_REASONS                               //!< Number of AppExitReason options
} AppExitReason;

//! Set the app exit reason to a new reason.
//! @param reason The new app exit reason
#define exit_reason_set(exit_reason) do {} while (0)

//! @} // group ExitReason

//! @addtogroup AppGlance App Glance
//!   \brief API for the application to modify its glance.
//!
//! @{

//! The ID of a published app resource defined within the publishedMedia section of package.json.
typedef uint32_t PublishedId;

//! An app's glance can change over time as defined by zero or more app glance slices that each
//! describe the state of the app glance at a particular point in time. Slices are displayed in the
//! order they are added, and they are removed at the specified expiration time.
typedef struct AppGlanceSlice {
  //! Describes how the slice should be visualized in the app's glance in the launcher.
  struct {
    //! The published resource ID of the bitmap icon to display in the app's glance. Use \ref
    //! APP_GLANCE_SLICE_DEFAULT_ICON to use the app's default bitmap icon.
    PublishedId icon;
    //! A template string to visualize in the app's glance. The string will be copied, so it is safe
    //! to destroy after adding the slice to the glance. Use NULL if no string should be displayed.
    const char *subtitle_template_string;
  } layout;
  //! The UTC time after which this slice should no longer be shown in the app's glance. Use \ref
  //! APP_GLANCE_SLICE_NO_EXPIRATION if the slice should never expire.
  time_t expiration_time;
} AppGlanceSlice;

//! Bitfield enum describing the result of trying to add an AppGlanceSlice to an app's glance.
typedef enum AppGlanceResult {
  //! The slice was successfully added to the app's glance.
  APP_GLANCE_RESULT_SUCCESS = 0,
  //! The subtitle_template_string provided in the slice was invalid.
  APP_GLANCE_RESULT_INVALID_TEMPLATE_STRING = 1 << 0,
  //! The subtitle_template_string provided in the slice was longer than 150 bytes.
  APP_GLANCE_RESULT_TEMPLATE_STRING_TOO_LONG = 1 << 1,
  //! The icon provided in the slice was invalid.
  APP_GLANCE_RESULT_INVALID_ICON = 1 << 2,
  //! The provided slice would exceed the app glance's slice capacity.
  APP_GLANCE_RESULT_SLICE_CAPACITY_EXCEEDED = 1 << 3,
  //! The expiration_time provided in the slice expires in the past.
  APP_GLANCE_RESULT_EXPIRES_IN_THE_PAST = 1 << 4,
  //! The \ref AppGlanceReloadSession provided was invalid.
  APP_GLANCE_RESULT_INVALID_SESSION = 1 << 5,
} AppGlanceResult;

struct AppGlanceReloadSession;
typedef struct AppGlanceReloadSession AppGlanceReloadSession;

//! Add a slice to the app's glance. This function will only succeed if called with a valid
//! \ref AppGlanceReloadSession that is provided in an \ref AppGlanceReloadCallback, which
//! cannot happen on SDK 3, so it will always return APP_GLANCE_RESULT_INVALID_SESSION.
//! @param session The session variable provided in an \ref AppGlanceReloadCallback
//! @param slice The slice to add to the app's glance
//! @return The result of trying to add the slice to the app's glance
#define app_glance_add_slice(session, slice) (APP_GLANCE_RESULT_INVALID_SESSION)

//! User-provided callback for reloading the slices in the app's glance.
//! @param session A session variable that must be passed to \ref app_glance_add_slice when adding
//! slices to the app's glance; it becomes invalid when the \ref AppGlanceReloadCallback returns
//! @param limit The number of entries that can be added to the app's glance
//! @param context User-provided context provided when calling \ref app_glance_reload()
typedef void (*AppGlanceReloadCallback)(AppGlanceReloadSession *session, size_t limit,
                                        void *context);

//! Clear any existing slices in the app's glance and trigger a reload via the provided callback.
//! Does nothing on SDK 3.
//! @param callback A function that will be called to add new slices to the app's glance; even if
//! the provided callback is NULL, any existing slices will still be cleared from the app's glance
//! @param context User-provided context that will be passed to the callback
#define app_glance_reload(callback, context) do {} while (0)

//! Can be used for the expiration_time of an \ref AppGlanceSlice so that the slice never expires.
#define APP_GLANCE_SLICE_NO_EXPIRATION ((time_t)0)

//! Can be used for the icon of an \ref AppGlanceSlice so that the slice displays the app's default
//! icon.
#define APP_GLANCE_SLICE_DEFAULT_ICON ((PublishedId)0)

//! @} // group AppGlance

//! @} // group Foundation

//! @addtogroup Graphics
//! @{

//! @addtogroup GraphicsTypes Graphics Types
//! \brief Basic graphics types (point, rect, size, color, bitmaps, etc.) and utility functions.
//!
//! @{

typedef union GColor8 {
  uint8_t argb;
  struct {
    uint8_t b:2; //!< Blue
    uint8_t g:2; //!< Green
    uint8_t r:2; //!< Red
    uint8_t a:2; //!< Alpha. 3 = 100% opaque, 2 = 66% opaque, 1 = 33% opaque, 0 = transparent.
  };
} GColor8;

typedef GColor8 GColor;

#include "gcolor_definitions.h"

//! Comparison function for GColors.
bool gcolor_equal(GColor8 x, GColor8 y);

//! This method assists in improving the legibility of text on various background colors.
//! It takes the background color for the region in question and computes a color for
//! maximum legibility.
//! @param background_color Background color for the region in question
//! @return A legible color for the given background color
GColor8 gcolor_legible_over(GColor8 background_color);

//! Convenience macro allowing use of a fallback color for black and white platforms.
//! On color platforms, the first expression will be chosen, the second otherwise.
#define COLOR_FALLBACK(color, bw) (bw)

//! Convenience macro to switch between two expression depending on the screen of the platform.
//! On platforms with rectangular screen, the first expression will be chosen, the second otherwise.
#define PBL_IF_RECT_ELSE(if_true, if_false) (if_true)

//! Convenience macro to switch between two expression depending on the screen of the platform.
//! On platforms with round screen, the first expression will be chosen, the second otherwise.
#define PBL_IF_ROUND_ELSE(if_true, if_false) (if_false)

//! Convenience macro to switch between two expression depending on the screen of the platform.
//! On black& white platforms, the first expression will be chosen, the second otherwise.
#define PBL_IF_BW_ELSE(if_true, if_false) (if_true)

//! Convenience macro to switch between two expression depending on the screen of the platform.
//! On color platforms, the first expression will be chosen, the second otherwise.
#define PBL_IF_COLOR_ELSE(if_true, if_false) (if_false)

//! Represents a point in a 2-dimensional coordinate system.
//! @note Conventionally, the origin of Pebble's 2D coordinate system is in the upper,
//! lefthand corner
//! its x-axis extends to the right and its y-axis extends to the bottom of the screen.
typedef struct GPoint {
  //! The x-coordinate.
  int16_t x;
  //! The y-coordinate.
  int16_t y;
} GPoint;

//! Convenience macro to make a GPoint.
#define GPoint(x, y) ((GPoint){(x), (y)})

//! Convenience macro to make a GPoint at (0, 0).
#define GPointZero GPoint(0, 0)

//! Tests whether 2 points are equal.
//! @param point_a Pointer to the first point
//! @param point_b Pointer to the second point
//! @return `true` if both points are equal, `false` if not.
bool gpoint_equal(const GPoint * const point_a, const GPoint * const point_b);

//! Represents a 2-dimensional size.
typedef struct GSize {
  //! The width
  int16_t w;
  //! The height
  int16_t h;
} GSize;

//! Convenience macro to make a GSize.
#define GSize(w, h) ((GSize){(w), (h)})

//! Convenience macro to make a GSize of (0, 0).
#define GSizeZero GSize(0, 0)

//! Tests whether 2 sizes are equal.
//! @param size_a Pointer to the first size
//! @param size_b Pointer to the second size
//! @return `true` if both sizes are equal, `false` if not.
bool gsize_equal(const GSize *size_a, const GSize *size_b);

//! Represents a rectangle and defining it using the origin of
//! the upper-lefthand corner and its size.
typedef struct GRect {
  //! The coordinate of the upper-lefthand corner point of the rectangle.
  GPoint origin;
  //! The size of the rectangle.
  GSize size;
} GRect;

//! Convenience macro to make a GRect
#define GRect(x, y, w, h) ((GRect){{(x), (y)}, {(w), (h)}})

//! Convenience macro to make a GRect of ((0, 0), (0, 0)).
#define GRectZero GRect(0, 0, 0, 0)

//! Tests whether 2 rectangles are equal.
//! @param rect_a Pointer to the first rectangle
//! @param rect_b Pointer to the second rectangle
//! @return `true` if both rectangles are equal, `false` if not.
bool grect_equal(const GRect* const rect_a, const GRect* const rect_b);

//! Tests whether the size of the rectangle is (0, 0).
//! @param rect Pointer to the rectangle
//! @return `true` if the rectangle its size is (0, 0), or `false` if not.
//! @note If the width and/or height of a rectangle is negative, this
//! function will return `true`!
bool grect_is_empty(const GRect* const rect);

//! Converts a rectangle's values so that the components of its size
//! (width and/or height) are both positive. In the width and/or height are negative,
//! the origin will offset, so that the final rectangle overlaps with the original.
//! For example, a GRect with size (-10, -5) and origin (20, 20), will be standardized
//! to size (10, 5) and origin (10, 15).
//! @param[in] rect The rectangle to convert.
//! @param[out] rect The standardized rectangle.
void grect_standardize(GRect *rect);

//! Trim one rectangle using the edges of a second rectangle.
//! @param[in] rect_to_clip The rectangle that needs to be clipped (in place).
//! @param[out] rect_to_clip The clipped rectangle.
//! @param rect_clipper The rectangle of which the edges will serve as "scissors"
//! in order to trim `rect_to_clip`.
void grect_clip(GRect * const rect_to_clip, const GRect * const rect_clipper);

//! Tests whether a rectangle contains a point.
//! @param rect The rectangle
//! @param point The point
//! @return `true` if the rectangle contains the point, or `false` if it does not.
bool grect_contains_point(const GRect *rect, const GPoint *point);

//! Convenience function to compute the center-point of a given rectangle.
//! This is equal to `(rect->x + rect->width / 2, rect->y + rect->height / 2)`.
//! @param rect The rectangle for which to calculate the center point.
//! @return The point at the center of `rect`
GPoint grect_center_point(const GRect *rect);

//! Reduce the width and height of a rectangle by insetting each of the edges with
//! a fixed inset. The returned rectangle will be centered relative to the input rectangle.
//! @note The function will trip an assertion if the crop yields a rectangle with negative width or height.
//! @param rect The rectangle that will be inset
//! @param crop_size_px The inset by which each of the rectangle will be inset.
//! A positive inset value results in a smaller rectangle, while negative inset value results
//! in a larger rectangle.
//! @return The cropped rectangle.
GRect grect_crop(GRect rect, const int32_t crop_size_px);

//! Repeat Sequence or animation indefinitely.
#define PLAY_COUNT_INFINITE UINT32_MAX

//! Duration of Sequence or animation is infinite.
#define PLAY_DURATION_INFINITE UINT32_MAX

//! The format of a GBitmap can either be 1-bit or 8-bit.
typedef enum GBitmapFormat {
  GBitmapFormat1Bit = 0, //<! 1-bit black and white. 0 = black, 1 = white.
  GBitmapFormat8Bit,      //<! 6-bit color + 2 bit alpha channel. See \ref GColor8 for pixel format.
  GBitmapFormat1BitPalette,
  GBitmapFormat2BitPalette,
  GBitmapFormat4BitPalette,
  GBitmapFormat8BitCircular,
} GBitmapFormat;

struct GBitmap;
typedef struct GBitmap GBitmap;

struct GBitmapSequence;
typedef struct GBitmapSequence GBitmapSequence;

//! Get the number of bytes per row in the bitmap data for the given \ref GBitmap.
//! On rectangular displays, this can be used as a safe way of iterating over the rows in the
//! bitmap, since bytes per row should be set according to format. On circular displays with pixel
//! format of \ref GBitmapFormat8BitCircular this will return 0, and should not be used for
//! iteration over frame buffer pixels. Instead, use \ref GBitmapDataRowInfo, which provides safe
//! minimum and maximum x values for a given row's y value.
//! @param bitmap A pointer to the GBitmap to get the bytes per row
//! @return The number of bytes per row of the GBitmap
//! @see \ref gbitmap_get_data
uint16_t gbitmap_get_bytes_per_row(const GBitmap *bitmap);

//! Get the \ref GBitmapFormat for the \ref GBitmap.
//! @param bitmap A pointer to the GBitmap to get the format
//! @return The format of the given \ref GBitmap.
GBitmapFormat gbitmap_get_format(const GBitmap *bitmap);

//! Get a pointer to the raw image data section of the given \ref GBitmap as specified by the format
//! of the bitmap.
//! @param bitmap A pointer to the GBitmap to get the data
//! @return pointer to the raw image data for the GBitmap
//! @see \ref gbitmap_get_bytes_per_row
//! @see \ref GBitmap
uint8_t* gbitmap_get_data(const GBitmap *bitmap);

//! Set the bitmap data for the given \ref GBitmap.
//! @param bitmap A pointer to the GBitmap to set data to
//! @param data A pointer to the bitmap data
//! @param format the format of the bitmap data. If this is a palettized format, make sure that
//! there is an accompanying call to \ref gbitmap_set_palette.
//! @param row_size_bytes How many bytes a single row takes. For example, bitmap data of format
//! \ref GBitmapFormat1Bit must have a row size as a multiple of 4 bytes.
//! @param free_on_destroy Set whether the data should be freed when the GBitmap is destroyed.
//! @see \ref gbitmap_destroy
void gbitmap_set_data(GBitmap *bitmap, uint8_t *data, GBitmapFormat format,
                     uint16_t row_size_bytes, bool free_on_destroy);

//! Gets the bounds of the content for the \ref GBitmap. This is set when loading the image or
//! if changed by \ref gbitmap_set_bounds.
//! @param bitmap A pointer to the GBitmap to get the bounding box from.
//! @return The bounding box for the GBitmap.
//! @see \ref gbitmap_set_bounds
GRect gbitmap_get_bounds(const GBitmap *bitmap);

//! Set the bounds of the given \ref GBitmap.
//! @param bitmap A pointer to the GBitmap to set the bounding box.
//! @param bounds The bounding box to set.
//! @see \ref gbitmap_get_bounds
void gbitmap_set_bounds(GBitmap *bitmap, GRect bounds);

//! Get the palette for the given \ref GBitmap.
//! @param bitmap A pointer to the GBitmap to get the palette from.
//! @return Pointer to a \ref GColor array containing the palette colors.
//! @see \ref gbitmap_set_palette
GColor* gbitmap_get_palette(const GBitmap *bitmap);

//! Set the palette for the given \ref GBitmap.
//! @param bitmap A pointer to the GBitmap to set the palette to
//! @param palette The palette to be used. Make sure that the palette is large enough for the
//! bitmap's format.
//! @param free_on_destroy Set whether the palette data should be freed when the GBitmap is
//! destroyed or when another palette is set.
//! @see \ref gbitmap_get_format
//! @see \ref gbitmap_destroy
//! @see \ref gbitmap_set_palette
void gbitmap_set_palette(GBitmap *bitmap, GColor *palette, bool free_on_destroy);

//! Creates a new \ref GBitmap on the heap using a Pebble image file stored as a resource.
//! The resulting GBitmap must be destroyed using \ref gbitmap_destroy().
//! @param resource_id The ID of the bitmap resource to load
//! @return A pointer to the \ref GBitmap. `NULL` if the GBitmap could not
//! be created
GBitmap* gbitmap_create_with_resource(uint32_t resource_id);

//! Creates a new GBitmap on the heap initialized with the provided Pebble image data.
//!
//! The resulting \ref GBitmap must be destroyed using \ref gbitmap_destroy() but the image
//! data will not be freed automatically. The developer is responsible for keeping the image
//! data in memory as long as the bitmap is used and releasing it after the bitmap is destroyed.
//! @note One way to generate Pebble image data is to use bitmapgen.py in the Pebble
//! SDK to generate a .pbi file.
//! @param data The Pebble image data. Must not be NULL. The function
//! assumes the data to be correct; there are no sanity checks performed on the
//! data. The data will not be copied and the pointer must remain valid for the
//! lifetime of this GBitmap.
//! @return A pointer to the \ref GBitmap. `NULL` if the \ref GBitmap could not
//! be created
GBitmap* gbitmap_create_with_data(const uint8_t *data);

//! Create a new \ref GBitmap on the heap as a sub-bitmap of a 'base' \ref
//! GBitmap, using a GRect to indicate what portion of the base to use. The
//! sub-bitmap will just reference the image data and palette of the base bitmap.
//! No deep-copying occurs as a result of calling this function, thus the caller
//! is responsible for making sure the base bitmap and palette will remain available when
//! using the sub-bitmap. Note that you should not destroy the parent bitmap until
//! the sub_bitmap has been destroyed.
//! The resulting \ref GBitmap must be destroyed using \ref gbitmap_destroy().
//! @param[in] base_bitmap The bitmap that the sub-bitmap of which the image data
//! will be used by the sub-bitmap
//! @param sub_rect The rectangle within the image data of the base bitmap. The
//! bounds of the base bitmap will be used to clip `sub_rect`.
//! @return A pointer to the \ref GBitmap. `NULL` if the GBitmap could not
//! be created
GBitmap* gbitmap_create_as_sub_bitmap(const GBitmap *base_bitmap, GRect sub_rect);

//! Create a \ref GBitmap based on raw PNG data.
//! The resulting \ref GBitmap must be destroyed using \ref gbitmap_destroy().
//! The developer is responsible for freeing png_data following this call.
//! @note PNG decoding currently supports 1,2,4 and 8 bit palettized and grayscale images.
//! @param png_data PNG image data.
//! @param png_data_size PNG image size in bytes.
//! @return A pointer to the \ref GBitmap. `NULL` if the \ref GBitmap could not
//! be created
GBitmap* gbitmap_create_from_png_data(const uint8_t *png_data, size_t png_data_size);

//! Creates a new blank GBitmap on the heap initialized to zeroes.
//! In the case that the format indicates a palettized bitmap, a palette of appropriate size will
//! also be allocated on the heap.
//! The resulting \ref GBitmap must be destroyed using \ref gbitmap_destroy().
//! @param size The Pebble image dimensions as a \ref GSize.
//! @param format The \ref GBitmapFormat the created image should be in.
//! @return A pointer to the \ref GBitmap. `NULL` if the \ref GBitmap could not
//! be created
GBitmap* gbitmap_create_blank(GSize size, GBitmapFormat format);

//! Creates a new blank GBitmap on the heap, initialized to zeroes, and assigns it the given
//! palette.
//! No deep-copying of the palette occurs, so the caller is responsible for making sure the palette
//! remains available when using the resulting bitmap. Management of that memory can be handed off
//! to the system with the free_on_destroy argument.
//! @param size The Pebble image dimensions as a \ref GSize.
//! @param format the \ref GBitmapFormat the created image and palette should be in.
//! @param palette a pointer to a palette that is to be used for this GBitmap. The palette should
//! be large enough to hold enough colors for the specified format. For example,
//! \ref GBitmapFormat2BitPalette should have 4 colors, since 2^2 = 4.
//! @param free_on_destroy Set whether the palette data should be freed along with the bitmap data
//! when the GBitmap is destroyed.
//! @return A Pointer to the \ref GBitmap. `NULL` if the \ref GBitmap could not be created.
GBitmap* gbitmap_create_blank_with_palette(GSize size, GBitmapFormat format,
                                           GColor *palette, bool free_on_destroy);

//! Given a 1-bit GBitmap, create a new bitmap of format GBitmapFormat1BitPalette.
//! The new data buffer is allocated on the heap, and a 2-color palette is allocated as well.
//! @param src_bitmap A GBitmap of format GBitmapFormat1Bit which is to be copied into a newly
//! created GBitmap of format GBitmapFormat1BitPalettized.
//! @returns The newly created 1-bit palettized GBitmap, or NULL if there is not sufficient space.
//! @note The new bitmap does not depend on any data from src_bitmap, so src_bitmap can be freed
//! without worry.
GBitmap* gbitmap_create_palettized_from_1bit(const GBitmap *src_bitmap);

//! Destroy a \ref GBitmap.
//! This must be called for every bitmap that's been created with gbitmap_create_*
//!
//! This function will also free the memory of the bitmap data (bitmap->addr) if the bitmap was created with \ref gbitmap_create_blank()
//! or \ref gbitmap_create_with_resource().
//!
//! If the GBitmap was created with \ref gbitmap_create_with_data(), you must release the memory
//! after calling gbitmap_destroy().
void gbitmap_destroy(GBitmap* bitmap);

//! Creates a GBitmapSequence from the specified resource (APNG/PNG files)
//! @param resource_id Resource to load and create GBitmapSequence from.
//! @return GBitmapSequence pointer if the resource was loaded, NULL otherwise
GBitmapSequence *gbitmap_sequence_create_with_resource(uint32_t resource_id);

//! Updates the contents of the bitmap sequence to the next frame
//! and optionally returns the delay in milliseconds until the next frame.
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @param bitmap Pointer to the initialized GBitmap in which to render the bitmap sequence
//! @param[out] delay_ms If not NULL, returns the delay in milliseconds until the next frame.
//! @return True if frame was rendered.  False if all frames (and loops) have been rendered
//! for the sequence.  Will also return false if frame could not be rendered
//! (includes out of memory errors).
//! @note GBitmap must be large enough to accommodate the bitmap_sequence image
//! \ref gbitmap_sequence_get_bitmap_size
bool gbitmap_sequence_update_bitmap_next_frame(GBitmapSequence *bitmap_sequence,
    GBitmap *bitmap, uint32_t *delay_ms);

//! Updates the contents of the bitmap sequence to the frame at elapsed in the sequence.
//! For looping animations this accounts for the loop, for example an animation of 1 second that
//! is configured to loop 2 times updated to 1500 ms elapsed time will display the sequence
//! frame at 500 ms.  Elapsed time is the time from the start of the animation, and will
//! be ignored if it is for a time earlier than the last rendered frame.
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @param bitmap Pointer to the initialized GBitmap in which to render the bitmap sequence
//! @param elapsed_ms Elapsed time in milliseconds in the sequence relative to start
//! @return True if a frame was rendered.  False if all frames (and loops) have already
//! been rendered for the sequence.  Will also return false if frame could not be rendered
//! (includes out of memory errors).
//! @note GBitmap must be large enough to accommodate the bitmap_sequence image
//! \ref gbitmap_sequence_get_bitmap_size
//! @note This function is disabled for play_count 0
bool gbitmap_sequence_update_bitmap_by_elapsed(GBitmapSequence *bitmap_sequence,
    GBitmap *bitmap, uint32_t elapsed_ms);

//! Deletes the GBitmapSequence structure and frees any allocated memory/decoder_data
//! @param bitmap_sequence Pointer to the bitmap sequence to free (delete)
void gbitmap_sequence_destroy(GBitmapSequence *bitmap_sequence);

//! Restarts the GBitmapSequence to the first frame \ref gbitmap_sequence_update_bitmap_next_frame
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @return True if sequence was restarted, false otherwise
bool gbitmap_sequence_restart(GBitmapSequence *bitmap_sequence);

//! This function gets the current frame number for the bitmap sequence
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @return index of current frame in the current loop of the bitmap sequence
int32_t gbitmap_sequence_get_current_frame_idx(GBitmapSequence *bitmap_sequence);

//! This function sets the total number of frames for the bitmap sequence
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @return number of frames contained in a single loop of the bitmap sequence
uint32_t gbitmap_sequence_get_total_num_frames(GBitmapSequence *bitmap_sequence);

//! This function gets the play count (number of times to repeat) the bitmap sequence
//! @note This value is initialized by the bitmap sequence data, and is modified by
//! \ref gbitmap_sequence_set_play_count
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @return Play count of bitmap sequence, PLAY_COUNT_INFINITE for infinite looping
uint32_t gbitmap_sequence_get_play_count(GBitmapSequence *bitmap_sequence);

//! This function sets the play count (number of times to repeat) the bitmap sequence
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @param play_count Number of times to repeat the bitmap sequence
//! with 0 disabling update_by_elapsed and update_next_frame, and
//! PLAY_COUNT_INFINITE for infinite looping of the animation
void gbitmap_sequence_set_play_count(GBitmapSequence *bitmap_sequence, uint32_t play_count);

//! This function gets the minimum required size (dimensions) necessary
//! to render the bitmap sequence to a GBitmap
//! using the /ref gbitmap_sequence_update_bitmap_next_frame
//! @param bitmap_sequence Pointer to loaded bitmap sequence
//! @return Dimensions required to render the bitmap sequence to a GBitmap
GSize gbitmap_sequence_get_bitmap_size(GBitmapSequence *bitmap_sequence);

//! Description of a single data row in the pixel data of a bitmap
//! @note This data type describes the actual pixel data of a bitmap and does not respect the
//!       bitmap's bounds.
//! @see \ref gbitmap_get_data_row_info
//! @see \ref gbitmap_get_data
//! @see \ref gbitmap_get_bounds
typedef struct {
    //! Address of the byte at column 0 of a given data row in a bitmap. Use this to calculate the
    //! memory address of a pixel. For GBitmapFormat8BitCircular or GBitmapFormat8Bit this would
    //! be: `uint8_t *pixel_addr = row_info.addr + x`.
    //! Note that this byte can be outside of the valid range for this row.
    //! For example: The first valid pixel (`min_x=76`) of a row might start at 76 bytes after the
    //! given data pointer (assuming that 1 pixel is represented as 1 byte as in
    //! GBitmapFormat8BitCircular or GBitmapFormat8Bit).
    uint8_t *data;
    //! The absolute column of a first valid pixel for a given data row.
    int16_t min_x;
    //! The absolute column of the last valid pixel for a given data row.
    //! For optimization reasons the result can be anywhere between
    //! grect_get_max_x(bitmap_bounds) - 1 and the physical data boundary.
    int16_t max_x;
} GBitmapDataRowInfo;

//! Provides information about a pixel data row
//! @param bitmap A pointer to the GBitmap to get row info
//! @param y Absolute row number in the pixel data, independent from the bitmap's bounds
//! @return Description of the row
//! @note This function does not respect the bitmap's bounds but purely operates on the pixel data.
//!       This function works with every bitmap format including GBitmapFormat1Bit.
//!       The result of the function for invalid rows is undefined.
//! @see \ref gbitmap_get_data
GBitmapDataRowInfo gbitmap_get_data_row_info(const GBitmap *bitmap, uint16_t y);

//! Values to specify how two things should be aligned relative to each other.
//! ![](galign.png)
//! @see \ref bitmap_layer_set_alignment()
typedef enum GAlign {
  //! Align by centering
  GAlignCenter,
  //! Align by making the top edges overlap and left edges overlap
  GAlignTopLeft,
  //! Align by making the top edges overlap and left edges overlap
  GAlignTopRight,
  //! Align by making the top edges overlap and centered horizontally
  GAlignTop,
  //! Align by making the left edges overlap and centered vertically
  GAlignLeft,
  //! Align by making the bottom edges overlap and centered horizontally
  GAlignBottom,
  //! Align by making the right edges overlap and centered vertically
  GAlignRight,
  //! Align by making the bottom edges overlap and right edges overlap
  GAlignBottomRight,
  //! Align by making the bottom edges overlap and left edges overlap
  GAlignBottomLeft
} GAlign;

//! Aligns one rectangle within another rectangle, using an alignment parameter.
//! The relative coordinate systems of both rectangles are assumed to be the same.
//! When clip is true, `rect` is also clipped by the constraint.
//! @param[in] rect The rectangle to align (in place)
//! @param[out] rect The aligned and optionally clipped rectangle
//! @param inside_rect The rectangle in which to align `rect`
//! @param alignment Determines the alignment of `rect` within `inside_rect` by
//! specifying what edges of should overlap.
//! @param clip Determines whether `rect` should be trimmed using the edges of `inside_rect`
//! in case `rect` extends outside of the area that `inside_rect` covers after the alignment.
void grect_align(GRect *rect, const GRect *inside_rect, const GAlign alignment, const bool clip);

//! Values to specify how the source image should be composited onto the destination image.
//!
//! For Aplite, there is no notion of "transparency" in the graphics system. However, the effect of
//! transparency can be created by masking and using compositing modes.
//! ![](compops.png)
//! Contrived example of how the different compositing modes affect drawing.
//! Often, the "destination image" is the render buffer and thus contains the image of
//! what has been drawn before or "underneath".
//!
//! For Basalt, at the moment, only two compositing modes are supported, \ref GCompOpAssign and
//! \ref GCompOpSet. The behavior of other compositing modes are undefined and may change in the
//! future. Transparency can be achieved using \ref GCompOpSet and requires pixel values with alpha
//! value .a < 3.
//! @see \ref bitmap_layer_set_compositing_mode()
//! @see \ref graphics_context_set_compositing_mode()
//! @see \ref graphics_draw_bitmap_in_rect()
//! @see \ref graphics_draw_rotated_bitmap()
typedef enum {
  //! Assign the pixel values of the source image to the destination pixels,
  //! effectively replacing the previous values for those pixels. For Basalt, when drawing a color
  //! palettized or 8-bit \ref GBitmap image, the opacity value is ignored.
  GCompOpAssign,
  //! Assign the **inverted** pixel values of the source image to the destination pixels,
  //! effectively replacing the previous values for those pixels.
  //! @note For Basalt, this mode is not supported and the resulting behavior is undefined.
  GCompOpAssignInverted,
  //! Use the boolean operator `OR` to composite the source and destination pixels.
  //! The visual result of this compositing mode is the source's white pixels
  //! are painted onto the destination and the source's black pixels are treated
  //! as clear.
  //! @note For Basalt, this mode is not supported and the resulting behavior is undefined.
  GCompOpOr,
  //! Use the boolean operator `AND` to composite the source and destination pixels.
  //! The visual result of this compositing mode is the source's black pixels
  //! are painted onto the destination and the source's white pixels are treated
  //! as clear.
  //! @note For Basalt, this mode is not supported and the resulting behavior is undefined.
  GCompOpAnd,
  //! Clears the bits in the destination image, using the source image as mask.
  //! The visual result of this compositing mode is that for the parts where the source image is
  //! white, the destination image will be painted black. Other parts will be left untouched.
  //! @note For Basalt, this mode is not supported and the resulting behavior is undefined.
  GCompOpClear,
  //! Sets the bits in the destination image, using the source image as mask. For Aplite,
  //! the visual result of this compositing mode is that for the parts where the source image is
  //! black, the destination image will be painted white. Other parts will be left untouched.
  //! For Basalt, when drawing a color palettized or 8-bit \ref GBitmap image, this mode will be
  //! required to apply any transparency.
  GCompOpSet,
} GCompOp;

struct GContext;
typedef struct GContext GContext;

//! Represents insets for four sides. Negative values mean a side extends.
//! @see \ref grect_inset
typedef struct {
  //! The inset at the top of an object.
  int16_t top;
  //! The inset at the right of an object.
  int16_t right;
  //! The inset at the bottom of an object.
  int16_t bottom;
  //! The inset at the left of an object.
  int16_t left;
} GEdgeInsets;

//! helper for \ref GEdgeInsets macro
#define GEdgeInsets4(t, r, b, l) \
  ((GEdgeInsets){.top = t, .right = r, .bottom = b, .left = l})

//! helper for \ref GEdgeInsets macro
#define GEdgeInsets3(t, rl, b) \
  ((GEdgeInsets){.top = t, .right = rl, .bottom = b, .left = rl})

//! helper for \ref GEdgeInsets macro
#define GEdgeInsets2(tb, rl) \
  ((GEdgeInsets){.top = tb, .right = rl, .bottom = tb, .left = rl})

//! helper for \ref GEdgeInsets macro
#define GEdgeInsets1(trbl) \
  ((GEdgeInsets){.top = trbl, .right = trbl, .bottom = trbl, .left = trbl})

//! helper for \ref GEdgeInsets macro
#define GEdgeInsetsN(_1, _2, _3, _4, NAME, ...) NAME

//! Convenience macro to make a GEdgeInsets
//! This macro follows the CSS shorthand notation where you can call it with
//!  - just one value GEdgeInsets(v1) to configure all edges with v1
//!    (GEdgeInsets){.top = v1, .right = v1, .bottom = v1, .left = v1}
//!  - two values v1, v2 to configure a vertical and horizontal inset as
//!    (GEdgeInsets){.top = v1, .right = v2, .bottom = v1, .left = v2}
//!  - three values v1, v2, v3 to configure it with
//!    (GEdgeInsets){.top = v1, .right = v2, .bottom = v3, .left = v2}
//!  - four values v1, v2, v3, v4 to configure it with
//!    (GEdgeInsets){.top = v1, .right = v2, .bottom = v3, .left = v4}
//! @see \ref grect_insets
#define GEdgeInsets(...) \
  GEdgeInsetsN(__VA_ARGS__, GEdgeInsets4, GEdgeInsets3, GEdgeInsets2, GEdgeInsets1)(__VA_ARGS__)

//! Returns a rectangle that is shrinked or expanded by the given edge insets.
//! @note The rectangle is standardized and then the inset parameters are applied.
//! If the resulting rectangle would have a negative height or width, a GRectZero is returned.
//! @param rect The rectangle that will be inset
//! @param insets The insets that will be applied
//! @return The resulting rectangle
//! @note Use this function in together with the \ref GEdgeInsets macro
//! \code{.c}
//! GRect r_inset_all_sides = grect_inset(r, GEdgeInsets(10));
//! GRect r_inset_vertical_horizontal = grect_inset(r, GEdgeInsets(10, 20));
//! GRect r_expand_top_right_shrink_bottom_left = grect_inset(r, GEdgeInsets(-10, -10, 10, 10));
//! \endcode
GRect grect_inset(GRect rect, GEdgeInsets insets);

//! @} // group GraphicsTypes

//! @addtogroup GraphicsContext Graphics Context
//! \brief The "canvas" into which an application draws
//!
//! The Pebble OS graphics engine, inspired by several notable graphics systems, including
//! Apple’s Quartz 2D and its predecessor QuickDraw, provides your app with a canvas into
//! which to draw, namely, the graphics context. A graphics context is the target into which
//! graphics functions can paint, using Pebble drawing routines (see \ref Drawing,
//! \ref PathDrawing and \ref TextDrawing).
//!
//! A graphics context holds a reference to the bitmap into which to paint. It also holds the
//! current drawing state, like the current fill color, stroke color, clipping box, drawing box,
//! compositing mode, and so on. The GContext struct is the type representing the graphics context.
//!
//! For drawing in your Pebble watchface or watchapp, you won't need to create a GContext
//! yourself. In most cases, it is provided by Pebble OS as an argument passed into a render
//! callback (the .update_proc of a Layer).
//!
//! Your app can’t call drawing functions at any given point in time: Pebble OS will request your
//! app to render. Typically, your app will be calling out to graphics functions in
//! the .update_proc callback of a Layer.
//! @see \ref Layer
//! @see \ref Drawing
//! @see \ref PathDrawing
//! @see \ref TextDrawing
//! @{

//! Sets the current stroke color of the graphics context.
//! @param ctx The graphics context onto which to set the stroke color
//! @param color The new stroke color
void graphics_context_set_stroke_color(GContext* ctx, GColor color);

//! Sets the current fill color of the graphics context.
//! @param ctx The graphics context onto which to set the fill color
//! @param color The new fill color
void graphics_context_set_fill_color(GContext* ctx, GColor color);

//! Sets the current text color of the graphics context.
//! @param ctx The graphics context onto which to set the text color
//! @param color The new text color
void graphics_context_set_text_color(GContext* ctx, GColor color);

//! Sets the current bitmap compositing mode of the graphics context.
//! @param ctx The graphics context onto which to set the compositing mode
//! @param mode The new compositing mode
//! @see \ref GCompOp
//! @see \ref bitmap_layer_set_compositing_mode()
//! @note At the moment, this only affects the bitmaps drawing operations
//! -- \ref graphics_draw_bitmap_in_rect(), \ref graphics_draw_rotated_bitmap, and
//! anything that uses those APIs --, but it currently does not affect the filling or stroking
//! operations.
void graphics_context_set_compositing_mode(GContext* ctx, GCompOp mode);

//! Sets whether antialiasing is applied to stroke drawing
//! @param ctx The graphics context onto which to set the antialiasing
//! @param enable True = antialiasing enabled, False = antialiasing disabled
//! @note Default value is true.
void graphics_context_set_antialiased(GContext* ctx, bool enable);

//! Sets the width of the stroke for drawing routines
//! @param ctx The graphics context onto which to set the stroke width
//! @param stroke_width Width in pixels of the stroke.
//! @note If stroke width of zero is passed, it will be ignored and will not change the value
//! stored in GContext. Currently, only odd stroke_width values are supported. If an even value
//! is passed in, the value will be stored as is, but the drawing routines will round down to the
//! previous integral value when drawing. Default value is 1.
void graphics_context_set_stroke_width(GContext* ctx, uint8_t stroke_width);

//! @} // group GraphicsContext

//! @addtogroup Drawing Drawing Primitives
//! \brief Functions to draw into a graphics context
//!
//! Use these drawing functions inside a Layer's `.update_proc` drawing
//! callback. A `GContext` is passed into this callback as an argument.
//! This `GContext` can then be used with all of the drawing functions which
//! are documented below.
//! See \ref GraphicsContext for more information about the graphics context.
//!
//! Refer to \htmlinclude UiFramework.html (chapter "Layers" and "Graphics") for a
//! conceptual overview of the drawing system, Layers and relevant code examples.
//!
//! Other drawing functions and related documentation:
//! * \ref TextDrawing
//! * \ref PathDrawing
//! * \ref GraphicsTypes
//! @{

//! Bit mask values to specify the corners of a rectangle.
//! The values can be combines using binary OR (`|`),
//! For example: the mask to indicate top left and bottom right corners can:
//! be created as follows: `(GCornerTopLeft | GCornerBottomRight)`
typedef enum {
  //! No corners
  GCornerNone = 0,
  //! Top-Left corner
  GCornerTopLeft = 1 << 0,
  //! Top-Right corner
  GCornerTopRight = 1 << 1,
  //! Bottom-Left corner
  GCornerBottomLeft = 1 << 2,
  //! Bottom-Right corner
  GCornerBottomRight = 1 << 3,
  //! All corners
  GCornersAll = GCornerTopLeft | GCornerTopRight | GCornerBottomLeft | GCornerBottomRight,
  //! Top corners
  GCornersTop = GCornerTopLeft | GCornerTopRight,
  //! Bottom corners
  GCornersBottom = GCornerBottomLeft | GCornerBottomRight,
  //! Left corners
  GCornersLeft = GCornerTopLeft | GCornerBottomLeft,
  //! Right corners
  GCornersRight = GCornerTopRight | GCornerBottomRight,
} GCornerMask;

//! Draws a pixel at given point in the current stroke color
//! @param ctx The destination graphics context in which to draw
//! @param point The point at which to draw the pixel
void graphics_draw_pixel(GContext* ctx, GPoint point);

//! Draws line in the current stroke color, current stroke width and AA flag
//! @param ctx The destination graphics context in which to draw
//! @param p0 The starting point of the line
//! @param p1 The ending point of the line
void graphics_draw_line(GContext* ctx, GPoint p0, GPoint p1);

//! Draws a 1-pixel wide rectangle outline in the current stroke color
//! @param ctx The destination graphics context in which to draw
//! @param rect The rectangle for which to draw the outline
void graphics_draw_rect(GContext *ctx, GRect rect);

//! Fills a rectangle with the current fill color, optionally rounding all or a
//! selection of its corners.
//! @param ctx The destination graphics context in which to draw
//! @param rect The rectangle to fill
//! @param corner_radius The rounding radius of the corners in pixels (maximum is 8 pixels)
//! @param corner_mask Bitmask of the corners that need to be rounded.
//! @see \ref GCornerMask
void graphics_fill_rect(GContext *ctx, GRect rect, uint16_t corner_radius,
                                       GCornerMask corner_mask);

//! Draws the outline of a circle in the current stroke color
//! @param ctx The destination graphics context in which to draw
//! @param p The center point of the circle
//! @param radius The radius in pixels
void graphics_draw_circle(GContext* ctx, GPoint p, uint16_t radius);

//! Fills a circle in the current fill color
//! @param ctx The destination graphics context in which to draw
//! @param p The center point of the circle
//! @param radius The radius in pixels
void graphics_fill_circle(GContext* ctx, GPoint p, uint16_t radius);

//! Draws the outline of a rounded rectangle in the current stroke color
//! @param ctx The destination graphics context in which to draw
//! @param rect The rectangle defining the dimensions of the rounded rectangle to draw
//! @param radius The corner radius in pixels
void graphics_draw_round_rect(GContext *ctx, GRect rect, uint16_t radius);

//! Draws a bitmap into the graphics context, inside the specified rectangle
//! @param ctx The destination graphics context in which to draw the bitmap
//! @param bitmap The bitmap to draw
//! @param rect The rectangle in which to draw the bitmap
//! @note If the size of `rect` is smaller than the size of the bitmap,
//! the bitmap will be clipped on right and bottom edges.
//! If the size of `rect` is larger than the size of the bitmap,
//! the bitmap will be tiled automatically in both horizontal and vertical
//! directions, effectively drawing a repeating pattern.
//! @see GBitmap
//! @see GContext
void graphics_draw_bitmap_in_rect(GContext *ctx, const GBitmap *bitmap, GRect rect);

//! A shortcut to capture the framebuffer in the native format of the watch.
//! @see graphics_capture_frame_buffer_format
GBitmap* graphics_capture_frame_buffer(GContext* ctx);

//! Captures the frame buffer for direct access, using the given format.
//! Graphics functions will not affect the frame buffer while it is captured.
//! The frame buffer is released when {@link graphics_release_frame_buffer} is called.
//! The frame buffer must be released before the end of a layer's `.update_proc`
//! for the layer to be drawn properly.
//!
//! While the frame buffer is captured calling {@link graphics_capture_frame_buffer}
//! will fail and return `NULL`.
//! @note When writing to the frame buffer, you should respect the visible boundaries of a
//! window on the screen. Use layer_get_frame(window_get_root_layer(window)).origin to obtain its
//! position relative to the frame buffer. For example, drawing to (5, 5) in the frame buffer
//! while the window is transitioning to the left with its origin at (-20, 0) would
//! effectively draw that point at (25, 5) relative to the window. For this reason you should
//! consider the window's root layer frame when calculating drawing coordinates.
//! @see GBitmap
//! @see GBitmapFormat
//! @see layer_get_frame
//! @see window_get_root_layer
//! @param ctx The graphics context providing the frame buffer
//! @param format The format in which the framebuffer should be captured. Supported formats
//! are \ref GBitmapFormat1Bit and \ref GBitmapFormat8Bit.
//! @return A pointer to the frame buffer. `NULL` if failed.
GBitmap *graphics_capture_frame_buffer_format(GContext *ctx, GBitmapFormat format);

//! Releases the frame buffer.
//! Must be called before the end of a layer's `.update_proc` for the layer to be drawn properly.
//!
//! If `buffer` does not point to the address previously returned by
//! {@link graphics_capture_frame_buffer} the frame buffer will not be released.
//! @param ctx The graphics context providing the frame buffer
//! @param buffer The pointer to frame buffer
//! @return True if the frame buffer was released successfully
bool graphics_release_frame_buffer(GContext* ctx, GBitmap* buffer);

//! Whether or not the frame buffer has been captured by {@link graphics_capture_frame_buffer}.
//! Graphics functions will not affect the frame buffer until it has been released by
//! {@link graphics_release_frame_buffer}.
//! @param ctx The graphics context providing the frame buffer
//! @return True if the frame buffer has been captured
bool graphics_frame_buffer_is_captured(GContext* ctx);

//! Draws a rotated bitmap with a memory-sensitive 2x anti-aliasing technique
//! (using ray-finding instead of super-sampling), which is thresholded into a b/w bitmap for 1-bit
//! and color blended for 8-bit.
//! @note This API has performance limitations that can degrade user experience. Use sparingly.
//! @param ctx The destination graphics context in which to draw
//! @param src The source bitmap to draw
//! @param src_ic Instance center (single point unaffected by rotation) relative to source bitmap
//! @param rotation Angle of rotation. Rotation is an integer between 0 (no rotation)
//! and TRIG_MAX_ANGLE (360 degree rotation). Use \ref DEG_TO_TRIGANGLE to easily convert degrees
//! to the appropriate value.
//! @param dest_ic Where to draw the instance center of the rotated bitmap in the context.
void graphics_draw_rotated_bitmap(GContext* ctx, GBitmap *src, GPoint src_ic, int rotation, GPoint dest_ic);

//! Values to specify how a given rectangle should be used to derive an oval shape.
//! @see \ref graphics_fill_radial_internal
//! @see \ref graphics_draw_arc_internal
//! @see \ref gpoint_from_polar_internal
//! @see \ref grect_centered_from_polar
typedef enum {
  //! Places the largest possible fully visible circle in the center of a rectangle.
  GOvalScaleModeFitCircle,
  //! Places the smallest possible circle in the center of a rectangle so that the rectangle is
  //! fully inside the circle.
  GOvalScaleModeFillCircle,
} GOvalScaleMode;

//! Draws a line arc clockwise between `angle_start` and `angle_end`, where 0° is
//! the top of the circle. If the difference between `angle_start` and `angle_end` is greater
//! than 360°, a full circle will be drawn.
//! @param ctx The destination graphics context in which to draw using the current
//!        stroke color and antialiasing setting.
//! @param rect The reference rectangle to derive the center point and radius (see scale_mode).
//! @param scale_mode Determines how rect will be used to derive the center point and radius.
//! @param angle_start Radial starting angle. Use \ref DEG_TO_TRIGANGLE to easily convert degrees
//! to the appropriate value.
//! @param angle_end Radial finishing angle. If smaller than `angle_start`, nothing will be drawn.
void graphics_draw_arc(GContext *ctx, GRect rect, GOvalScaleMode scale_mode,
                       int32_t angle_start, int32_t angle_end);

//! Fills a circle clockwise between `angle_start` and `angle_end`, where 0° is
//! the top of the circle. If the difference between `angle_start` and `angle_end` is greater
//! than 360°, a full circle will be drawn and filled. If `angle_start` is greater than
//! `angle_end` nothing will be drawn.
//! @note A simple example is drawing a 'Pacman' shape, with a starting angle of -225°, and
//! ending angle of 45°. By setting `inset_thickness` to a non-zero value (such as 30) this
//! example will produce the letter C.
//! @param ctx The destination graphics context in which to draw using the current
//! fill color and antialiasing setting.
//! @param rect The reference rectangle to derive the center point and radius (see scale).
//! @param scale_mode Determines how rect will be used to derive the center point and radius.
//! @param inset_thickness Describes how thick in pixels the radial will be drawn towards its
//!        center measured from the outside.
//! @param angle_start Radial starting angle. Use \ref DEG_TO_TRIGANGLE to easily convert degrees
//! to the appropriate value.
//! @param angle_end Radial finishing angle. If smaller than `angle_start`, nothing will be drawn.
void graphics_fill_radial(GContext *ctx, GRect rect, GOvalScaleMode scale_mode,
                          uint16_t inset_thickness,
                          int32_t angle_start, int32_t angle_end);

//! Calculates a GPoint located at the angle provided on the perimeter of a circle defined by the
//! provided GRect.
//! @param rect The reference rectangle to derive the center point and radius (see scale_mode).
//! @param scale_mode Determines how rect will be used to derive the center point and radius.
//! @param angle The angle at which the point on the circle's perimeter should be calculated.
//! Use \ref DEG_TO_TRIGANGLE to easily convert degrees to the appropriate value.
//! @return The point on the circle's perimeter.
GPoint gpoint_from_polar(GRect rect, GOvalScaleMode scale_mode, int32_t angle);

//! Calculates a rectangle centered on the perimeter of a circle at a given angle.
//! Use this to construct rectangles that follow the perimeter of a circle as an input for
//! \ref graphics_fill_radial_internal or \ref graphics_draw_arc_internal,
//! e.g. to draw circles every 30 degrees on a watchface.
//! @param rect The reference rectangle to derive the circle's center point and radius (see
//!        scale_mode).
//! @param scale_mode Determines how rect will be used to derive the circle's center point and
//!        radius.
//! @param angle The angle at which the point on the circle's perimeter should be calculated.
//! Use \ref DEG_TO_TRIGANGLE to easily convert degrees to the appropriate value.
//! @param size Width and height of the desired rectangle.
//! @return The rectangle centered on the circle's perimeter.
GRect grect_centered_from_polar(GRect rect, GOvalScaleMode scale_mode, int32_t angle, GSize size);

//! @} // group Drawing

//! @addtogroup DrawCommand Draw Commands
//! \brief Pebble Draw Commands are a way to encode arbitrary path draw and fill calls in binary
//! format, so that vector-like graphics can be represented on the watch.
//!
//! These draw commands can
//! be loaded from resources, manipulated in place and drawn to the current graphics context. Each
//! \ref GDrawCommand can be an arbitrary path or a circle with optional fill or stroke. The stroke
//! width and color of the stroke and fill are also encoded within the \ref GDrawCommand. Paths can
//! can be drawn open or closed.
//!
//! All aspects of a draw command can be modified, except for the number of points in a path (a
//! circle only has one point, the center).
//!
//! Draw commands are grouped into a \ref GDrawCommandList, which can be drawn all at once.
//! Each individual \ref GDrawCommand can be accessed from a \ref GDrawCommandList for modification.
//!
//! A \ref GDrawCommandList forms the basis for \ref GDrawCommandImage and \ref GDrawCommandFrame
//! objects. A \ref GDrawCommandImage represents a static image and can be represented by the PDC
//! file format and can be loaded as a resource.
//!
//! Once you have a \ref GDrawCommandImage loaded in memory you can draw it on the screen in a
//! \ref LayerUpdateProc with the \ref gdraw_command_image_draw().
//!
//! A \ref GDrawCommandFrame represents a single frame of an animated sequence, with multiple frames
//! making up a single \ref GDrawCommandSequence, which can also be stored as a PDC and loaded as a
//! resource.
//!
//! To draw a \ref GDrawCommandSequence, use the \ref gdraw_command_sequence_get_frame_by_elapsed()
//! to obtain the current \ref GDrawCommandFrame and \ref gdraw_command_frame_draw() to draw it.
//!
//! Draw commands also allow access to drawing with sub-pixel precision. The points are treated as
//! Fixed point types in the format 13.3, so that 1/8th of a pixel precision is possible. Only the
//! points in draw commands of the type GDrawCommandTypePrecisePath will be treated as higher
//! precision.
//!
//! @{

//! Draw commands are the basic building block of the draw command system, encoding the type of
//! command to draw, the stroke width and color, fill color, and points that define the path (or
//! center of a circle
typedef struct GDrawCommand GDrawCommand;

//! Draw command frames contain a list of commands to draw for that frame and a duration,
//! indicating the length of time for which the frame should be drawn in an animation sequence.
//! Frames form the building blocks of a \ref GDrawCommandSequence, which consists of multiple
//! frames.
typedef struct GDrawCommandFrame GDrawCommandFrame;

//! Draw command images contain a list of commands that can be drawn. An image can be loaded from
//! PDC file data.
typedef struct GDrawCommandImage GDrawCommandImage;

//! Draw command lists contain a list of commands that can be iterated over and drawn all at once
typedef struct GDrawCommandList GDrawCommandList;

//! Callback for iterating over draw command list
//! @param command current \ref GDrawCommand in iteration
//! @param index index of the current command in the list
//! @param context context pointer for the iteration operation
//! @return true if the iteration should continue after this command is processed
typedef bool (*GDrawCommandListIteratorCb)(GDrawCommand *command, uint32_t index, void *context);

//! Draw command sequences allow the animation of frames over time. Each sequence has a list of
//! frames that can be accessed by the elapsed duration of the animation (not maintained internally)
//! or by index. Sequences can be loaded from PDC file data.
typedef struct GDrawCommandSequence GDrawCommandSequence;

typedef enum {
  GDrawCommandTypeInvalid = 0,  //!< Invalid draw command type
  GDrawCommandTypePath,         //!< Arbitrary path draw command type
  GDrawCommandTypeCircle,       //!< Circle draw command type
  GDrawCommandTypePrecisePath,  //!< Arbitrary path drawn with sub-pixel precision (1/8th precision)
} GDrawCommandType;

//! Draw a command
//! @param ctx The destination graphics context in which to draw
//! @param command \ref GDrawCommand to draw
void gdraw_command_draw(GContext *ctx, GDrawCommand *command);

//! Get the command type
//! @param command \ref GDrawCommand from which to get the type
//! @return The type of the given \ref GDrawCommand
GDrawCommandType gdraw_command_get_type(GDrawCommand *command);

//! Set the fill color of a command
//! @param command ref DrawCommand for which to set the fill color
//! @param fill_color \ref GColor to set for the fill
void gdraw_command_set_fill_color(GDrawCommand *command, GColor fill_color);

//! Get the fill color of a command
//! @param command \ref GDrawCommand from which to get the fill color
//! @return fill color of the given \ref GDrawCommand
GColor gdraw_command_get_fill_color(GDrawCommand *command);

//! Set the stroke color of a command
//! @param command \ref GDrawCommand for which to set the stroke color
//! @param stroke_color \ref GColor to set for the stroke
void gdraw_command_set_stroke_color(GDrawCommand *command, GColor stroke_color);

//! Get the stroke color of a command
//! @param command \ref GDrawCommand from which to get the stroke color
//! @return The stroke color of the given \ref GDrawCommand
GColor gdraw_command_get_stroke_color(GDrawCommand *command);

//! Set the stroke width of a command
//! @param command \ref GDrawCommand for which to set the stroke width
//! @param stroke_width stroke width to set for the command
void gdraw_command_set_stroke_width(GDrawCommand *command, uint8_t stroke_width);

//! Get the stroke width of a command
//! @param command \ref GDrawCommand from which to get the stroke width
//! @return The stroke width of the given \ref GDrawCommand
uint8_t gdraw_command_get_stroke_width(GDrawCommand *command);

//! Get the number of points in a command
uint16_t gdraw_command_get_num_points(GDrawCommand *command);

//! Set the value of the point in a command at the specified index
//! @param command \ref GDrawCommand for which to set the value of a point
//! @param point_idx Index of the point to set the value for
//! @param point new point value to set
void gdraw_command_set_point(GDrawCommand *command, uint16_t point_idx, GPoint point);

//! Get the value of a point in a command from the specified index
//! @param command \ref GDrawCommand from which to get a point
//! @param point_idx The index to get the point for
//! @return The point in the \ref GDrawCommand specified by point_idx
//! @note The index \b must be less than the number of points
GPoint gdraw_command_get_point(GDrawCommand *command, uint16_t point_idx);

//! Set the radius of a circle command
//! @note This only works for commands of type \ref GDrawCommandCircle
//! @param command \ref GDrawCommand from which to set the circle radius
//! @param radius The radius to set for the circle.
void gdraw_command_set_radius(GDrawCommand *command, uint16_t radius);

//! Get the radius of a circle command.
//! @note this only works for commands of type\ref GDrawCommandCircle.
//! @param command \ref GDrawCommand from which to get the circle radius
//! @return The radius in pixels if command is of type \ref GDrawCommandCircle
uint16_t gdraw_command_get_radius(GDrawCommand *command);

//! Set the path of a stroke command to be open
//! @note This only works for commands of type \ref GDrawCommandPath and
//! \ref GDrawCommandPrecisePath
//! @param command \ref GDrawCommand for which to set the path open status
//! @param path_open true if path should be hidden
void gdraw_command_set_path_open(GDrawCommand *command, bool path_open);

//! Return whether a stroke command path is open
//! @note This only works for commands of type \ref GDrawCommandPath and
//! \ref GDrawCommandPrecisePath
//! @param command \ref GDrawCommand from which to get the path open status
//! @return true if the path is open
bool gdraw_command_get_path_open(GDrawCommand *command);

//! Set a command as hidden. This command will not be drawn when \ref gdraw_command_draw is called
//! with this command
//! @param command \ref GDrawCommand for which to set the hidden status
//! @param hidden true if command should be hidden
void gdraw_command_set_hidden(GDrawCommand *command, bool hidden);

//! Return whether a command is hidden
//! @param command \ref GDrawCommand from which to get the hidden status
//! @return true if command is hidden
bool gdraw_command_get_hidden(GDrawCommand *command);

//! Draw a frame
//! @param ctx The destination graphics context in which to draw
//! @param sequence The sequence from which the frame comes from (this is required)
//! @param frame Frame to draw
//! @param offset Offset from draw context origin to draw the frame
void gdraw_command_frame_draw(GContext *ctx, GDrawCommandSequence *sequence,
                              GDrawCommandFrame *frame, GPoint offset);

//! Set the duration of the frame
//! @param frame \ref GDrawCommandFrame for which to set the duration
//! @param duration duration of the frame in milliseconds
void gdraw_command_frame_set_duration(GDrawCommandFrame *frame, uint32_t duration);

//! Get the duration of the frame
//! @param frame \ref GDrawCommandFrame from which to get the duration
//! @return duration of the frame in milliseconds
uint32_t gdraw_command_frame_get_duration(GDrawCommandFrame *frame);

//! Creates a GDrawCommandImage from the specified resource (PDC file)
//! @param resource_id Resource containing data to load and create GDrawCommandImage from.
//! @return GDrawCommandImage pointer if the resource was loaded, NULL otherwise
GDrawCommandImage *gdraw_command_image_create_with_resource(uint32_t resource_id);

//! Creates a GDrawCommandImage as a copy from a given image
//! @param image Image to copy.
//! @return cloned image or NULL if the operation failed
GDrawCommandImage *gdraw_command_image_clone(GDrawCommandImage *image);

//! Deletes the GDrawCommandImage structure and frees associated data
//! @param image Pointer to the image to free (delete)
void gdraw_command_image_destroy(GDrawCommandImage *image);

//! Draw an image
//! @param ctx The destination graphics context in which to draw
//! @param image Image to draw
//! @param offset Offset from draw context origin to draw the image
void gdraw_command_image_draw(GContext *ctx, GDrawCommandImage *image, GPoint offset);

//! Get size of the bounding box surrounding all draw commands in the image. This bounding
//! box can be used to set the graphics context or layer bounds when drawing the image.
//! @param image \ref GDrawCommandImage from which to get the bounding box size
//! @return bounding box size
GSize gdraw_command_image_get_bounds_size(GDrawCommandImage *image);

//! Set size of the bounding box surrounding all draw commands in the image. This bounding
//! box can be used to set the graphics context or layer bounds when drawing the image.
//! @param image \ref GDrawCommandImage for which to set the bounding box size
//! @param size bounding box size
void gdraw_command_image_set_bounds_size(GDrawCommandImage *image, GSize size);

//! Get the command list of the image
//! @param image \ref GDrawCommandImage from which to get the command list
//! @return command list
GDrawCommandList *gdraw_command_image_get_command_list(GDrawCommandImage *image);

//! Iterate over all commands in a command list
//! @param command_list \ref GDrawCommandList over which to iterate
//! @param handle_command iterator callback
//! @param callback_context context pointer to be passed into the iterator callback
void gdraw_command_list_iterate(GDrawCommandList *command_list,
                                GDrawCommandListIteratorCb handle_command, void *callback_context);

//! Draw all commands in a command list
//! @param ctx The destination graphics context in which to draw
//! @param command_list list of commands to draw
void gdraw_command_list_draw(GContext *ctx, GDrawCommandList *command_list);

//! Get the command at the specified index
//! @note the specified index must be less than the number of commands in the list
//! @param command_list \ref GDrawCommandList from which to get a command
//! @param command_idx index of the command to get
//! @return pointer to \ref GDrawCommand at the specified index
GDrawCommand *gdraw_command_list_get_command(GDrawCommandList *command_list, uint16_t command_idx);

//! Get the number of commands in the list
//! @param command_list \ref GDrawCommandList from which to get the number of commands
//! @return number of commands in command list
uint32_t gdraw_command_list_get_num_commands(GDrawCommandList *command_list);

//! Creates a \ref GDrawCommandSequence from the specified resource (PDC file)
//! @param resource_id Resource containing data to load and create GDrawCommandSequence from.
//! @return GDrawCommandSequence pointer if the resource was loaded, NULL otherwise
GDrawCommandSequence *gdraw_command_sequence_create_with_resource(uint32_t resource_id);

//! Creates a \ref GDrawCommandSequence as a copy from a given sequence
//! @param sequence Sequence to copy
//! @return cloned sequence or NULL if the operation failed
GDrawCommandSequence *gdraw_command_sequence_clone(GDrawCommandSequence *sequence);

//! Deletes the \ref GDrawCommandSequence structure and frees associated data
//! @param image Pointer to the sequence to destroy
void gdraw_command_sequence_destroy(GDrawCommandSequence *sequence);

//! Get the frame that should be shown after the specified amount of elapsed time
//! The last frame will be returned if the elapsed time exceeds the total time
//! @param sequence \ref GDrawCommandSequence from which to get the frame
//! @param elapsed_ms elapsed time in milliseconds
//! @return pointer to \ref GDrawCommandFrame that should be displayed at the elapsed time
GDrawCommandFrame *gdraw_command_sequence_get_frame_by_elapsed(GDrawCommandSequence *sequence,
                                                               uint32_t elapsed_ms);

//! Get the frame at the specified index
//! @param sequence \ref GDrawCommandSequence from which to get the frame
//! @param index Index of frame to get
//! @return pointer to \ref GDrawCommandFrame at the specified index
GDrawCommandFrame *gdraw_command_sequence_get_frame_by_index(GDrawCommandSequence *sequence,
                                                             uint32_t index);

//! Get the size of the bounding box surrounding all draw commands in the sequence. This bounding
//! box can be used to set the graphics context or layer bounds when drawing the frames in the
//! sequence.
//! @param sequence \ref GDrawCommandSequence from which to get the bounds
//! @return bounding box size
GSize gdraw_command_sequence_get_bounds_size(GDrawCommandSequence *sequence);

//! Set size of the bounding box surrounding all draw commands in the sequence. This bounding
//! box can be used to set the graphics context or layer bounds when drawing the frames in the
//! sequence.
//! @param sequence \ref GDrawCommandSequence for which to set the bounds
//! @param size bounding box size
void gdraw_command_sequence_set_bounds_size(GDrawCommandSequence *sequence, GSize size);

//! Get the play count of the sequence
//! @param sequence \ref GDrawCommandSequence from which to get the play count
//! @return play count of sequence
uint32_t gdraw_command_sequence_get_play_count(GDrawCommandSequence *sequence);

//! Set the play count of the sequence
//! @param sequence \ref GDrawCommandSequence for which to set the play count
//! @param play_count play count
void gdraw_command_sequence_set_play_count(GDrawCommandSequence *sequence, uint32_t play_count);

//! Get the total duration of the sequence.
//! @param sequence \ref GDrawCommandSequence from which to get the total duration
//! @return total duration of the sequence in milliseconds
uint32_t gdraw_command_sequence_get_total_duration(GDrawCommandSequence *sequence);

//! Get the number of frames in the sequence
//! @param sequence \ref GDrawCommandSequence from which to get the number of frames
//! @return number of frames in the sequence
uint32_t gdraw_command_sequence_get_num_frames(GDrawCommandSequence *sequence);

//! Get the command list of the frame
//! @param frame \ref GDrawCommandFrame from which to get the command list
//! @return command list
GDrawCommandList *gdraw_command_frame_get_command_list(GDrawCommandFrame *frame);

//! @} // group DrawCommand

//! @addtogroup PathDrawing Drawing Paths
//! \brief Functions to draw polygons into a graphics context
//!
//! Code example:
//! \code{.c}
//! static GPath *s_my_path_ptr = NULL;
//!
//! static const GPathInfo BOLT_PATH_INFO = {
//!   .num_points = 6,
//!   .points = (GPoint []) {{21, 0}, {14, 26}, {28, 26}, {7, 60}, {14, 34}, {0, 34}}
//! };
//!
//! // .update_proc of my_layer:
//! void my_layer_update_proc(Layer *my_layer, GContext* ctx) {
//!   // Fill the path:
//!   graphics_context_set_fill_color(ctx, GColorWhite);
//!   gpath_draw_filled(ctx, s_my_path_ptr);
//!   // Stroke the path:
//!   graphics_context_set_stroke_color(ctx, GColorBlack);
//!   gpath_draw_outline(ctx, s_my_path_ptr);
//! }
//!
//! void setup_my_path(void) {
//!   s_my_path_ptr = gpath_create(&BOLT_PATH_INFO);
//!   // Rotate 15 degrees:
//!   gpath_rotate_to(s_my_path_ptr, TRIG_MAX_ANGLE / 360 * 15);
//!   // Translate by (5, 5):
//!   gpath_move_to(s_my_path_ptr, GPoint(5, 5));
//! }
//!
//! // For brevity, the setup of my_layer is not written out...
//! \endcode
//! @{

//! Data structure describing a naked path
//! @note Note that this data structure only refers to an array of points;
//! the points are not stored inside this data structure itself.
//! In most cases, one cannot use a stack-allocated array of GPoints. Instead
//! one often needs to provide longer-lived (static or "global") storage for the points.
typedef struct GPathInfo {
  //! The number of points in the `points` array
  uint32_t num_points;
  //! Pointer to an array of points.
  GPoint *points;
} GPathInfo;

//! Data structure describing a path, plus its rotation and translation.
//! @note See the remark with \ref GPathInfo
typedef struct GPath {
  //! The number of points in the `points` array
  uint32_t num_points;
  //! Pointer to an array of points.
  GPoint *points;
  //! The rotation that will be used when drawing the path with
  //! \ref gpath_draw_filled() or \ref gpath_draw_outline()
  int32_t rotation;
  //! The translation that will to be used when drawing the path with
  //! \ref gpath_draw_filled() or \ref gpath_draw_outline()
  GPoint offset;
} GPath;

//! Creates a new GPath on the heap based on a series of points described by a GPathInfo.
//!
//! Values after initialization:
//! * `num_points` and `points` pointer: copied from the GPathInfo.
//! * `rotation`: 0
//! * `offset`: (0, 0)
//! @return A pointer to the GPath. `NULL` if the GPath could not
//! be created
GPath* gpath_create(const GPathInfo *init);

//! Free a dynamically allocated gpath created with \ref gpath_create()
void gpath_destroy(GPath* gpath);

//! Draws the fill of a path into a graphics context, using the current fill color,
//! relative to the drawing area as set up by the layering system.
//! @param ctx The graphics context to draw into
//! @param path The path to fill
//! @see \ref graphics_context_set_fill_color()
void gpath_draw_filled(GContext* ctx, GPath *path);

//! Draws the outline of a path into a graphics context, using the current stroke color and
//! width, relative to the drawing area as set up by the layering system. The first and last points
//! in the path do have a line between them.
//! @param ctx The graphics context to draw into
//! @param path The path to draw
//! @see \ref graphics_context_set_stroke_color()
//! @see \ref gpath_draw_outline_open()
void gpath_draw_outline(GContext* ctx, GPath *path);

//! Sets the absolute rotation of the path.
//! The current rotation will be replaced by the specified angle.
//! @param path The path onto which to set the rotation
//! @param angle The absolute angle of the rotation. The angle is represented in the same way
//! that is used with \ref sin_lookup(). See \ref TRIG_MAX_ANGLE for more information.
//! @note Setting a rotation does not affect the points in the path directly.
//! The rotation is applied on-the-fly during drawing, either using \ref gpath_draw_filled() or
//! \ref gpath_draw_outline().
void gpath_rotate_to(GPath *path, int32_t angle);

//! Sets the absolute offset of the path.
//! The current translation will be replaced by the specified offset.
//! @param path The path onto which to set the translation
//! @param point The point which is used as the vector for the translation.
//! @note Setting a translation does not affect the points in the path directly.
//! The translation is applied on-the-fly during drawing, either using \ref gpath_draw_filled() or
//! \ref gpath_draw_outline().
void gpath_move_to(GPath *path, GPoint point);

//! Draws an open outline of a path into a graphics context, using the current stroke color and
//! width, relative to the drawing area as set up by the layering system. The first and last points
//! in the path do not have a line between them.
//! @param ctx The graphics context to draw into
//! @param path The path to draw
//! @see \ref graphics_context_set_stroke_color()
//! @see \ref gpath_draw_outline()
void gpath_draw_outline_open(GContext* ctx, GPath* path);

//! @} // group PathDrawing

//! @addtogroup Fonts
//! @see \ref TextLayer
//! @see \ref TextDrawing
//! @see \ref text_layer_set_font
//! @see \ref graphics_draw_text
//! @{

struct FontInfo;
typedef struct FontInfo FontInfo;

//! Pointer to opaque font data structure.
//! @see \ref fonts_load_custom_font()
//! @see \ref text_layer_set_font()
//! @see \ref graphics_draw_text()
typedef FontInfo* GFont;

//! Loads a system font corresponding to the specified font key.
//! @param font_key The string key of the font to load. See `pebble_fonts.h` for a list of system fonts.
//! @return An opaque pointer to the loaded font, or, a pointer to the default
//! (fallback) font if the specified font cannot be loaded.
//! @note This may load a font from the flash peripheral into RAM.
GFont fonts_get_system_font(const char *font_key);

//! Loads a custom font.
//! @param handle The resource handle of the font to load. See resource_ids.auto.h
//! for a list of resource IDs, and use \ref resource_get_handle() to obtain the resource handle.
//! @return An opaque pointer to the loaded font, or a pointer to the default
//! (fallback) font if the specified font cannot be loaded.
//! @see Read the <a href="http://developer.getpebble.com/guides/pebble-apps/resources/">App
//! Resources</a> guide on how to embed a font into your app.
//! @note this may load a font from the flash peripheral into RAM.
GFont fonts_load_custom_font(ResHandle handle);

//! Unloads the specified custom font and frees the memory that is occupied by
//! it.
//! @note When an application exits, the system automatically unloads all fonts
//! that have been loaded.
//! @param font The font to unload.
void fonts_unload_custom_font(GFont font);

//! @} // group Fonts

//! @addtogroup TextDrawing Drawing Text
//!   \brief Functions to draw text into a graphics context
//!
//! See \ref GraphicsContext for more information about the graphics context.
//!
//! Other drawing functions and related documentation:
//! * \ref Drawing
//! * \ref PathDrawing
//! * \ref GraphicsTypes
//! @{

//! Text overflow mode controls the way text overflows when the string that is drawn does not fit
//! inside the area constraint.
//! @see graphics_draw_text
//! @see text_layer_set_overflow_mode
typedef enum {
  //! On overflow, wrap words to a new line below the current one. Once vertical space is consumed,
  //! the last line may be clipped.
  GTextOverflowModeWordWrap,
  //! On overflow, wrap words to a new line below the current one.
  //! Once vertical space is consumed, truncate as needed to fit a trailing ellipsis (...).
  //! Clipping may occur if the vertical space cannot accomodate the first line of text.
  GTextOverflowModeTrailingEllipsis,
  //! Acts like \ref GTextOverflowModeTrailingEllipsis, plus trims leading and trailing newlines,
  //! while treating all other newlines as spaces.
  GTextOverflowModeFill
} GTextOverflowMode;

//! Text aligment controls the way the text is aligned inside the box the text is drawn into.
//! @see graphics_draw_text
//! @see text_layer_set_text_alignment
typedef enum {
  //! Aligns the text to the left of the drawing box
  GTextAlignmentLeft,
  //! Aligns the text centered inside the drawing box
  GTextAlignmentCenter,
  //! Aligns the text to the right of the drawing box
  GTextAlignmentRight,
} GTextAlignment;

struct GTextAttributes;
typedef struct GTextAttributes GTextAttributes;

//! Creates an instance of GTextAttributes for advanced control when rendering text.
//! @return New instance of GTextAttributes
//! @see \ref graphics_draw_text
GTextAttributes *graphics_text_attributes_create(void);

//! Destroys a previously created instance of GTextAttributes
void graphics_text_attributes_destroy(GTextAttributes *text_attributes);

//! Restores text flow to the rectangular default.
//! @param text_attributes The attributes for which to disable text flow
//! @see graphics_text_attributes_enable_screen_text_flow
//! @see text_layer_restore_default_text_flow_and_paging
void graphics_text_attributes_restore_default_text_flow(GTextAttributes *text_attributes);

//! Enables text flow that follows the boundaries of the screen.
//! @param text_attributes The attributes for which text flow should be enabled
//! @param inset Additional amount of pixels to inset to the inside of the screen for text flow
//! calculation. Can be zero.
//! @see graphics_text_attributes_restore_default_text_flow
//! @see text_layer_enable_screen_text_flow_and_paging
void graphics_text_attributes_enable_screen_text_flow(GTextAttributes *text_attributes,
                                                      uint8_t inset);

//! Restores paging and locked content origin to the defaults.
//! @param text_attributes The attributes for which to restore paging and locked content origin
//! @see graphics_text_attributes_enable_paging
//! @see text_layer_restore_default_text_flow_and_paging
void graphics_text_attributes_restore_default_paging(GTextAttributes *text_attributes);

//! Enables paging and locks the text flow calculation to a fixed point on the screen.
//! @param text_attributes Attributes for which to enable paging and locked content origin
//! @param content_origin_on_screen Absolute coordinate on the screen where the text content
//!     starts before an animation or scrolling takes place. Usually the frame's origin of a layer
//!     in screen coordinates.
//! @param paging_on_screen Rectangle in absolute coordinates on the screen that describes where
//!     text content pages. Usually the container's absolute frame in screen coordinates.
//! @see graphics_text_attributes_restore_default_paging
//! @see graphics_text_attributes_enable_screen_text_flow
//! @see text_layer_enable_screen_text_flow_and_paging
//! @see layer_convert_point_to_screen
void graphics_text_attributes_enable_paging(GTextAttributes *text_attributes,
                                            GPoint content_origin_on_screen,
                                            GRect paging_on_screen);

//! Draw text into the current graphics context, using the context's current text color.
//! The text will be drawn inside a box with the specified dimensions and
//! configuration, with clipping occuring automatically.
//! @param ctx The destination graphics context in which to draw
//! @param text The zero terminated UTF-8 string to draw
//! @param font The font in which the text should be set
//! @param box The bounding box in which to draw the text. The first line of text will be drawn
//! against the top of the box.
//! @param overflow_mode The overflow behavior, in case the text is larger than what fits inside
//! the box.
//! @param alignment The horizontal alignment of the text
//! @param text_attributes Optional text attributes to describe the characteristics of the text
void graphics_draw_text(GContext *ctx, const char *text, GFont const font, const GRect box,
                        const GTextOverflowMode overflow_mode, const GTextAlignment alignment,
                        GTextAttributes *text_attributes);

//! Obtain the maximum size that a text with given font, overflow mode and alignment occupies
//! within a given rectangular constraint.
//! @param text The zero terminated UTF-8 string for which to calculate the size
//! @param font The font in which the text should be set while calculating the size
//! @param box The bounding box in which the text should be constrained
//! @param overflow_mode The overflow behavior, in case the text is larger than what fits
//! inside the box.
//! @param alignment The horizontal alignment of the text
//! @return The maximum size occupied by the text
//! @see app_graphics_text_layout_get_content_size_with_attributes
GSize graphics_text_layout_get_content_size(const char *text, GFont const font, const GRect box,
                                                const GTextOverflowMode overflow_mode,
                                                const GTextAlignment alignment);

//! Obtain the maximum size that a text with given font, overflow mode and alignment occupies
//! within a given rectangular constraint.
//! @param text The zero terminated UTF-8 string for which to calculate the size
//! @param font The font in which the text should be set while calculating the size
//! @param box The bounding box in which the text should be constrained
//! @param overflow_mode The overflow behavior, in case the text is larger than what fits
//! inside the box.
//! @param alignment The horizontal alignment of the text
//! @param text_attributes Optional text attributes to describe the characteristics of the text
//! @return The maximum size occupied by the text
//! @see app_graphics_text_layout_get_content_size
GSize graphics_text_layout_get_content_size_with_attributes(
  const char *text, GFont const font, const GRect box, const GTextOverflowMode overflow_mode,
  const GTextAlignment alignment, GTextAttributes *text_attributes);

//! @} // group TextDrawing

//! @} // group Graphics

//! @addtogroup Smartstrap
//! @{

//! The default request timeout in milliseconds (see \ref smartstrap_set_timeout).
#define SMARTSTRAP_TIMEOUT_DEFAULT 250

//! The service_id to specify in order to read/write raw data to the smartstrap.
#define SMARTSTRAP_RAW_DATA_SERVICE_ID 0

//! The attribute_id to specify in order to read/write raw data to the smartstrap.
#define SMARTSTRAP_RAW_DATA_ATTRIBUTE_ID 0

//! Convenience macro to switch between two expressions depending on smartstrap support.
//! On platforms with a smartstrap the first expression will be chosen, the second otherwise.
#define PBL_IF_SMARTSTRAP_ELSE(if_true, if_false) (if_false)

//! Error values which may be returned from the smartstrap APIs.
typedef enum {
  //! No error occured.
  SmartstrapResultOk = 0,
  //! Invalid function arguments were supplied.
  SmartstrapResultInvalidArgs,
  //! The smartstrap port is not present on this watch.
  SmartstrapResultNotPresent,
  //! A request is already pending on the specified attribute.
  SmartstrapResultBusy,
  //! Either a smartstrap is not connected or the connected smartstrap does not support the
  //! specified service.
  SmartstrapResultServiceUnavailable,
  //! The smartstrap reported that it does not support the requested attribute.
  SmartstrapResultAttributeUnsupported,
  //! A time-out occured during the request.
  SmartstrapResultTimeOut,
} SmartstrapResult;

//! A type representing a smartstrap ServiceId.
typedef uint16_t SmartstrapServiceId;

//! A type representing a smartstrap AttributeId.
typedef uint16_t SmartstrapAttributeId;

//! A type representing an attribute of a service provided by a smartstrap. This type is used when
//! issuing requests to the smartstrap.
typedef struct SmartstrapAttribute SmartstrapAttribute;

//! The type of function which is called after the smartstrap connection status changes.
//! @param service_id The ServiceId for which the availability changed.
//! @param is_available Whether or not this service is now available.
typedef void (*SmartstrapServiceAvailabilityHandler)(SmartstrapServiceId service_id,
                                                     bool is_available);

//! The type of function which can be called when a read request is completed.
//! @note Any write request made to the same attribute within this function will fail with
//! SmartstrapResultBusy.
//! @param attribute The attribute which was read.
//! @param result The result of the read.
//! @param data The data read from the smartstrap or NULL if the read was not successful.
//! @param length The length of the data or 0 if the read was not successful.
typedef void (*SmartstrapReadHandler)(SmartstrapAttribute *attribute, SmartstrapResult result,
                                      const uint8_t *data, size_t length);

//! The type of function which can be called when a write request is completed.
//! @param attribute The attribute which was written.
//! @param result The result of the write.
typedef void (*SmartstrapWriteHandler)(SmartstrapAttribute *attribute, SmartstrapResult result);

//! The type of function which can be called when the smartstrap sends a notification to the watch
//! @param attribute The attribute which the notification came from.
typedef void (*SmartstrapNotifyHandler)(SmartstrapAttribute *attribute);

//! Handlers which are passed to smartstrap_subscribe.
typedef struct {
  //! The connection handler is called after the connection state changes.
  SmartstrapServiceAvailabilityHandler availability_did_change;
  //! The read handler is called whenever a read is complete or the read times-out.
  SmartstrapReadHandler did_read;
  //! The did_write handler is called when a write has completed.
  SmartstrapWriteHandler did_write;
  //! The notified handler is called whenever a notification is received for an attribute.
  SmartstrapNotifyHandler notified;
} SmartstrapHandlers;

//! Subscribes handlers to be called after certain smartstrap events occur.
//! @note Registering an availability_did_change handler will cause power to be applied to the
//! smartstrap port and connection establishment to begin.
//! @see smartstrap_unsubscribe
//! @returns `SmartstrapResultNotPresent` if the watch does not have a smartstrap port or
//! `SmartstrapResultOk` otherwise.
SmartstrapResult smartstrap_subscribe(SmartstrapHandlers handlers);

//! Unsubscribes the handlers. The handlers will no longer be called, but in-flight requests will
//! otherwise be unaffected.
//! @note If power was being applied to the smartstrap port and there are no attributes have been
//! created (or they have all been destroyed), this will cause the smartstrap power to be turned
//! off.
void smartstrap_unsubscribe(void);

//! Changes the value of the timeout which is used for smartstrap requests. This timeout is started
//! after the request is completely sent to the smartstrap and will be canceled only if the entire
//! response is received before it triggers. The new timeout value will take affect only for
//! requests made after this API is called.
//! @param timeout_ms The duration of the timeout to set, in milliseconds.
//! @note The maximum allowed timeout is currently 1000ms. If a larger value is passed, it will be
//! internally lowered to the maximum.
//! @see SMARTSTRAP_TIMEOUT_DEFAULT
void smartstrap_set_timeout(uint16_t timeout_ms);

//! Creates and returns a SmartstrapAttribute for the specified service and attribute. This API
//! will allocate an internal buffer of the requested length on the app's heap.
//! @note Creating an attribute will result in power being applied to the smartstrap port (if it
//! isn't already) and connection establishment to begin.
//! @param service_id The ServiceId to create the attribute for.
//! @param attribute_id The AttributeId to create the attribute for.
//! @param buffer_length The length of the internal buffer which will be used to store the read
//! and write requests for this attribute.
//! @returns The newly created SmartstrapAttribute or NULL if an internal error occured or if the
//! specified length is greater than SMARTSTRAP_ATTRIBUTE_LENGTH_MAXIMUM.
SmartstrapAttribute *smartstrap_attribute_create(SmartstrapServiceId service_id,
                                                     SmartstrapAttributeId attribute_id,
                                                     size_t buffer_length);

//! Destroys a SmartstrapAttribute. No further handlers will be called for this attribute and it
//! may not be used for any future requests.
//! @param[in] attribute The SmartstrapAttribute which should be destroyed.
//! @note If power was being applied to the smartstrap port, no availability_did_change handler is
//! subscribed, and the last attribute is being destroyed, this will cause the smartstrap power to
//! be turned off.
void smartstrap_attribute_destroy(SmartstrapAttribute *attribute);

//! Checks whether or not the specified service is currently supported by a connected smartstrap.
//! @param service_id The SmartstrapServiceId of the service to check for availability.
//! @returns Whether or not the service is available.
bool smartstrap_service_is_available(SmartstrapServiceId service_id);

//! Returns the ServiceId which the attribute was created for (see \ref
//! smartstrap_attribute_create).
//! @param attribute The SmartstrapAttribute for which to obtain the service ID.
//! @returns The SmartstrapServiceId which the attribute was created with.
SmartstrapServiceId smartstrap_attribute_get_service_id(SmartstrapAttribute *attribute);

//! Gets the AttributeId which the attribute was created for (see \ref smartstrap_attribute_create).
//! @param attribute The SmartstrapAttribute for which to obtain the attribute ID.
//! @returns The SmartstrapAttributeId which the attribute was created with.
SmartstrapAttributeId smartstrap_attribute_get_attribute_id(SmartstrapAttribute *attribute);

//! Performs a read request for the specified attribute. The `did_read` callback will be called when
//! the response is received from the smartstrap or when an error occurs.
//! @param attribute The attribute to be perform the read request on.
//! @returns `SmartstrapResultOk` if the read operation was started. The `did_read` callback will
//! be called once the read request has been completed.
SmartstrapResult smartstrap_attribute_read(SmartstrapAttribute *attribute);

//! Begins a write request for the specified attribute and returns a buffer into which the app
//! should write the data before calling smartstrap_attribute_end_write.
//! @note The buffer must not be used after smartstrap_attribute_end_write is called.
//! @param[in] attribute The attribute to begin writing for.
//! @param[out] buffer The buffer to write the data into.
//! @param[out] buffer_length The length of the buffer in bytes.
//! @returns `SmartstrapResultOk` if a write operation was started and the `buffer` and
//! `buffer_length` parameters were set, or an error otherwise.
SmartstrapResult smartstrap_attribute_begin_write(SmartstrapAttribute *attribute,
                                                      uint8_t **buffer, size_t *buffer_length);

//! This should be called by the app when it is done writing to the buffer provided by
//! smartstrap_begin_write and the data is ready to be sent to the smartstrap.
//! @param[in] attribute The attribute to begin writing for.
//! @param write_length The length of the data to be written, in bytes.
//! @param request_read Whether or not a read request on this attribute should be
//! automatically triggered following a successful write request.
//! @returns `SmartstrapResultOk` if a write operation was queued to be sent to the smartstrap. The
//! `did_write` handler will be called when the request is written to the smartstrap, and if
//! `request_read` was set to true, the `did_read` handler will be called when the read is complete.
SmartstrapResult smartstrap_attribute_end_write(SmartstrapAttribute *attribute,
                                                    size_t write_length, bool request_read);

//! @} // group Smartstrap

//! @addtogroup UI
//! @{

//! @addtogroup Clicks
//! @{

//! Reference to opaque click recognizer
//! When a \ref ClickHandler callback is called, the recognizer that fired the handler is passed in.
//! @see \ref ClickHandler
//! @see \ref click_number_of_clicks_counted()
//! @see \ref click_recognizer_get_button_id()
//! @see \ref click_recognizer_is_repeating()
typedef void *ClickRecognizerRef;

//! Function signature of the callback that handles a recognized click pattern
//! @param recognizer The click recognizer that detected a "click" pattern
//! @param context Pointer to application specified data (see \ref window_set_click_config_provider_with_context() and
//! \ref window_set_click_context()). This defaults to the window.
//! @see \ref ClickConfigProvider
typedef void (*ClickHandler)(ClickRecognizerRef recognizer, void *context);

//! This callback is called every time the window becomes visible (and when you call \ref window_set_click_config_provider() if
//! the window is already visible).
//!
//! Subscribe to click events using
//!   \ref window_single_click_subscribe()
//!   \ref window_single_repeating_click_subscribe()
//!   \ref window_multi_click_subscribe()
//!   \ref window_long_click_subscribe()
//!   \ref window_raw_click_subscribe()
//! These subscriptions will get used by the click recognizers of each of the 4 buttons.
//! @param context Pointer to application specific data (see \ref window_set_click_config_provider_with_context()).
typedef void (*ClickConfigProvider)(void *context);

//! Gets the click count.
//! You can use this inside a click handler implementation to get the click count for multi_click
//! and (repeated) click events.
//! @param recognizer The click recognizer for which to get the click count
//! @return The number of consecutive clicks, and for auto-repeating the number of repetitions.
uint8_t click_number_of_clicks_counted(ClickRecognizerRef recognizer);

//! Gets the button identifier.
//! You can use this inside a click handler implementation to get the button id for the click event.
//! @param recognizer The click recognizer for which to get the button id that caused the click event
//! @return the ButtonId of the click recognizer
ButtonId click_recognizer_get_button_id(ClickRecognizerRef recognizer);

//! Is this a repeating click.
//! You can use this inside a click handler implementation to find out whether this is a repeating click or not.
//! @param recognizer The click recognizer for which to find out whether this is a repeating click.
//! @return true if this is a repeating click.
bool click_recognizer_is_repeating(ClickRecognizerRef recognizer);

//! @} // group Clicks

//! @addtogroup Layer Layers
//! \brief User interface layers for displaying graphic components
//!
//! Layers are objects that can be displayed on a Pebble watchapp window, enabling users to see
//! visual objects, like text or images. Each layer stores the information about its state
//! necessary to draw or redraw the object that it represents and uses graphics routines along with
//! this state to draw itself when asked. Layers can be used to display various graphics.
//!
//! Layers are the basic building blocks for your application UI. Layers can be nested inside each other.
//! Every window has a root layer which is always the topmost layer.
//! You provide a function that is called to draw the content of the layer when needed; or
//! you can use standard layers that are provided by the system, such as text layer, image layer,
//! menu layer, action bar layer, and so on.
//!
//! The Pebble layer hierarchy is the list of things that need to be drawn to the screen.
//! Multiple layers can be arranged into a hierarchy. This enables ordering (front to back),
//! layout and hierarchy. Through relative positioning, visual objects that are grouped together by
//! adding them into the same layer can be moved all at once. This means that the child layers
//! will move accordingly. If a parent layer has clipping enabled, all the children will be clipped
//! to the frame of the parent.
//!
//! Pebble OS provides convenience layers with built-in logic for displaying different graphic
//! components, like text and bitmap layers.
//!
//! Refer to the \htmlinclude UiFramework.html (chapter "Layers") for a conceptual overview
//! of Layers and relevant code examples.
//!
//! The Modules listed here contain what can be thought of conceptually as subclasses of Layer. The
//! listed types can be safely type-casted to `Layer` (or `Layer *` in case of a pointer).
//! The `layer_...` functions can then be used with the data structures of these subclasses.
//! <br/>For example, the following is legal:
//! \code{.c}
//! TextLayer *text_layer;
//! ...
//! layer_set_hidden((Layer *)text_layer, true);
//! \endcode
//! @{

struct Layer;
typedef struct Layer Layer;

//! Function signature for a Layer's render callback (the name of the type
//! is derived from the words 'update procedure').
//! The system will call the `.update_proc` callback whenever the Layer needs
//! to be rendered.
//! @param layer The layer that needs to be rendered
//! @param ctx The destination graphics context to draw into
//! @see \ref Graphics
//! @see \ref layer_set_update_proc()
typedef void (*LayerUpdateProc)(struct Layer *layer, GContext* ctx);

//! Creates a layer on the heap and sets its frame and bounds.
//! Default values:
//! * `bounds` : origin (0, 0) and a size equal to the frame that is passed in.
//! * `clips` : `true`
//! * `hidden` : `false`
//! * `update_proc` : `NULL` (draws nothing)
//! @param frame The frame at which the layer should be initialized.
//! @see \ref layer_set_frame()
//! @see \ref layer_set_bounds()
//! @return A pointer to the layer. `NULL` if the layer could not
//! be created
Layer* layer_create(GRect frame);

//! Creates a layer on the heap with extra space for callback data, and set its frame andbounds.
//! Default values:
//! * `bounds` : origin (0, 0) and a size equal to the frame that is passed in.
//! * `clips` : `true`
//! * `hidden` : `false`
//! * `update_proc` : `NULL` (draws nothing)
//! @param frame The frame at which the layer should be initialized.
//! @param data_size The size (in bytes) of memory to allocate for callback data.
//! @see \ref layer_create()
//! @see \ref layer_set_frame()
//! @see \ref layer_set_bounds()
//! @return A pointer to the layer. `NULL` if the layer could not be created
Layer* layer_create_with_data(GRect frame, size_t data_size);

//! Destroys a layer previously created by layer_create
void layer_destroy(Layer* layer);

//! Marks the complete layer as "dirty", awaiting to be asked by the system to redraw itself.
//! Typically, this function is called whenever state has changed that affects what the layer
//! is displaying.
//! * The layer's `.update_proc` will not be called before this function returns,
//! but will be called asynchronously, shortly.
//! * Internally, a call to this function will schedule a re-render of the window that the
//! layer belongs to. In effect, all layers in that window's layer hierarchy will be asked to redraw.
//! * If an earlier re-render request is still pending, this function is a no-op.
//! @param layer The layer to mark dirty
void layer_mark_dirty(Layer *layer);

//! Sets the layer's render function.
//! The system will call the `update_proc` automatically when the layer needs to redraw itself, see
//! also \ref layer_mark_dirty().
//! @param layer Pointer to the layer structure.
//! @param update_proc Pointer to the function that will be called when the layer needs to be rendered.
//! Typically, one performs a series of drawing commands in the implementation of the `update_proc`,
//! see \ref Drawing, \ref PathDrawing and \ref TextDrawing.
void layer_set_update_proc(Layer *layer, LayerUpdateProc update_proc);

//! Sets the frame of the layer, which is it's bounding box relative to the coordinate
//! system of its parent layer.
//! The size of the layer's bounds will be extended automatically, so that the bounds
//! cover the new frame.
//! @param layer The layer for which to set the frame
//! @param frame The new frame
//! @see \ref layer_set_bounds()
void layer_set_frame(Layer *layer, GRect frame);

//! Gets the frame of the layer, which is it's bounding box relative to the coordinate
//! system of its parent layer.
//! If the frame has changed, \ref layer_mark_dirty() will be called automatically.
//! @param layer The layer for which to get the frame
//! @return The frame of the layer
//! @see layer_set_frame
GRect layer_get_frame(const Layer *layer);

//! Sets the bounds of the layer, which is it's bounding box relative to its frame.
//! If the bounds has changed, \ref layer_mark_dirty() will be called automatically.
//! @param layer The layer for which to set the bounds
//! @param bounds The new bounds
//! @see \ref layer_set_frame()
void layer_set_bounds(Layer *layer, GRect bounds);

//! Gets the bounds of the layer
//! @param layer The layer for which to get the bounds
//! @return The bounds of the layer
//! @see layer_set_bounds
GRect layer_get_bounds(const Layer *layer);

//! Get the largest unobstructed bounds rectangle of a layer.
//! @param layer The layer for which to get the unobstructed bounds.
//! @return The unobstructed bounds of the layer.
//! @see UnobstructedArea
#define layer_get_unobstructed_bounds(layer) layer_get_bounds(layer)

//! Converts a point from the layer's local coordinate system to screen coordinates.
//! @note If the layer isn't part of the view hierarchy the result is undefined.
//! @param layer The view whose coordinate system will be used to convert the value to the screen.
//! @param point A point specified in the local coordinate system (bounds) of the layer.
//! @return The point converted to the coordinate system of the screen.
GPoint layer_convert_point_to_screen(const Layer *layer, GPoint point);

//! Converts a rectangle from the layer's local coordinate system to screen coordinates.
//! @note If the layer isn't part of the view hierarchy the result is undefined.
//! @param layer The view whose coordinate system will be used to convert the value to the screen.
//! @param rect A rectangle specified in the local coordinate system (bounds) of the layer.
//! @return The rectangle converted to the coordinate system of the screen.
GRect layer_convert_rect_to_screen(const Layer *layer, GRect rect);

//! Gets the window that the layer is currently attached to.
//! @param layer The layer for which to get the window
//! @return The window that this layer is currently attached to, or `NULL` if it has
//! not been added to a window's layer hierarchy.
//! @see \ref window_get_root_layer()
//! @see \ref layer_add_child()
struct Window *layer_get_window(const Layer *layer);

//! Removes the layer from its current parent layer
//! If removed successfully, the child's parent layer will be marked dirty
//! automatically.
//! @param child The layer to remove
void layer_remove_from_parent(Layer *child);

//! Removes child layers from given layer
//! If removed successfully, the child's parent layer will be marked dirty
//! automatically.
//! @param parent The layer from which to remove all child layers
void layer_remove_child_layers(Layer *parent);

//! Adds the child layer to a given parent layer, making it appear
//! in front of its parent and in front of any existing child layers
//! of the parent.
//! If the child layer was already part of a layer hierarchy, it will
//! be removed from its old parent first.
//! If added successfully, the parent (and children) will be marked dirty
//! automatically.
//! @param parent The layer to which to add the child layer
//! @param child The layer to add to the parent layer
void layer_add_child(Layer *parent, Layer *child);

//! Inserts the layer as a sibling behind another layer. If the layer to insert was
//! already part of a layer hierarchy, it will be removed from its old parent first.
//! The below_layer has to be a child of a parent layer,
//! otherwise this function will be a noop.
//! If inserted successfully, the parent (and children) will be marked dirty
//! automatically.
//! @param layer_to_insert The layer to insert into the hierarchy
//! @param below_sibling_layer The layer that will be used as the sibling layer
//! above which the insertion will take place
void layer_insert_below_sibling(Layer *layer_to_insert, Layer *below_sibling_layer);

//! Inserts the layer as a sibling in front of another layer.
//! The above_layer has to be a child of a parent layer,
//! otherwise this function will be a noop.
//! If inserted successfully, the parent (and children) will be marked dirty
//! automatically.
//! @param layer_to_insert The layer to insert into the hierarchy
//! @param above_sibling_layer The layer that will be used as the sibling layer
//! below which the insertion will take place
void layer_insert_above_sibling(Layer *layer_to_insert, Layer *above_sibling_layer);

//! Sets the visibility of the layer.
//! If the visibility has changed, \ref layer_mark_dirty() will be called automatically
//! on the parent layer.
//! @param layer The layer for which to set the visibility
//! @param hidden Supply `true` to make the layer hidden, or `false` to make it
//! non-hidden.
void layer_set_hidden(Layer *layer, bool hidden);

//! Gets the visibility of the layer.
//! @param layer The layer for which to get the visibility
//! @return True if the layer is hidden, false if it is not hidden.
bool layer_get_hidden(const Layer *layer);

//! Sets whether clipping is enabled for the layer. If enabled, whatever the layer _and
//! its children_ will draw using their `.update_proc` callbacks, will be clipped by the
//! this layer's frame.
//! If the clipping has changed, \ref layer_mark_dirty() will be called automatically.
//! @param layer The layer for which to set the clipping property
//! @param clips Supply `true` to make the layer clip to its frame, or `false`
//! to make it non-clipping.
void layer_set_clips(Layer *layer, bool clips);

//! Gets whether clipping is enabled for the layer.  If enabled, whatever the layer _and
//! its children_ will draw using their `.update_proc` callbacks, will be clipped by the
//! this layer's frame.
//! @param layer The layer for which to get the clipping property
//! @return True if clipping is enabled for the layer, false if clipping is not enabled for
//! the layer.
bool layer_get_clips(const Layer *layer);

//! Gets the data from a layer that has been created with an extra data region.
//! @param layer The layer to get the data region from.
//! @return A void pointer to the data region.
void* layer_get_data(const Layer *layer);

//! @} // group Layer

//! @addtogroup Window
//! \brief The basic building block of the user interface
//!
//! Windows are the top-level elements in the UI hierarchy and the basic building blocks for a Pebble
//! UI. A single window is always displayed at a time on Pebble, with the exception of when animating
//! from one window to the other, which, in that case, is managed by the window stack. You can stack
//! windows on top of each other, but only the topmost window will be visible.
//!
//! Users wearing a Pebble typically interact with the content and media displayed in a window, clicking
//! and pressing buttons on the watch, depending on what they see and wish to respond to in a window.
//!
//! Windows serve to display a hierarchy of layers on the screen and handle user input. When a window is
//! visible, its root Layer (and all its child layers) are drawn onto the screen automatically.
//!
//! You need a window, which always fills the entire screen, to display images, text, and graphics in
//! your Pebble app. A layer by itself doesn’t display on Pebble; it must be in the current window’s
//! layer hierarchy to be visible.
//!
//! The Window Stack serves as the global manager of what window is presented and makes sure that input
//! events are forwarded to the topmost window.
//!
//! Refer to the \htmlinclude UiFramework.html (chapter "Window") for a conceptual
//! overview of Window, the Window Stack and relevant code examples.
//! @{

struct Window;
typedef struct Window Window;

//! Function signature for a handler that deals with transition events of a window.
//! @see WindowHandlers
//! @see \ref window_set_window_handlers()
typedef void (*WindowHandler)(struct Window *window);

//! WindowHandlers
//! These handlers are called by the \ref WindowStack as windows get pushed on / popped.
//! All these handlers use \ref WindowHandler as their function signature.
//! @see \ref window_set_window_handlers()
//! @see \ref WindowStack
typedef struct WindowHandlers {
  //! Called when the window is pushed to the screen when it's not loaded.
  //! This is a good moment to do the layout of the window.
  WindowHandler load;

  //! Called when the window comes on the screen (again). E.g. when
  //! second-top-most window gets revealed (again) after popping the top-most
  //! window, but also when the window is pushed for the first time. This is a
  //! good moment to start timers related to the window, or reset the UI, etc.
  WindowHandler appear;

  //! Called when the window leaves the screen, e.g. when another window
  //! is pushed, or this window is popped. Good moment to stop timers related
  //! to the window.
  WindowHandler disappear;

  //! Called when the window is deinited, but could be used in the future to
  //! free resources bound to windows that are not on screen.
  WindowHandler unload;
} WindowHandlers;

//! Creates a new Window on the heap and initalizes it with the default values.
//!
//! * Background color : `GColorWhite`
//! * Root layer's `update_proc` : function that fills the window's background using `background_color`.
//! * `click_config_provider` : `NULL`
//! * `window_handlers` : all `NULL`
//! @return A pointer to the window. `NULL` if the window could not
//! be created
Window* window_create(void);

//! Destroys a Window previously created by window_create.
void window_destroy(Window* window);

//! Sets the click configuration provider callback function on the window.
//! This will automatically setup the input handlers of the window as well to use
//! the click recognizer subsystem.
//! @param window The window for which to set the click config provider
//! @param click_config_provider The callback that will be called to configure the click recognizers with the window
//! @see Clicks
//! @see ClickConfigProvider
void window_set_click_config_provider(Window *window, ClickConfigProvider click_config_provider);

//! Same as window_set_click_config_provider(), but will assign a custom context pointer
//! (instead of the window pointer) that will be passed into the ClickHandler click event handlers.
//! @param window The window for which to set the click config provider
//! @param click_config_provider The callback that will be called to configure the click recognizers with the window
//! @param context Pointer to application specific data that will be passed to the click configuration provider callback (defaults to the window).
//! @see Clicks
//! @see window_set_click_config_provider
void window_set_click_config_provider_with_context(Window *window, ClickConfigProvider click_config_provider, void *context);

//! Gets the current click configuration provider of the window.
//! @param window The window for which to get the click config provider
ClickConfigProvider window_get_click_config_provider(const Window *window);

//! Gets the current click configuration provider context of the window.
//! @param window The window for which to get the click config provider context
void *window_get_click_config_context(Window *window);

//! Sets the window handlers of the window.
//! These handlers get called e.g. when the user enters or leaves the window.
//! @param window The window for which to set the window handlers
//! @param handlers The handlers for the specified window
//! @see \ref WindowHandlers
void window_set_window_handlers(Window *window, WindowHandlers handlers);

//! Gets the root Layer of the window.
//! The root layer is the layer at the bottom of the layer hierarchy for this window.
//! It is the window's "canvas" if you will. By default, the root layer only draws
//! a solid fill with the window's background color.
//! @param window The window for which to get the root layer
//! @return The window's root layer
struct Layer* window_get_root_layer(const Window *window);

//! Sets the background color of the window, which is drawn automatically by the
//! root layer of the window.
//! @param window The window for which to set the background color
//! @param background_color The new background color
//! @see \ref window_get_root_layer()
void window_set_background_color(Window *window, GColor background_color);

//! Gets whether the window has been loaded.
//! If a window is loaded, its `.load` handler has been called (and the `.unload` handler
//! has not been called since).
//! @return true if the window is currently loaded or false if not.
//! @param window The window to query its loaded status
//! @see \ref WindowHandlers
bool window_is_loaded(Window *window);

//! Sets a pointer to developer-supplied data that the window uses, to
//! provide a means to access the data at later times in one of the window event handlers.
//! @see window_get_user_data
//! @param window The window for which to set the user data
//! @param data A pointer to user data.
void window_set_user_data(Window *window, void *data);

//! Gets the pointer to developer-supplied data that was previously
//! set using window_set_user_data().
//! @see window_set_user_data
//! @param window The window for which to get the user data
void* window_get_user_data(const Window *window);

//! Subscribe to single click events.
//! @note Must be called from the \ref ClickConfigProvider.
//! @note \ref window_single_click_subscribe() and \ref window_single_repeating_click_subscribe() conflict, and cannot both be used on the same button.
//! @note When there is a multi_click and/or long_click setup, there will be a delay before the single click
//! @param button_id The button events to subscribe to.
//! @param handler The \ref ClickHandler to fire on this event.
//! handler will get fired. On the other hand, when there is no multi_click nor long_click setup, the single click handler will fire directly on button down.
//! @see ButtonId
//! @see Clicks
//! @see window_single_repeating_click_subscribe
void window_single_click_subscribe(ButtonId button_id, ClickHandler handler);

//! Subscribe to single click event, with a repeat interval. A single click is detected every time "repeat_interval_ms" has been reached.
//! @note Must be called from the \ref ClickConfigProvider.
//! @note \ref window_single_click_subscribe() and \ref window_single_repeating_click_subscribe() conflict, and cannot both be used on the same button.
//! @note The back button cannot be overridden with a repeating click.
//! @param button_id The button events to subscribe to.
//! @param repeat_interval_ms When holding down, how many milliseconds before the handler is fired again.
//! A value of 0ms means "no repeat timer". The minimum is 30ms, and values below will be disregarded.
//! If there is a long-click handler subscribed on this button, `repeat_interval_ms` will not be used.
//! @param handler The \ref ClickHandler to fire on this event.
//! @see window_single_click_subscribe
void window_single_repeating_click_subscribe(ButtonId button_id, uint16_t repeat_interval_ms, ClickHandler handler);

//! Subscribe to multi click events.
//! @note Must be called from the \ref ClickConfigProvider.
//! @param button_id The button events to subscribe to.
//! @param min_clicks Minimum number of clicks before handler is fired. Defaults to 2.
//! @param max_clicks Maximum number of clicks after which the click counter is reset. A value of 0 means use "min" also as "max".
//! @param timeout The delay after which a sequence of clicks is considered finished, and the click counter is reset. A value of 0 means to use the system default 300ms.
//! @param last_click_only Defaults to false. When true, only the handler for the last multi-click is called.
//! @param handler The \ref ClickHandler to fire on this event. Fired for multi-clicks, as "filtered" by the `last_click_only`, `min`, and `max` parameters.
void window_multi_click_subscribe(ButtonId button_id, uint8_t min_clicks, uint8_t max_clicks, uint16_t timeout, bool last_click_only, ClickHandler handler);

//! Subscribe to long click events.
//! @note Must be called from the \ref ClickConfigProvider.
//! @note The back button cannot be overridden with a long click.
//! @param button_id The button events to subscribe to.
//! @param delay_ms Milliseconds after which "handler" is fired. A value of 0 means to use the system default 500ms.
//! @param down_handler The \ref ClickHandler to fire as soon as the button has been held for `delay_ms`. This may be NULL to have no down handler.
//! @param up_handler The \ref ClickHandler to fire on the release of a long click. This may be NULL to have no up handler.
void window_long_click_subscribe(ButtonId button_id, uint16_t delay_ms, ClickHandler down_handler, ClickHandler up_handler);

//! Subscribe to raw click events.
//! @note Must be called from within the \ref ClickConfigProvider.
//! @note The back button cannot be overridden with a raw click.
//! @param button_id The button events to subscribe to.
//! @param down_handler The \ref ClickHandler to fire as soon as the button has been pressed. This may be NULL to have no down handler.
//! @param up_handler The \ref ClickHandler to fire on the release of the button. This may be NULL to have no up handler.
//! @param context If this context is not NULL, it will override the general context.
void window_raw_click_subscribe(ButtonId button_id, ClickHandler down_handler, ClickHandler up_handler, void *context);

//! Set the context that will be passed to handlers for the given button's events. By default the context passed to handlers
//! is equal to the \ref ClickConfigProvider context (defaults to the window).
//! @note Must be called from within the \ref ClickConfigProvider.
//! @param button_id The button to set the context for.
//! @param context Set the context that will be passed to handlers for the given button's events.
void window_set_click_context(ButtonId button_id, void *context);

//! @} // group Window

//! @addtogroup WindowStack Window Stack
//! \brief The multiple window manager
//!
//! In Pebble OS, the window stack serves as the global manager of what window is presented,
//! ensuring that input events are forwarded to the topmost window.
//! The navigation model of Pebble centers on the concept of a vertical “stack” of windows, similar
//! to mobile app interactions.
//!
//! In working with the Window Stack API, the basic operations include push and pop. When an app wants to
//! display a new window, it pushes a new window onto the stack. This appears like a window sliding in
//! from the right. As an app is closed, the window is popped off the stack and disappears.
//!
//! For more complicated operations, involving multiple windows, you can determine which windows reside
//! on the stack, using window_stack_contains_window() and remove any specific window, using window_stack_remove().
//!
//! Refer to the \htmlinclude UiFramework.html (chapter "Window Stack") for a conceptual overview
//! of the window stack and relevant code examples.
//!
//! Also see the \ref WindowHandlers of a \ref Window for the callbacks that can be added to a window
//! in order to act upon window stack transitions.
//!
//! @{

//! Pushes the given window on the window navigation stack,
//! on top of the current topmost window of the app.
//! @param window The window to push on top
//! @param animated Pass in `true` to animate the push using a sliding animation,
//! or `false` to skip the animation.
void window_stack_push(Window *window, bool animated);

//! Pops the topmost window on the navigation stack
//! @param animated See \ref window_stack_remove()
//! @return The window that is popped, or NULL if there are no windows to pop.
Window* window_stack_pop(bool animated);

//! Pops all windows.
//! See \ref window_stack_remove() for a description of the `animated` parameter and notes.
void window_stack_pop_all(const bool animated);

//! Removes a given window from the window stack
//! that belongs to the app task.
//! @note If there are no windows for the app left on the stack, the app
//! will be killed by the system, shortly. To avoid this, make sure
//! to push another window shortly after or before removing the last window.
//! @param window The window to remove. If the window is NULL or if it
//! is not on the stack, this function is a no-op.
//! @param animated Pass in `true` to animate the removal of the window using
//! a side-to-side sliding animation to reveal the next window.
//! This is only used in case the window happens to be on top of the window
//! stack (thus visible).
//! @return True if window was successfully removed, false otherwise.
bool window_stack_remove(Window *window, bool animated);

//! Gets the topmost window on the stack that belongs to the app.
//! @return The topmost window on the stack that belongs to the app or
//! NULL if no app window could be found.
Window* window_stack_get_top_window(void);

//! Checks if the window is on the window stack
//! @param window The window to look for on the window stack
//! @return true if the window is currently on the window stack.
bool window_stack_contains_window(Window *window);

//! @} // group WindowStack

//! @addtogroup Animation
//!   \brief Abstract framework to create arbitrary animations
//!
//! The Animation framework provides your Pebble app with an base layer to create arbitrary
//! animations. The simplest way to work with animations is to use the layer frame
//! \ref PropertyAnimation, which enables you to move a Layer around on the screen.
//! Using animation_set_implementation(), you can implement a custom animation.
//!
//! Refer to the \htmlinclude UiFramework.html (chapter "Animation") for a conceptual overview
//! of the animation framework and on how to write custom animations.
//! @{

struct Animation;
typedef struct Animation Animation;

//! The type used to represent how far an animation has progressed. This is passed to the
//! animation's update handler
typedef int32_t AnimationProgress;

//! Values that are used to indicate the different animation curves,
//! which determine the speed at which the animated value(s) change(s).
typedef enum {
  //! Linear curve: the velocity is constant.
  AnimationCurveLinear = 0,
  //! Bicubic ease-in: accelerate from zero velocity
  AnimationCurveEaseIn = 1,
  //! Bicubic ease-in: decelerate to zero velocity
  AnimationCurveEaseOut = 2,
  //! Bicubic ease-in-out: accelerate from zero velocity, decelerate to zero velocity
  AnimationCurveEaseInOut = 3,
  AnimationCurveDefault = AnimationCurveEaseInOut,
  //! Custom (user-provided) animation curve
  AnimationCurveCustomFunction = 4,
  //! User-provided interpolation function
  AnimationCurveCustomInterpolationFunction = 5,
  // Two more Reserved for forward-compatibility use.
  AnimationCurve_Reserved1 = 6,
  AnimationCurve_Reserved2 = 7,
} AnimationCurve;

//! Creates a new Animation on the heap and initalizes it with the default values.
//!
//! * Duration: 250ms,
//! * Curve: \ref AnimationCurveEaseInOut (ease-in-out),
//! * Delay: 0ms,
//! * Handlers: `{NULL, NULL}` (none),
//! * Context: `NULL` (none),
//! * Implementation: `NULL` (no implementation),
//! * Scheduled: no
//! @return A pointer to the animation. `NULL` if the animation could not
//! be created
Animation * animation_create(void);

//! Destroys an Animation previously created by animation_create.
//! @return true if successful, false on failure
bool animation_destroy(Animation *animation);

//! Constant to indicate "infinite" duration.
//! This can be used with \ref animation_set_duration() to indicate that the animation
//! should run indefinitely. This is useful when implementing for example a frame-by-frame
//! simulation that does not have a clear ending (e.g. a game).
//! @note Note that `distance_normalized` parameter that is passed
//! into the `.update` implementation is meaningless in when an infinite duration is used.
//! @note This can be returned by animation_get_duration (if the play count is infinite)
#define ANIMATION_DURATION_INFINITE UINT32_MAX

//! Constant to indicate infinite play count.
//! Can be passed to \ref animation_set_play_count() to repeat indefinitely.
//! @note This can be returned by \ref animation_get_play_count().
#define ANIMATION_PLAY_COUNT_INFINITE UINT32_MAX

//! The normalized distance at the start of the animation.
#define ANIMATION_NORMALIZED_MIN 0

//! The normalized distance at the end of the animation.
#define ANIMATION_NORMALIZED_MAX 65535

Animation *animation_clone(Animation *from);

//! Create a new sequence animation from a list of 2 or more other animations. The returned
//! animation owns the animations that were provided as arguments and no further write operations
//! on those handles are allowed. The variable length argument list must be terminated with a NULL
//! ptr
//! @note the maximum number of animations that can be supplied to this method is 20
//! @param animation_a the first required component animation
//! @param animation_b the second required component animation
//! @param animation_c either the third component, or NULL if only adding 2 components
//! @return The newly created sequence animation
Animation *animation_sequence_create(Animation *animation_a, Animation *animation_b,
                                     Animation *animation_c, ...);

//! An alternate form of animation_sequence_create() that accepts an array of other animations.
//! @note the maximum number of elements allowed in animation_array is 256
//! @param animation_array an array of component animations to include
//! @param array_len the number of elements in the animation_array
//! @return The newly created sequence animation
Animation *animation_sequence_create_from_array(Animation **animation_array, uint32_t array_len);

//! Create a new spawn animation from a list of 2 or more other animations. The returned
//! animation owns the animations that were provided as arguments and no further write operations
//! on those handles are allowed. The variable length argument list must be terminated with a NULL
//! ptr
//! @note the maximum number of animations that can be supplied to this method is 20
//! @param animation_a the first required component animation
//! @param animation_b the second required component animation
//! @param animation_c either the third component, or NULL if only adding 2 components
//! @return The newly created spawn animation or NULL on failure
Animation *animation_spawn_create(Animation *animation_a, Animation *animation_b,
                                  Animation *animation_c, ...);

//! An alternate form of animation_spawn_create() that accepts an array of other animations.
//! @note the maximum number of elements allowed in animation_array is 256
//! @param animation_array an array of component animations to include
//! @param array_len the number of elements in the animation_array
//! @return The newly created spawn animation or NULL on failure
Animation *animation_spawn_create_from_array(Animation **animation_array, uint32_t array_len);

//! Seek to a specific location in the animation. Only forward seeking is allowed. Returns true
//! if successful, false if the passed in seek location is invalid.
//! @param animation the animation for which to set the elapsed.
//! @param elapsed_ms the new elapsed time in milliseconds
//! @return true if successful, false if the requested elapsed is invalid.
bool animation_set_elapsed(Animation *animation, uint32_t elapsed_ms);

//! Get the current location in the animation.
//! @note The animation must be scheduled to get the elapsed time. If it is not schedule,
//! this method will return false.
//! @param animation The animation for which to fetch the elapsed.
//! @param[out] elapsed_ms pointer to variable that will contain the elapsed time in milliseconds
//! @return true if successful, false on failure
bool animation_get_elapsed(Animation *animation, int32_t *elapsed_ms);

//! Set an animation to run in reverse (or forward)
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation the animation to operate on
//! @param reverse set to true to run in reverse, false to run forward
//! @return true if successful, false on failure
bool animation_set_reverse(Animation *animation, bool reverse);

//! Get the reverse setting of an animation
//! @param animation The animation for which to get the setting
//! @return the reverse setting
bool animation_get_reverse(Animation *animation);

//! Set an animation to play N times. The default is 1.
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation the animation to set the play count of
//! @param play_count number of times to play this animation. Set to ANIMATION_PLAY_COUNT_INFINITE
//! to make an animation repeat indefinitely.
//! @return true if successful, false on failure
bool animation_set_play_count(Animation *animation, uint32_t play_count);

//! Get the play count of an animation
//! @param animation The animation for which to get the setting
//! @return the play count
uint32_t animation_get_play_count(Animation *animation);

//! Sets the time in milliseconds that an animation takes from start to finish.
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation The animation for which to set the duration.
//! @param duration_ms The duration in milliseconds of the animation. This excludes
//! any optional delay as set using \ref animation_set_delay().
//! @return true if successful, false on failure
bool animation_set_duration(Animation *animation, uint32_t duration_ms);

//! Get the static duration of an animation from start to end (ignoring how much has already
//! played, if any).
//! @param animation The animation for which to get the duration
//! @param include_delay if true, include the delay time
//! @param include_play_count if true, incorporate the play_count
//! @return the duration, in milliseconds. This includes any optional delay a set using
//! \ref animation_set_delay.
uint32_t animation_get_duration(Animation *animation, bool include_delay, bool include_play_count);

//! Sets an optional delay for the animation.
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation The animation for which to set the delay.
//! @param delay_ms The delay in milliseconds that the animation system should
//! wait from the moment the animation is scheduled to starting the animation.
//! @return true if successful, false on failure
bool animation_set_delay(Animation *animation, uint32_t delay_ms);

//! Get the delay of an animation in milliseconds
//! @param animation The animation for which to get the setting
//! @return the delay in milliseconds
uint32_t animation_get_delay(Animation *animation);

//! Sets the animation curve for the animation.
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation The animation for which to set the curve.
//! @param curve The type of curve.
//! @see AnimationCurve
//! @return true if successful, false on failure
bool animation_set_curve(Animation *animation, AnimationCurve curve);

//! Gets the animation curve for the animation.
//! @param animation The animation for which to get the curve.
//! @return The type of curve.
AnimationCurve animation_get_curve(Animation *animation);

//! The function pointer type of a custom animation curve.
//! @param linear_distance The linear normalized animation distance to be curved.
//! @see animation_set_custom_curve
typedef AnimationProgress (*AnimationCurveFunction)(AnimationProgress linear_distance);

//! Sets a custom animation curve function.
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation The animation for which to set the curve.
//! @param curve_function The custom animation curve function.
//! @see AnimationCurveFunction
//! @return true if successful, false on failure
bool animation_set_custom_curve(Animation *animation, AnimationCurveFunction curve_function);

//! Gets the custom animation curve function for the animation.
//! @param animation The animation for which to get the curve.
//! @return The custom animation curve function for the given animation. NULL if not set.
AnimationCurveFunction animation_get_custom_curve(Animation *animation);

//! The function pointer type of the handler that will be called when an animation is started,
//! just before updating the first frame of the animation.
//! @param animation The animation that was started.
//! @param context The pointer to custom, application specific data, as set using
//! \ref animation_set_handlers()
//! @note This is called after any optional delay as set by \ref animation_set_delay() has expired.
//! @see animation_set_handlers
typedef void (*AnimationStartedHandler)(Animation *animation, void *context);

//! The function pointer type of the handler that will be called when the animation is stopped.
//! @param animation The animation that was stopped.
//! @param finished True if the animation was stopped because it was finished normally,
//! or False if the animation was stopped prematurely, because it was unscheduled before finishing.
//! @param context The pointer to custom, application specific data, as set using
//! \ref animation_set_handlers()
//! @see animation_set_handlers
//! \note
//! This animation (i.e.: the `animation` parameter) may be destroyed here.
//! It is not recommended to unschedule or destroy a **different** Animation within this
//! Animation's `stopped` handler.
typedef void (*AnimationStoppedHandler)(Animation *animation, bool finished, void *context);

//! The handlers that will get called when an animation starts and stops.
//! See documentation with the function pointer types for more information.
//! @see animation_set_handlers
typedef struct AnimationHandlers {
  //! The handler that will be called when an animation is started.
  AnimationStartedHandler started;
  //! The handler that will be called when an animation is stopped.
  AnimationStoppedHandler stopped;
} AnimationHandlers;

//! Sets the callbacks for the animation.
//! Often an application needs to run code at the start or at the end of an animation.
//! Using this function is possible to register callback functions with an animation,
//! that will get called at the start and end of the animation.
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation The animation for which to set up the callbacks.
//! @param callbacks The callbacks.
//! @param context A pointer to application specific data, that will be passed as an argument by
//! the animation subsystem when a callback is called.
//! @return true if successful, false on failure
bool animation_set_handlers(Animation *animation, AnimationHandlers callbacks, void *context);

//! Gets the application-specific callback context of the animation.
//! This `void` pointer is passed as an argument when the animation system calls AnimationHandlers
//! callbacks. The context pointer can be set to point to any application specific data using
//! \ref animation_set_handlers().
//! @param animation The animation.
//! @see animation_set_handlers
void *animation_get_context(Animation *animation);

//! Schedules the animation. Call this once after configuring an animation to get it to
//! start running.
//!
//! If the animation's implementation has a `.setup` callback it will get called before
//! this function returns.
//!
//! @note If the animation was already scheduled,
//! it will first unschedule it and then re-schedule it again.
//! Note that in that case, the animation's `.stopped` handler, the implementation's
//! `.teardown` and `.setup` will get called, due to the unscheduling and scheduling.
//! @param animation The animation to schedule.
//! @see \ref animation_unschedule()
//! @return true if successful, false on failure
bool animation_schedule(Animation *animation);

//! Unschedules the animation, which in effect stops the animation.
//! @param animation The animation to unschedule.
//! @note If the animation was not yet finished, unscheduling it will
//! cause its `.stopped` handler to get called, with the "finished" argument set to false.
//! @note If the animation is not scheduled or NULL, calling this routine is
//! effectively a no-op
//! @see \ref animation_schedule()
//! @return true if successful, false on failure
bool animation_unschedule(Animation *animation);

//! Unschedules all animations of the application.
//! @see animation_unschedule
void animation_unschedule_all(void);

//! @return True if the animation was scheduled, or false if it was not.
//! @note An animation will be scheduled when it is running and not finished yet.
//! An animation that has finished is automatically unscheduled.
//! For convenience, passing in a NULL animation argument will simply return false
//! @param animation The animation for which to get its scheduled state.
//! @see animation_schedule
//! @see animation_unschedule
bool animation_is_scheduled(Animation *animation);

//! Pointer to function that (optionally) prepares the animation for running.
//! This callback is called when the animation is added to the scheduler.
//! @param animation The animation that needs to be set up.
//! @see animation_schedule
//! @see AnimationTeardownImplementation
typedef void (*AnimationSetupImplementation)(Animation *animation);

//! Pointer to function that updates the animation according to the given normalized progress.
//! This callback will be called repeatedly by the animation scheduler whenever the animation needs
//! to be updated.
//! @param animation The animation that needs to update; gets passed in by the animation framework.
//! @param progress The current normalized progress; gets passed in by the animation
//! framework for each animation frame.
//! The value \ref ANIMATION_NORMALIZED_MIN represents the start and \ref ANIMATION_NORMALIZED_MAX
//! represents the end. Values outside this range (generated by a custom curve function) can be used
//! to implement features like a bounce back effect, where the progress exceeds the desired final
//! value before returning to complete the animation.
//! When using a system provided curve function, each frame during the animation will have a
//! progress value between \ref ANIMATION_NORMALIZED_MIN and \ref ANIMATION_NORMALIZED_MAX based on
//! the animation duration and the \ref AnimationCurve.
//! For example, say an animation was scheduled at t = 1.0s, has a delay of 1.0s, a duration of 2.0s
//! and a curve of AnimationCurveLinear. Then the .update callback will get called on t = 2.0s with
//! distance_normalized = \ref ANIMATION_NORMALIZED_MIN. For each frame thereafter until t = 4.0s,
//! the update callback will get called where distance_normalized is (\ref ANIMATION_NORMALIZED_MIN
//! + (((\ref ANIMATION_NORMALIZED_MAX - \ref ANIMATION_NORMALIZED_MIN) * t) / duration)).
//! Other system animation curve functions will result in a non-linear relation between
//! distance_normalized and time.
typedef void (*AnimationUpdateImplementation)(Animation *animation,
              const AnimationProgress progress);

//! Pointer to function that (optionally) cleans up the animation.
//! This callback is called when the animation is removed from the scheduler.
//! In case the `.setup` implementation
//! allocated any memory, this is a good place to release that memory again.
//! @param animation The animation that needs to be teared down.
//! @see animation_unschedule
//! @see AnimationSetupImplementation
typedef void (*AnimationTeardownImplementation)(Animation *animation);

//! The 3 callbacks that implement a custom animation.
//! Only the `.update` callback is mandatory, `.setup` and `.teardown` are optional.
//! See the documentation with the function pointer typedefs for more information.
//!
//! @note The `.setup` callback is called immediately after scheduling the animation,
//! regardless if there is a delay set for that animation using \ref animation_set_delay().
//!
//! The diagram below illustrates the order in which callbacks can be expected to get called
//! over the life cycle of an animation. It also illustrates where the implementation of
//! different animation callbacks are intended to be “living”.
//! ![](animations.png)
//!
//! @see AnimationSetupImplementation
//! @see AnimationUpdateImplementation
//! @see AnimationTeardownImplementation
typedef struct AnimationImplementation {
  //! Called by the animation system when an animation is scheduled, to prepare it for running.
  //! This callback is optional and can be left `NULL` when not needed.
  AnimationSetupImplementation setup;
  //! Called by the animation system when the animation needs to calculate the next animation frame.
  //! This callback is mandatory and should not be left `NULL`.
  AnimationUpdateImplementation update;
  //! Called by the animation system when an animation is unscheduled, to clean up after it has run.
  //! This callback is optional and can be left `NULL` when not needed.
  AnimationTeardownImplementation teardown;
} AnimationImplementation;

//! Sets the implementation of the custom animation.
//! When implementing custom animations, use this function to specify what functions need to be
//! called to for the setup, frame update and teardown of the animation.
//! @note Trying to set an attribute when an animation is immutable will return false (failure). An
//! animation is immutable once it has been added to a sequence or spawn animation or has been
//! scheduled.
//! @param animation The animation for which to set the implementation.
//! @param implementation The structure with function pointers to the implementation of the setup,
//!  update and teardown functions.
//! @see AnimationImplementation
//! @return true if successful, false on failure
bool animation_set_implementation(Animation *animation,
                                  const AnimationImplementation *implementation);

//! Gets the implementation of the custom animation.
//! @param animation The animation for which to get the implementation.
//! @see AnimationImplementation
//! @return NULL if animation implementation has not been setup.
const AnimationImplementation* animation_get_implementation(Animation *animation);

//! @addtogroup PropertyAnimation
//! \brief A ProperyAnimation animates the value of a "property" of a "subject" over time.
//!
//! <h3>Animating a Layer's frame property</h3>
//! Currently there is only one specific type of property animation offered off-the-shelf, namely
//! one to change the frame (property) of a layer (subject), see \ref
//! property_animation_create_layer_frame().
//!
//! <h3>Implementing a custom PropertyAnimation</h3>
//! It is fairly simple to create your own variant of a PropertyAnimation.
//!
//! Please refer to \htmlinclude UiFramework.html (chapter "Property Animations") for a conceptual
//! overview of the animation framework and make sure you understand the underlying \ref Animation,
//! in case you are not familiar with it, before trying to implement a variation on
//! PropertyAnimation.
//!
//! To implement a custom property animation, use \ref property_animation_create() and provide a
//! function pointers to the accessors (getter and setter) and setup, update and teardown callbacks
//! in the implementation argument. Note that the type of property to animate with \ref
//! PropertyAnimation is limited to int16_t, GPoint or GRect.
//!
//! For each of these types, there are implementations provided for the necessary `.update` handler
//! of the animation: see \ref property_animation_update_int16(), \ref
//! property_animation_update_gpoint() and \ref property_animation_update_grect().
//! These update functions expect the `.accessors` to conform to the following interface:
//! Any getter needs to have the following function signature: `__type__ getter(void *subject);`
//! Any setter needs to have to following function signature: `void setter(void *subject,
//! __type__ value);`
//! See \ref Int16Getter, \ref Int16Setter, \ref GPointGetter, \ref GPointSetter,
//! \ref GRectGetter, \ref GRectSetter for the typedefs that accompany the update fuctions.
//!
//! \code{.c}
//! static const PropertyAnimationImplementation my_implementation = {
//!   .base = {
//!     // using the "stock" update callback:
//!     .update = (AnimationUpdateImplementation) property_animation_update_gpoint,
//!   },
//!   .accessors = {
//!     // my accessors that get/set a GPoint from/onto my subject:
//!     .setter = { .gpoint = my_layer_set_corner_point, },
//!     .getter = { .gpoint = (const GPointGetter) my_layer_get_corner_point, },
//!   },
//! };
//! static PropertyAnimation* s_my_animation_ptr = NULL;
//! static GPoint s_to_point = GPointZero;
//! ...
//! // Use NULL as 'from' value, this will make the animation framework call the getter
//! // to get the current value of the property and use that as the 'from' value:
//! s_my_animation_ptr = property_animation_create(&my_implementation, my_layer, NULL, &s_to_point);
//! animation_schedule(property_animation_get_animation(s_my_animation_ptr));
//! \endcode
//! @{

struct PropertyAnimationAccessors;
typedef struct PropertyAnimationAccessors PropertyAnimationAccessors;

struct PropertyAnimationImplementation;
typedef struct PropertyAnimationImplementation PropertyAnimationImplementation;

struct PropertyAnimation;
typedef struct PropertyAnimation PropertyAnimation;

//! Convenience function to create and initialize a property animation that animates the frame of a
//! Layer. It sets up the PropertyAnimation to use \ref layer_set_frame() and \ref layer_get_frame()
//! as accessors and uses the `layer` parameter as the subject for the animation.
//! The same defaults are used as with \ref animation_create().
//! @param layer the layer that will be animated
//! @param from_frame the frame that the layer should animate from
//! @param to_frame the frame that the layer should animate to
//! @note Pass in `NULL` as one of the frame arguments to have it set automatically to the layer's
//! current frame. This will result in a call to \ref layer_get_frame() to get the current frame of
//! the layer.
//! @return A handle to the property animation. `NULL` if animation could not be created
PropertyAnimation *property_animation_create_layer_frame(struct Layer *layer, GRect *from_frame,
                                                         GRect *to_frame);

//! Convenience function to create and initialize a property animation that animates the bound's
//! origin of a Layer. It sets up the PropertyAnimation to use layer_set_bounds() and
//! layer_get_bounds() as accessors and uses the `layer` parameter as the subject for the animation.
//! The same defaults are used as with \ref animation_create().
//! @param layer the layer that will be animated
//! @param from_origin the origin that the bounds should animate from
//! @param to_origin the origin that the layer should animate to
//! @return A handle to the property animation. `NULL` if animation could not be created
PropertyAnimation *property_animation_create_bounds_origin(struct Layer *layer, GPoint *from,
    GPoint *to);

//! Creates a new PropertyAnimation on the heap and and initializes it with the specified values.
//! The same defaults are used as with \ref animation_create().
//! If the `from_value` or the `to_value` is `NULL`, the getter accessor will be called to get the
//! current value of the property and be used instead.
//! @param implementation Pointer to the implementation of the animation. In most cases, it makes
//! sense to pass in a `static const` struct pointer.
//! @param subject Pointer to the "subject" being animated. This will be passed in when the getter/
//! setter accessors are called,
//! see \ref PropertyAnimationAccessors, \ref GPointSetter, and friends. The value of this pointer
//! will be copied into the `.subject` field of the PropertyAnimation struct.
//! @param from_value Pointer to the value that the subject should animate from
//! @param to_value Pointer to the value that the subject should animate to
//! @note Pass in `NULL` as one of the value arguments to have it set automatically to the subject's
//! current property value, as returned by the getter function. Also note that passing in `NULL` for
//! both `from_value` and `to_value`, will result in the animation having the same from- and to-
//! values, effectively not doing anything.
//! @return A handle to the property animation. `NULL` if animation could not be created
PropertyAnimation* property_animation_create(const PropertyAnimationImplementation *implementation,
                      void *subject, void *from_value, void *to_value);

//! Destroy a property animation allocated by property_animation_create() or relatives.
//! @param property_animation the return value from property_animation_create
void property_animation_destroy(PropertyAnimation* property_animation);

//! Default update callback for a property animations to update a property of type int16_t.
//! Assign this function to the `.base.update` callback field of your
//! PropertyAnimationImplementation, in combination with a `.getter` and `.setter` accessors of
//! types \ref Int16Getter and \ref Int16Setter.
//! The implementation of this function will calculate the next value of the animation and call the
//! setter to set the new value upon the subject.
//! @param property_animation The property animation for which the update is requested.
//! @param distance_normalized The current normalized distance. See \ref
//! AnimationUpdateImplementation
//! @note This function is not supposed to be called "manually", but will be called automatically
//! when the animation is being run.
void property_animation_update_int16(PropertyAnimation *property_animation,
                                     const uint32_t distance_normalized);

//! Default update callback for a property animations to update a property of type uint32_t.
//! Assign this function to the `.base.update` callback field of your
//! PropertyAnimationImplementation, in combination with a `.getter` and `.setter` accessors of
//! types \ref UInt32Getter and \ref UInt32Setter.
//! The implementation of this function will calculate the next value of the animation and call the
//! setter to set the new value upon the subject.
//! @param property_animation The property animation for which the update is requested.
//! @param distance_normalized The current normalized distance. See \ref
//! AnimationUpdateImplementation
//! @note This function is not supposed to be called "manually", but will be called automatically
//! when the animation is being run.
void property_animation_update_uint32(PropertyAnimation *property_animation,
                                      const uint32_t distance_normalized);

//! Default update callback for a property animations to update a property of type GPoint.
//! Assign this function to the `.base.update` callback field of your
//! PropertyAnimationImplementation,
//! in combination with a `.getter` and `.setter` accessors of types \ref GPointGetter and \ref
//! GPointSetter.
//! The implementation of this function will calculate the next point of the animation and call the
//! setter to set the new point upon the subject.
//! @param property_animation The property animation for which the update is requested.
//! @param distance_normalized The current normalized distance. See \ref
//! AnimationUpdateImplementation
//! @note This function is not supposed to be called "manually", but will be called automatically
//! when the animation is being run.
void property_animation_update_gpoint(PropertyAnimation *property_animation,
                                      const uint32_t distance_normalized);

//! Default update callback for a property animations to update a property of type GRect.
//! Assign this function to the `.base.update` callback field of your
//! PropertyAnimationImplementation, in combination with a `.getter` and `.setter` accessors of
//! types \ref GRectGetter and \ref GRectSetter. The implementation of this function will calculate
//! the next rectangle of the animation and call the setter to set the new rectangle upon the
//! subject.
//! @param property_animation The property animation for which the update is requested.
//! @param distance_normalized The current normalized distance. See \ref
//! AnimationUpdateImplementation
//! @note This function is not supposed to be called "manually", but will be called automatically
//! when the animation is being run.
void property_animation_update_grect(PropertyAnimation *property_animation,
                                     const uint32_t distance_normalized);

//! Default update callback for a property animations to update a property of type GColor8.
//! Assign this function to the `.base.update` callback field of your
//! PropertyAnimationImplementation, in combination with a `.getter` and `.setter` accessors of
//! types \ref GColor8Getter and \ref GColor8Setter. The implementation of this function will
//! calculate the next rectangle of the animation and call the setter to set the new value upon
//! the subject.
//! @param property_animation The property animation for which the update is requested.
//! @param distance_normalized The current normalized distance. See \ref
//! AnimationUpdateImplementation
//! @note This function is not supposed to be called "manually", but will be called automatically
//! when the animation is being run.
void property_animation_update_gcolor8(PropertyAnimation *property_animation,
                                       const uint32_t distance_normalized);

//! Work-around for function pointer return type GPoint to avoid
//! tripping the pre-processor to use the equally named GPoint define
typedef GPoint GPointReturn;

//! Work-around for function pointer return type GRect to avoid
//! tripping the pre-processor to use the equally named GRect define
typedef GRect GRectReturn;

//! Function signature of a setter function to set a property of type int16_t onto the subject.
//! @see \ref property_animation_update_int16()
//! @see \ref PropertyAnimationAccessors
typedef void (*Int16Setter)(void *subject, int16_t int16);

//! Function signature of a getter function to get the current property of type int16_t of the
//! subject.
//! @see \ref property_animation_create()
//! @see \ref PropertyAnimationAccessors
typedef int16_t (*Int16Getter)(void *subject);

//! Function signature of a setter function to set a property of type uint32_t onto the subject.
//! @see \ref property_animation_update_int16()
//! @see \ref PropertyAnimationAccessors
typedef void (*UInt32Setter)(void *subject, uint32_t uint32);

//! Function signature of a getter function to get the current property of type uint32_t of the
//! subject.
//! @see \ref property_animation_create()
//! @see \ref PropertyAnimationAccessors
typedef uint32_t (*UInt32Getter)(void *subject);

//! Function signature of a setter function to set a property of type GPoint onto the subject.
//! @see \ref property_animation_update_gpoint()
//! @see \ref PropertyAnimationAccessors
typedef void (*GPointSetter)(void *subject, GPoint gpoint);

//! Function signature of a getter function to get the current property of type GPoint of the subject.
//! @see \ref property_animation_create()
//! @see \ref PropertyAnimationAccessors
typedef GPointReturn (*GPointGetter)(void *subject);

//! Function signature of a setter function to set a property of type GRect onto the subject.
//! @see \ref property_animation_update_grect()
//! @see \ref PropertyAnimationAccessors
typedef void (*GRectSetter)(void *subject, GRect grect);

//! Function signature of a getter function to get the current property of type GRect of the subject.
//! @see \ref property_animation_create()
//! @see \ref PropertyAnimationAccessors
typedef GRectReturn (*GRectGetter)(void *subject);

//! Function signature of a setter function to set a property of type GColor8 onto the subject.
//! @see \ref property_animation_update_gcolor8()
//! @see \ref PropertyAnimationAccessors
typedef void (*GColor8Setter)(void *subject, GColor8 gcolor);

//! Function signature of a getter function to get the current property of type GColor8 of the
//! subject.
//! @see \ref property_animation_create()
//! @see \ref PropertyAnimationAccessors
typedef GColor8 (*GColor8Getter)(void *subject);

//! Data structure containing the setter and getter function pointers that the property animation
//! should use.
//! The specified setter function will be used by the animation's update callback. <br/> Based on
//! the type of the property (int16_t, GPoint or GRect), the accompanying update callback should be
//! used, see \ref property_animation_update_int16(), \ref property_animation_update_gpoint() and
//! \ref property_animation_update_grect(). <br/>
//! The getter function is used when the animation is initialized, to assign the current value of
//! the subject's property as "from" or "to" value, see \ref property_animation_create().
typedef struct PropertyAnimationAccessors {
  //! Function pointer to the implementation of the function that __sets__ the updated property
  //! value. This function will be called repeatedly for each animation frame.
  //! @see PropertyAnimationAccessors
  union {
    //! Use if the property to animate is of int16_t type
    Int16Setter int16;
    //! Use if the property to animate is of GPoint type
    GPointSetter gpoint;
    //! Use if the property to animate is of GRect type
    GRectSetter grect;
    //! Use if the property to animate is of GColor8 type
    GColor8Setter gcolor8;
    //! Use if the property to animate is of uint32_t type
    UInt32Setter uint32;
  } setter;
  //! Function pointer to the implementation of the function that __gets__ the current property
  //! value. This function will be called during \ref property_animation_create(), to get the current
  //! property value, in case the `from_value` or `to_value` argument is `NULL`.
  //! @see PropertyAnimationAccessors
  union {
    //! Use if the property to animate is of int16_t type
    Int16Getter int16;
    //! Use if the property to animate is of GPoint type
    GPointGetter gpoint;
    //! Use if the property to animate is of GRect type
    GRectGetter grect;
    //! Use if the property to animate is of GColor8 type
    GColor8Getter gcolor8;
    //! Use if the property to animate is of uint32_t type
    UInt32Getter uint32;
  } getter;
} PropertyAnimationAccessors;

//! Data structure containing a collection of function pointers that form the implementation of the
//! property animation.
//! See the code example at the top (\ref PropertyAnimation).
typedef struct PropertyAnimationImplementation {
  //! The "inherited" fields from the Animation "base class".
  AnimationImplementation base;
  //! The accessors to set/get the property to be animated.
  PropertyAnimationAccessors accessors;
} PropertyAnimationImplementation;

//! Convenience function to retrieve an animation instance from a property animation instance
//! @param property_animation The property animation
//! @return The \ref Animation within this PropertyAnimation
Animation *property_animation_get_animation(PropertyAnimation *property_animation);

//! Convenience function to clone a property animation instance
//! @param property_animation The property animation
//! @return A clone of the original Animation
#define property_animation_clone(property_animation) \
    (PropertyAnimation *)animation_clone((Animation *)property_animation)

//! Convenience function to retrieve the 'from' GRect value from property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr The value will be retrieved into this pointer
//! @return true on success, false on failure
#define property_animation_get_from_grect(property_animation, value_ptr) \
    property_animation_from(property_animation, value_ptr, sizeof(GRect), false)

//! Convenience function to set the 'from' GRect value of property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer to the new value
//! @return true on success, false on failure
#define property_animation_set_from_grect(property_animation, value_ptr) \
    property_animation_from(property_animation, value_ptr, sizeof(GRect), true)

//! Convenience function to retrieve the 'from' GPoint value from property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr The value will be retrieved into this pointer
//! @return true on success, false on failure
#define property_animation_get_from_gpoint(property_animation, value_ptr) \
    property_animation_from(property_animation, value_ptr, sizeof(GPoint), false)

//! Convenience function to set the 'from' GPoint value of property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer to the new value
//! @return true on success, false on failure
#define property_animation_set_from_gpoint(property_animation, value_ptr) \
    property_animation_from(property_animation, value_ptr, sizeof(GPoint), true)

//! Convenience function to retrieve the 'from' int16_t value from property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr The value will be retrieved into this pointer
//! @return true on success, false on failure
#define property_animation_get_from_int16(property_animation, value_ptr) \
    property_animation_from(property_animation, value_ptr, sizeof(int16_t), false)

//! Convenience function to set the 'from' int16_t value of property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer to the new value
//! @return true on success, false on failure
#define property_animation_set_from_int16(property_animation, value_ptr) \
    property_animation_from(property_animation, value_ptr, sizeof(int16_t), true)

//! Convenience function to retrieve the 'to' GRect value from property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr The value will be retrieved into this pointer
//! @return true on success, false on failure
#define property_animation_get_to_grect(property_animation, value_ptr) \
    property_animation_to(property_animation, value_ptr, sizeof(GRect), false)

//! Convenience function to set the 'to' GRect value of property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer to the new value
//! @return true on success, false on failure
#define property_animation_set_to_grect(property_animation, value_ptr) \
    property_animation_to(property_animation, value_ptr, sizeof(GRect), true)

//! Convenience function to retrieve the 'to' GPoint value from property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr The value will be retrieved into this pointer
//! @return true on success, false on failure
#define property_animation_get_to_gpoint(property_animation, value_ptr) \
    property_animation_to(property_animation, value_ptr, sizeof(GPoint), false)

//! Convenience function to set the 'to' GPoint value of property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer to the new value
//! @return true on success, false on failure
#define property_animation_set_to_gpoint(property_animation, value_ptr) \
    property_animation_to(property_animation, value_ptr, sizeof(GPoint), true)

//! Convenience function to retrieve the 'to' int16_t value from property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr The value will be retrieved into this pointer
//! @return true on success, false on failure
#define property_animation_get_to_int16(property_animation, value_ptr) \
    property_animation_to(property_animation, value_ptr, sizeof(int16_t), false)

//! Convenience function to set the 'to' int16_t value of property animation handle
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer to the new value
//! @return true on success, false on failure
#define property_animation_set_to_int16(property_animation, value_ptr) \
    property_animation_to(property_animation, value_ptr, sizeof(int16_t), true)

//! Retrieve the subject of a property animation
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer used to store the subject of this property animation
//! @return The subject of this PropertyAnimation
#define property_animation_get_subject(property_animation, value_ptr) \
    property_animation_subject(property_animation, value_ptr, false)

//! Set the subject of a property animation
//! @param property_animation The PropertyAnimation to be accessed
//! @param value_ptr Pointer to the new subject value
#define property_animation_set_subject(property_animation, value_ptr) \
    property_animation_subject(property_animation, value_ptr, true)

//! Helper function used by the property_animation_get|set_subject macros
//! @param property_animation Handle to the property animation
//! @param subject The subject to get or set.
//! @param set true to set new subject, false to retrieve existing value
//! @return true if successful, false on failure (usually a bad animation_h)
bool property_animation_subject(PropertyAnimation *property_animation, void **subject, bool set);

//! Helper function used by the property_animation_get|set_from_.* macros
//! @param property_animation Handle to the property animation
//! @param from Pointer to the value
//! @param size Size of the from value
//! @param set true to set new value, false to retrieve existing one
//! @return true if successful, false on failure (usually a bad animation_h)
bool property_animation_from(PropertyAnimation *property_animation, void *from, size_t size,
                              bool set);

//! Helper function used by the property_animation_get|set_to_.* macros
//! @param property_animation handle to the property animation
//! @param to Pointer to the value
//! @param size Size of the to value
//! @param set true to set new value, false to retrieve existing one
//! @return true if successful, false on failure (usually a bad animation_h)
bool property_animation_to(PropertyAnimation *property_animation, void *to, size_t size,
                            bool set);

//! @} // group PropertyAnimation

//! @} // group Animation

//! @addtogroup UnobstructedArea
//! @{

//! Handler that will be called just before the unobstructed area will begin changing
//! @param final_unobstructed_screen_area The final unobstructed screen area after
//! the unobstructed area has finished changing.
//! @param context A user-provided context.
typedef void (*UnobstructedAreaWillChangeHandler)(GRect final_unobstructed_screen_area,
                                                  void *context);

//! Handler that will be called every time the unobstructed area changes
//! @param progress The progress of the animation changing the unobstructed area.
//! @param context A user-provided context.
typedef void (*UnobstructedAreaChangeHandler)(AnimationProgress progress, void *context);

//! Handler that will be called after the unobstructed area has finished changing
//! @param context A user-provided context.
typedef void (*UnobstructedAreaDidChangeHandler)(void *context);

typedef struct UnobstructedAreaHandlers {
  //! Handler that will be called just before the unobstructed area will begin changing.
  UnobstructedAreaWillChangeHandler will_change;
  //! Handler that will be called every time the unobstructed area changes.
  UnobstructedAreaChangeHandler change;
  //! Handler that will be called after the unobstructed area has finished changing.
  UnobstructedAreaDidChangeHandler did_change;
} UnobstructedAreaHandlers;

//! Subscribe to be notified when the app's unobstructed area changes. When an unobstructed area
//! begins changing, the `will_change` handler will be called, and every `will_change` call is
//! always paired with a `did_change` call that occurs when it is done changing given that
//! the `will_change` and `did_change` handlers are set. When subscribing while the unobstructed
//! area is changing, the `will_change` handler will be called after subscription in the next event
//! loop.
//! @param handlers The handlers that should be called when the unobstructed area changes.
//! @param context A user-provided context that will be passed to the callback handlers.
//! @see layer_get_unobstructed_bounds
#define unobstructed_area_service_subscribe(handlers, context) do {} while (0)

//! Unsubscribe from notifications about changes to the app's unobstructed area.
#define unobstructed_area_service_unsubscribe() do {} while (0)

//! @} // group UnobstructedArea

//! @addtogroup Layer
//! @{

//! @addtogroup TextLayer
//! \brief Layer that displays and formats a text string.
//!
//! ![](text_layer.png)
//! The geometric information (bounds, frame) of the Layer
//! is used as the "box" in which the text is drawn. The \ref TextLayer also has a number of
//! other properties that influence how the text is drawn. Most important of these properties are:
//! a pointer to the string to draw itself, the font, the text color, the background color of the
//! layer, the overflow mode and alignment of the text inside the layer.
//! @see Layer
//! @see TextDrawing
//! @see Fonts
//! @{

struct TextLayer;
typedef struct TextLayer TextLayer;

//! Creates a new TextLayer on the heap and initializes it with the default values.
//!
//! * Font: Raster Gothic 14-point Boldface (system font)
//! * Text Alignment: \ref GTextAlignmentLeft
//! * Text color: \ref GColorBlack
//! * Background color: \ref GColorWhite
//! * Clips: `true`
//! * Hidden: `false`
//! * Caching: `false`
//!
//! The text layer is automatically marked dirty after this operation.
//! @param frame The frame with which to initialze the TextLayer
//! @return A pointer to the TextLayer. `NULL` if the TextLayer could not
//! be created
TextLayer* text_layer_create(GRect frame);

//! Destroys a TextLayer previously created by text_layer_create.
void text_layer_destroy(TextLayer* text_layer);

//! Gets the "root" Layer of the text layer, which is the parent for the sub-
//! layers used for its implementation.
//! @param text_layer Pointer to the TextLayer for which to get the "root" Layer
//! @return The "root" Layer of the text layer.
Layer* text_layer_get_layer(TextLayer *text_layer);

//! Sets the pointer to the string where the TextLayer is supposed to find the text
//! at a later point in time, when it needs to draw itself.
//! @param text_layer The TextLayer of which to set the text
//! @param text The new text to set onto the TextLayer. This must be a null-terminated and valid UTF-8 string!
//! @note The string is not copied, so its buffer most likely cannot be stack allocated,
//! but is recommended to be a buffer that is long-lived, at least as long as the TextLayer
//! is part of a visible Layer hierarchy.
//! @see text_layer_get_text
void text_layer_set_text(TextLayer *text_layer, const char *text);

//! Gets the pointer to the string that the TextLayer is using.
//! @param text_layer The TextLayer for which to get the text
//! @see text_layer_set_text
const char* text_layer_get_text(TextLayer *text_layer);

//! Sets the background color of the bounding box that will be drawn behind the text
//! @param text_layer The TextLayer of which to set the background color
//! @param color The new \ref GColor to set the background to
//! @see text_layer_set_text_color
void text_layer_set_background_color(TextLayer *text_layer, GColor color);

//! Sets the color of text that will be drawn
//! @param text_layer The TextLayer of which to set the text color
//! @param color The new \ref GColor to set the text color to
//! @see text_layer_set_background_color
void text_layer_set_text_color(TextLayer *text_layer, GColor color);

//! Sets the line break mode of the TextLayer
//! @param text_layer The TextLayer of which to set the overflow mode
//! @param line_mode The new \ref GTextOverflowMode to set
void text_layer_set_overflow_mode(TextLayer *text_layer, GTextOverflowMode line_mode);

//! Sets the font of the TextLayer
//! @param text_layer The TextLayer of which to set the font
//! @param font The new \ref GFont for the TextLayer
//! @see fonts_get_system_font
//! @see fonts_load_custom_font
void text_layer_set_font(TextLayer *text_layer, GFont font);

//! Sets the alignment of the TextLayer
//! @param text_layer The TextLayer of which to set the alignment
//! @param text_alignment The new text alignment for the TextLayer
//! @see GTextAlignment
void text_layer_set_text_alignment(TextLayer *text_layer, GTextAlignment text_alignment);

//! Enables text flow following the boundaries of the screen and pagination that introduces
//! extra line spacing at page breaks to avoid partially clipped lines for the TextLayer.
//! If the TextLayer is part of a \ref ScrollLayer the ScrollLayer's frame will be used to
//! configure paging.
//! @note Make sure the TextLayer is part of the view hierarchy before calling this function.
//!   Otherwise it has no effect.
//! @param text_layer The TextLayer for which to enable text flow and paging
//! @param inset Additional amount of pixels to inset to the inside of the screen for text flow
//! @see text_layer_restore_default_text_flow_and_paging
//! @see graphics_text_attributes_enable_screen_text_flow
//! @see graphics_text_attributes_enable_paging
void text_layer_enable_screen_text_flow_and_paging(TextLayer *text_layer, uint8_t inset);

//! Restores text flow and paging for the TextLayer to the rectangular defaults.
//! @param text_layer The TextLayer for which to restore text flow and paging
//! @see text_layer_enable_screen_text_flow_and_paging
//! @see graphics_text_attributes_restore_default_text_flow
//! @see graphics_text_attributes_restore_default_paging
void text_layer_restore_default_text_flow_and_paging(TextLayer *text_layer);

//! Calculates the size occupied by the current text of the TextLayer
//! @param text_layer the TextLayer for which to calculate the text's size
//! @return The size occupied by the current text of the TextLayer
GSize text_layer_get_content_size(TextLayer *text_layer);

//! Update the size of the text layer
//! This is a convenience function to update the frame of the TextLayer.
//! @param text_layer The TextLayer of which to set the size
//! @param max_size The new size for the TextLayer
void text_layer_set_size(TextLayer *text_layer, const GSize max_size);

//! @} // group TextLayer

//! @addtogroup ScrollLayer
//! \brief Layer that scrolls its contents, animated.
//!
//! ![](scroll_layer.png)
//! <h3>Key Points</h3>
//! * Facilitates vertical scrolling of a layer sub-hierarchy zero or more
//! arbitrary layers. The example image shows a scroll layer containing one
//! large TextLayer.
//! * Shadows to indicate that there is more content are automatically drawn
//! on top of the content. When the end of the scroll layer is reached, the
//! shadow will automatically be retracted.
//! * Scrolling from one offset to another is animated implicitly by default.
//! * The scroll layer contains a "content" sub-layer, which is the layer that
//! is actually moved up an down. Any layer that is a child of this "content"
//! sub-layer, will be moved as well. Effectively, an entire layout of layers
//! can be scrolled this way. Use the convenience function
//! \ref scroll_layer_add_child() to add child layers to the "content" sub-layer.
//! * The scroll layer needs to be informed of the total size of the contents,
//! in order to calculate from and to what point it should be able to scroll.
//! Use \ref scroll_layer_set_content_size() to set the size of the contents.
//! * The button behavior is set up, using the convenience function
//! \ref scroll_layer_set_click_config_onto_window(). This will associate the
//! UP and DOWN buttons with scrolling up and down.
//! * The SELECT button can be configured by installing a click configuration
//! provider using \ref scroll_layer_set_callbacks().
//! * To scroll programatically to a certain offset, use
//! \ref scroll_layer_set_content_offset().
//! * It is possible to get called back for each scrolling increment, by
//! installing the `.content_offset_changed_handler` callback using
//! \ref scroll_layer_set_callbacks().
//! * Only vertical scrolling is supported at the moment.
//! @{

struct ScrollLayer;
typedef struct ScrollLayer ScrollLayer;

//! Function signature for the `.content_offset_changed_handler` callback.
typedef void (*ScrollLayerCallback)(struct ScrollLayer *scroll_layer, void *context);

//! All the callbacks that the ScrollLayer exposes for use by applications.
//! @note The context parameter can be set using scroll_layer_set_context() and
//! gets passed in as context with all of these callbacks.
typedef struct ScrollLayerCallbacks {

  //! Provider function to set up the SELECT button handlers. This will be
  //! called after the scroll layer has configured the click configurations for
  //! the up/down buttons, so it can also be used to modify the default up/down
  //! scrolling behavior.
  ClickConfigProvider click_config_provider;

  //! Called every time the the content offset changes. During a scrolling
  //! animation, it will be called for each intermediary offset as well
  ScrollLayerCallback content_offset_changed_handler;

} ScrollLayerCallbacks;

//! Creates a new ScrollLayer on the heap and initalizes it with the default values:
//! * Clips: `true`
//! * Hidden: `false`
//! * Content size: `frame.size`
//! * Content offset: \ref GPointZero
//! * Callbacks: None (`NULL` for each one)
//! * Callback context: `NULL`
//! @return A pointer to the ScrollLayer. `NULL` if the ScrollLayer could not
//! be created
ScrollLayer* scroll_layer_create(GRect frame);

//! Destroys a ScrollLayer previously created by scroll_layer_create.
void scroll_layer_destroy(ScrollLayer *scroll_layer);

//! Gets the "root" Layer of the scroll layer, which is the parent for the sub-
//! layers used for its implementation.
//! @param scroll_layer Pointer to the ScrollLayer for which to get the "root" Layer
//! @return The "root" Layer of the scroll layer.
Layer* scroll_layer_get_layer(const ScrollLayer *scroll_layer);

//! Adds the child layer to the content sub-layer of the ScrollLayer.
//! This will make the child layer part of the scrollable contents.
//! The content sub-layer of the ScrollLayer will become the parent of the
//! child layer.
//! @param scroll_layer The ScrollLayer to which to add the child layer.
//! @param child The Layer to add to the content sub-layer of the ScrollLayer.
//! @note You may need to update the size of the scrollable contents using
//! \ref scroll_layer_set_content_size().
void scroll_layer_add_child(ScrollLayer *scroll_layer, Layer *child);

//! Convenience function to set the \ref ClickConfigProvider callback on the
//! given window to scroll layer's internal click config provider. This internal
//! click configuration provider, will set up the default UP & DOWN
//! scrolling behavior.
//! This function calls \ref window_set_click_config_provider_with_context to
//! accomplish this.
//!
//! If you application has set a `.click_config_provider`
//! callback using \ref scroll_layer_set_callbacks(), this will be called
//! by the internal click config provider, after configuring the UP & DOWN
//! buttons. This allows your application to configure the SELECT button
//! behavior and optionally override the UP & DOWN
//! button behavior. The callback context for the SELECT click recognizer is
//! automatically set to the scroll layer's context (see
//! \ref scroll_layer_set_context() ). This context is passed into
//! \ref ClickHandler callbacks. For the UP and DOWN buttons, the scroll layer
//! itself is passed in by default as the callback context in order to deal with
//! those buttons presses to scroll up and down automatically.
//! @param scroll_layer The ScrollLayer that needs to receive click events.
//! @param window The window for which to set the click configuration.
//! @see \ref Clicks
//! @see window_set_click_config_provider_with_context
void scroll_layer_set_click_config_onto_window(ScrollLayer *scroll_layer, struct Window *window);

//! Sets the callbacks that the scroll layer exposes.
//! The `context` as set by \ref scroll_layer_set_context() is passed into each
//! of the callbacks. See \ref ScrollLayerCallbacks for the different callbacks.
//! @note If the `context` is NULL, a pointer to scroll_layer is used
//! as context parameter instead when calling callbacks.
//! @param scroll_layer The ScrollLayer for which to assign new callbacks.
//! @param callbacks The new callbacks.
void scroll_layer_set_callbacks(ScrollLayer *scroll_layer, ScrollLayerCallbacks callbacks);

//! Sets a new callback context. This context is passed into the scroll layer's
//! callbacks and also the \ref ClickHandler for the SELECT button.
//! If `NULL` or not set, the context defaults to a pointer to the ScrollLayer
//! itself.
//! @param scroll_layer The ScrollLayer for which to assign the new callback
//! context.
//! @param context The new callback context.
//! @see scroll_layer_set_click_config_onto_window
//! @see scroll_layer_set_callbacks
void scroll_layer_set_context(ScrollLayer *scroll_layer, void *context);

//! Scrolls to the given offset, optionally animated.
//! @note When scrolling down, the offset's `.y` decrements. When scrolling up,
//! the offset's `.y` increments. If scrolled completely to the top, the offset
//! is \ref GPointZero.
//! @note The `.x` field must be `0`. Horizontal scrolling is not supported.
//! @param scroll_layer The ScrollLayer for which to set the content offset
//! @param offset The final content offset
//! @param animated Pass in `true` to animate to the new content offset, or
//! `false` to set the new content offset without animating.
//! @see scroll_layer_get_content_offset
void scroll_layer_set_content_offset(ScrollLayer *scroll_layer, GPoint offset, bool animated);

//! Gets the point by which the contents are offset.
//! @param scroll_layer The ScrollLayer for which to get the content offset
//! @see scroll_layer_set_content_offset
GPoint scroll_layer_get_content_offset(ScrollLayer *scroll_layer);

//! Sets the size of the contents layer. This determines the area that is
//! scrollable. At the moment, this needs to be set "manually" and is not
//! derived from the geometry of the contents layers.
//! @param scroll_layer The ScrollLayer for which to set the content size.
//! @param size The new content size.
//! @see scroll_layer_get_content_size
void scroll_layer_set_content_size(ScrollLayer *scroll_layer, GSize size);

//! Gets the size of the contents layer.
//! @param scroll_layer The ScrollLayer for which to get the content size
//! @see scroll_layer_set_content_size
GSize scroll_layer_get_content_size(const ScrollLayer *scroll_layer);

//! Set the frame of the scroll layer and adjusts the internal layers' geometry
//! accordingly. The scroll layer is marked dirty automatically.
//! @param scroll_layer The ScrollLayer for which to set the frame
//! @param frame The new frame
void scroll_layer_set_frame(ScrollLayer *scroll_layer, GRect frame);

//! The click handlers for the UP button that the scroll layer will install as
//! part of \ref scroll_layer_set_click_config_onto_window().
//! @note This handler is exposed, in case one wants to implement an alternative
//! handler for the UP button, as a way to invoke the default behavior.
//! @param recognizer The click recognizer for which the handler is called
//! @param context A void pointer to the ScrollLayer that is the context of the click event
void scroll_layer_scroll_up_click_handler(ClickRecognizerRef recognizer, void *context);

//! The click handlers for the DOWN button that the scroll layer will install as
//! part of \ref scroll_layer_set_click_config_onto_window().
//! @note This handler is exposed, in case one wants to implement an alternative
//! handler for the DOWN button, as a way to invoke the default behavior.
//! @param recognizer The click recognizer for which the handler is called
//! @param context A void pointer to the ScrollLayer that is the context of the click event
void scroll_layer_scroll_down_click_handler(ClickRecognizerRef recognizer, void *context);

//! Sets the visibility of the scroll layer shadow.
//! If the visibility has changed, \ref layer_mark_dirty() will be called automatically
//! on the scroll layer.
//! @param scroll_layer The scroll layer for which to set the shadow visibility
//! @param hidden Supply `true` to make the shadow hidden, or `false` to make it
//! non-hidden.
void scroll_layer_set_shadow_hidden(ScrollLayer *scroll_layer, bool hidden);

//! Gets the visibility of the scroll layer shadow.
//! @param scroll_layer The scroll layer for which to get the visibility
//! @return True if the shadow is hidden, false if it is not hidden.
bool scroll_layer_get_shadow_hidden(const ScrollLayer *scroll_layer);

//! Enables or disables paging of the ScrollLayer (default: disabled). When enabled, every button
//! press will change the scroll offset by the frame's height.
//! @param scroll_layer The scroll layer for which to enable or disable paging
//! @param paging_enabled True, if paging should be enabled. False to enable.
void scroll_layer_set_paging(ScrollLayer *scroll_layer, bool paging_enabled);

//! Check whether or not the ScrollLayer uses paging when pressing buttons.
//! @param scroll_layer The scroll layer for which to get the paging behavior.
//! @return True, if paging is enabled; false otherwise.
bool scroll_layer_get_paging(ScrollLayer *scroll_layer);

struct ContentIndicator;
typedef struct ContentIndicator ContentIndicator;

//! Gets the ContentIndicator for a ScrollLayer.
//! @param scroll_layer The ScrollLayer for which to get the ContentIndicator
//! @return A pointer to the ContentIndicator, or `NULL` upon failure.
ContentIndicator *scroll_layer_get_content_indicator(ScrollLayer *scroll_layer);

//! Value to describe directions for \ref ContentIndicator.
//! @see \ref content_indicator_configure_direction
//! @see \ref content_indicator_set_content_available
typedef enum {
  ContentIndicatorDirectionUp = 0, //!< The up direction.
  ContentIndicatorDirectionDown, //!< The down direction.
  NumContentIndicatorDirections //!< The number of supported directions.
} ContentIndicatorDirection;

//! Struct used to configure directions for \ref ContentIndicator.
//! @see \ref content_indicator_configure_direction
typedef struct {
  Layer *layer; //!< The layer where the arrow indicator will be rendered when content is available.
  bool times_out; //!< Whether the display of the arrow indicator should timeout.
  GAlign alignment; //!< The alignment of the arrow within the provided layer.
  struct {
    GColor foreground; //!< The color of the arrow.
    GColor background; //!< The color of the layer behind the arrow.
  } colors;
} ContentIndicatorConfig;

//! Creates a ContentIndicator on the heap.
//! @return A pointer to the ContentIndicator. `NULL` if the ContentIndicator could not be created.
ContentIndicator *content_indicator_create(void);

//! Destroys a ContentIndicator previously created using \ref content_indicator_create().
//! @param content_indicator The ContentIndicator to destroy.
void content_indicator_destroy(ContentIndicator *content_indicator);

//! Configures a ContentIndicator for the given direction.
//! @param content_indicator The ContentIndicator to configure.
//! @param direction The direction for which to configure the ContentIndicator.
//! @param config The configuration to use to configure the ContentIndicator. If NULL, the data
//! for the specified direction will be reset.
//! @return True if the ContentIndicator was successfully configured for the given direction,
//! false otherwise.
bool content_indicator_configure_direction(ContentIndicator *content_indicator,
                                           ContentIndicatorDirection direction,
                                           const ContentIndicatorConfig *config);

//! Retrieves the availability status of content in the given direction.
//! @param content_indicator The ContentIndicator for which to get the content availability.
//! @param direction The direction for which to get the content availability.
//! @return True if content is available in the given direction, false otherwise.
bool content_indicator_get_content_available(ContentIndicator *content_indicator,
                                             ContentIndicatorDirection direction);

//! Sets the availability status of content in the given direction.
//! @param content_indicator The ContentIndicator for which to set the content availability.
//! @param direction The direction for which to set the content availability.
//! @param available Whether or not content is available.
//! @note If times_out is enabled, calling this function resets any previously scheduled timeout
//! timer for the ContentIndicator.
void content_indicator_set_content_available(ContentIndicator *content_indicator,
                                             ContentIndicatorDirection direction,
                                             bool available);

//! @} // group ScrollLayer

//! @addtogroup MenuLayer
//! \brief Layer that displays a standard list menu. Data is provided using
//! callbacks.
//!
//! ![](menu_layer.png)
//! <h3>Key Points</h3>
//! * The familiar list-style menu widget, as used throughout the Pebble user
//! interface.
//! * Built on top of \ref ScrollLayer, inheriting all its goodness like
//! animated scrolling, automatic "more content" shadow indicators, etc.
//! * All data needed to render the menu is requested on-demand via callbacks,
//! to avoid the need to keep a lot of data in memory.
//! * Support for "sections". A section is a group of items, visually separated
//! by a header with the name at the top of the section.
//! * Variable heights: each menu item cell and each section header can have
//! its own height. The heights are provided by callbacks.
//! * Deviation from the Layer system for cell drawing: Each menu item does
//! _not_ have its own Layer (to minimize memory usage). Instead, a
//! drawing callback is set onto the \ref MenuLayer that is responsible
//! for drawing each menu item. The \ref MenuLayer will call this callback for each
//! menu item that is visible and needs to be rendered.
//! * Cell and header drawing can be customized by implementing a custom drawing
//! callback.
//! * A few "canned" menu cell drawing functions are provided for convenience,
//! which support the default menu cell layout with a title, optional subtitle
//! and icon.
//!
//! For short, static list menus, consider using \ref SimpleMenuLayer.
//! @{

//! Section drawing function to draw a basic section cell with the title, subtitle, and icon of the section. 
//! Call this function inside the `.draw_row` callback implementation, see \ref MenuLayerCallbacks.
//! Note that if the size of `cell_layer` is too small to fit all of the cell items specified, not
//! all of them may be drawn.
//! @param ctx The destination graphics context
//! @param cell_layer The layer of the cell to draw
//! @param title If non-null, draws a title in larger text (24 points, bold
//! Raster Gothic system font).
//! @param subtitle If non-null, draws a subtitle in smaller text (18 points,
//! Raster Gothic system font). If `NULL`, the title will be centered vertically
//! inside the menu cell.
//! @param icon If non-null, draws an icon to the left of the text. If `NULL`,
//! the icon will be omitted and the leftover space is used for the title and
//! subtitle.
void menu_cell_basic_draw(GContext* ctx,
                          const Layer *cell_layer,
                          const char *title,
                          const char *subtitle,
                          GBitmap *icon);

//! Cell drawing function to draw a basic menu cell layout with title, subtitle
//! Cell drawing function to draw a menu cell layout with only one big title.
//! Call this function inside the `.draw_row` callback implementation, see
//! \ref MenuLayerCallbacks.
//! @param ctx The destination graphics context
//! @param cell_layer The layer of the cell to draw
//! @param title If non-null, draws a title in larger text (28 points, bold
//! Raster Gothic system font).
void menu_cell_title_draw(GContext* ctx,
                          const Layer *cell_layer,
                          const char *title);

//! Section header drawing function to draw a basic section header cell layout
//! with the title of the section.
//! Call this function inside the `.draw_header` callback implementation, see
//! \ref MenuLayerCallbacks.
//! @param ctx The destination graphics context
//! @param cell_layer The layer of the cell to draw
//! @param title If non-null, draws the title in small text (14 points, bold
//! Raster Gothic system font).
void menu_cell_basic_header_draw(GContext* ctx,
                                 const Layer *cell_layer,
                                 const char *title);

//! Default section header height in pixels
#define MENU_CELL_BASIC_HEADER_HEIGHT ((const int16_t) 16)


#define MENU_INDEX_NOT_FOUND ((const uint16_t) ~0)

//! Data structure to represent an menu item's position in a menu, by specifying
//! the section index and the row index within that section.
typedef struct MenuIndex {
  //! The index of the section
  uint16_t section;
  //! The index of the row within the section with index `.section`
  uint16_t row;
} MenuIndex;

//! Macro to create a MenuIndex
#define MenuIndex(section, row) ((MenuIndex){ (section), (row) })

//! Comparator function to determine the order of two MenuIndex values.
//! @param a Pointer to the menu index of the first item
//! @param b Pointer to the menu index of the second item
//! @return 0 if A and B are equal, 1 if A has a higher section & row
//! combination than B or else -1
int16_t menu_index_compare(const MenuIndex *a, const MenuIndex *b);

typedef struct MenuCellSpan {
  int16_t y;
  int16_t h;
  int16_t sep;
  MenuIndex index;
} MenuCellSpan;

struct MenuLayer;
typedef struct MenuLayer MenuLayer;

//! Function signature for the callback to get the number of sections in a menu.
//! @param menu_layer The \ref MenuLayer for which the data is requested
//! @param callback_context The callback context
//! @return The number of sections in the menu
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef uint16_t (*MenuLayerGetNumberOfSectionsCallback)(struct MenuLayer *menu_layer,
                                                         void *callback_context);

//! Function signature for the callback to get the number of rows in a
//! given section in a menu.
//! @param menu_layer The \ref MenuLayer for which the data is requested
//! @param section_index The index of the section of the menu for which the
//! number of items it contains is requested
//! @param callback_context The callback context
//! @return The number of rows in the given section in the menu
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef uint16_t (*MenuLayerGetNumberOfRowsInSectionsCallback)(struct MenuLayer *menu_layer,
                                                               uint16_t section_index,
                                                               void *callback_context);

//! Function signature for the callback to get the height of the menu cell
//! at a given index.
//! @param menu_layer The \ref MenuLayer for which the data is requested
//! @param cell_index The MenuIndex for which the cell height is requested
//! @param callback_context The callback context
//! @return The height of the cell at the given MenuIndex
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef int16_t (*MenuLayerGetCellHeightCallback)(struct MenuLayer *menu_layer,
                                                  MenuIndex *cell_index,
                                                  void *callback_context);

//! Function signature for the callback to get the height of the section header
//! at a given section index.
//! @param menu_layer The \ref MenuLayer for which the data is requested
//! @param section_index The index of the section for which the header height is
//! requested
//! @param callback_context The callback context
//! @return The height of the section header at the given section index
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef int16_t (*MenuLayerGetHeaderHeightCallback)(struct MenuLayer *menu_layer,
                                                    uint16_t section_index,
                                                    void *callback_context);

//! Function signature for the callback to get the height of the separator
//! at a given index.
//! @param menu_layer The \ref MenuLayer for which the data is requested
//! @param cell_index The MenuIndex for which the cell height is requested
//! @param callback_context The callback context
//! @return The height of the separator at the given MenuIndex
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef int16_t (*MenuLayerGetSeparatorHeightCallback)(struct MenuLayer *menu_layer,
                                                       MenuIndex *cell_index,
                                                       void *callback_context);

//! Function signature for the callback to render the menu cell at a given
//! MenuIndex.
//! @param ctx The destination graphics context to draw into
//! @param cell_layer The cell's layer, containing the geometry of the cell
//! @param cell_index The MenuIndex of the cell that needs to be drawn
//! @param callback_context The callback context
//! @note The `cell_layer` argument is provided to make it easy to re-use an
//! `.update_proc` implementation in this callback. Only the bounds and frame
//! of the `cell_layer` are actually valid and other properties should be
//! ignored.
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef void (*MenuLayerDrawRowCallback)(GContext* ctx,
                                         const Layer *cell_layer,
                                         MenuIndex *cell_index,
                                         void *callback_context);

//! Function signature for the callback to render the section header at a given
//! section index.
//! @param ctx The destination graphics context to draw into
//! @param cell_layer The header cell's layer, containing the geometry of the
//! header cell
//! @param section_index The section index of the section header that needs to
//! be drawn
//! @param callback_context The callback context
//! @note The `cell_layer` argument is provided to make it easy to re-use an
//! `.update_proc` implementation in this callback. Only the bounds and frame
//! of the `cell_layer` are actually valid and other properties should be
//! ignored.
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef void (*MenuLayerDrawHeaderCallback)(GContext* ctx,
                                            const Layer *cell_layer,
                                            uint16_t section_index,
                                            void *callback_context);

//! Function signature for the callback to render the separator at a given
//! MenuIndex.
//! @param ctx The destination graphics context to draw into
//! @param cell_layer The cell's layer, containing the geometry of the cell
//! @param cell_index The MenuIndex of the separator that needs to be drawn
//! @param callback_context The callback context
//! @note The `cell_layer` argument is provided to make it easy to re-use an
//! `.update_proc` implementation in this callback. Only the bounds and frame
//! of the `cell_layer` are actually valid and other properties should be
//! ignored.
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef void (*MenuLayerDrawSeparatorCallback)(GContext* ctx,
                                               const Layer *cell_layer,
                                               MenuIndex *cell_index,
                                               void *callback_context);

//! Function signature for the callback to handle the event that a user hits
//! the SELECT button.
//! @param menu_layer The \ref MenuLayer for which the selection event occured
//! @param cell_index The MenuIndex of the cell that is selected
//! @param callback_context The callback context
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef void (*MenuLayerSelectCallback)(struct MenuLayer *menu_layer,
                                        MenuIndex *cell_index,
                                        void *callback_context);

//! Function signature for the callback to handle a change in the current
//! selected item in the menu.
//! @param menu_layer The \ref MenuLayer for which the selection event occured
//! @param new_index The MenuIndex of the new item that is selected now
//! @param old_index The MenuIndex of the old item that was selected before
//! @param callback_context The callback context
//! @see \ref menu_layer_set_callbacks()
//! @see \ref MenuLayerCallbacks
typedef void (*MenuLayerSelectionChangedCallback)(struct MenuLayer *menu_layer,
                                                  MenuIndex new_index,
                                                  MenuIndex old_index,
                                                  void *callback_context);

//! Function signature for the callback which allows or changes selection behavior of the menu.
//! In order to change the cell that should be selected, modify the passed in new_index.
//! Preventing the selection from changing, new_index can be assigned the value of old_index.
//! @param menu_layer The \ref MenuLayer for which the selection event that occured
//! @param new_index Pointer to the index that the MenuLayer is going to change selection to.
//! @param old_index The index that is being unselected.
//! @param callback_context The callback context
//! @note \ref menu_layer_set_selected_index will not trigger this callback when
//! the selection changes, but \ref menu_layer_set_selected_next will.
typedef void (*MenuLayerSelectionWillChangeCallback)(struct MenuLayer *menu_layer,
                                                     MenuIndex *new_index,
                                                     MenuIndex old_index,
                                                     void *callback_context);

//! Function signature for the callback which draws the menu's background.
//! The background is underneath the cells of the menu, and is visible in the
//! padding below the bottom cell, or if a cell's background color is set to \ref GColorClear.
//! @param ctx The destination graphics context to draw into.
//! @param bg_layer The background's layer, containing the geometry of the background.
//! @param highlight Whether this should be rendered as highlighted or not. Highlight style
//! should match the highlight style of cells, since this color can be used for animating selection.
typedef void (*MenuLayerDrawBackgroundCallback)(GContext* ctx,
                                                const Layer *bg_layer,
                                                bool highlight,
                                                void *callback_context);

//! Data structure containing all the callbacks of a \ref MenuLayer.
typedef struct MenuLayerCallbacks {
  //! Callback that gets called to get the number of sections in the menu.
  //! This can get called at various moments throughout the life of a menu.
  //! @note When `NULL`, the number of sections defaults to 1.
  MenuLayerGetNumberOfSectionsCallback get_num_sections;

  //! Callback that gets called to get the number of rows in a section. This
  //! can get called at various moments throughout the life of a menu.
  //! @note Must be set to a valid callback; `NULL` causes undefined behavior.
  MenuLayerGetNumberOfRowsInSectionsCallback get_num_rows;

  //! Callback that gets called to get the height of a cell.
  //! This can get called at various moments throughout the life of a menu.
  //! @note When `NULL`, the default height of 44 pixels is used.
  MenuLayerGetCellHeightCallback get_cell_height;

  //! Callback that gets called to get the height of a section header.
  //! This can get called at various moments throughout the life of a menu.
  //! @note When `NULL`, the default height of 0 pixels is used. This disables
  //! section headers.
  MenuLayerGetHeaderHeightCallback get_header_height;

  //! Callback that gets called to render a menu item.
  //! This gets called for each menu item, every time it needs to be
  //! re-rendered.
  //! @note Must be set to a valid callback; `NULL` causes undefined behavior.
  MenuLayerDrawRowCallback draw_row;

  //! Callback that gets called to render a section header.
  //! This gets called for each section header, every time it needs to be
  //! re-rendered.
  //! @note Must be set to a valid callback, unless `.get_header_height` is
  //! `NULL`. Causes undefined behavior otherwise.
  MenuLayerDrawHeaderCallback draw_header;

  //! Callback that gets called when the user triggers a click with the SELECT
  //! button.
  //! @note When `NULL`, click events for the SELECT button are ignored.
  MenuLayerSelectCallback select_click;

  //! Callback that gets called when the user triggers a long click with the
  //! SELECT button.
  //! @note When `NULL`, long click events for the SELECT button are ignored.
  MenuLayerSelectCallback select_long_click;

  //! Callback that gets called whenever the selection changes.
  //! @note When `NULL`, selection change events are ignored.
  MenuLayerSelectionChangedCallback selection_changed;

  //! Callback that gets called to get the height of a separator
  //! This can get called at various moments throughout the life of a menu.
  //! @note When `NULL`, the default height of 0 is used.
  MenuLayerGetSeparatorHeightCallback get_separator_height;

  //! Callback that gets called to render a separator.
  //! This gets called for each separator, every time it needs to be
  //! re-rendered.
  //! @note Must be set to a valid callback, unless `.get_separator_height` is
  //! `NULL`. Causes undefined behavior otherwise.
  MenuLayerDrawSeparatorCallback draw_separator;

  //! Callback that gets called before the selected cell changes.
  //! This gets called before the selected item in the MenuLayer is changed,
  //! and will allow for the selected cell to be overridden.
  //! This allows for skipping cells in the menu, locking selection onto a given item,
  MenuLayerSelectionWillChangeCallback selection_will_change;

  //! Callback that gets called before any cells are drawn.
  //! This supports two states, either highlighted or not highlighted.
  //! If highlighted is specified, it is expected to be colored in the same
  //! style as the menu's cells are.
  //! If this callback is not specified, it will default to the colors set with
  //! \ref menu_layer_set_normal_colors and \ref menu_layer_set_highlight_colors.
  MenuLayerDrawBackgroundCallback draw_background;
} MenuLayerCallbacks;

//! Creates a new \ref MenuLayer on the heap and initalizes it with the default values.
//!
//! * Clips: `true`
//! * Hidden: `false`
//! * Content size: `frame.size`
//! * Content offset: \ref GPointZero
//! * Callbacks: None (`NULL` for each one)
//! * Callback context: `NULL`
//! * After the relevant callbacks are called to populate the menu, the item at MenuIndex(0, 0)
//!   will be selected initially.
//! @return A pointer to the \ref MenuLayer. `NULL` if the \ref MenuLayer could not
//! be created
MenuLayer* menu_layer_create(GRect frame);

//! Destroys a \ref MenuLayer previously created by menu_layer_create.
void menu_layer_destroy(MenuLayer* menu_layer);

//! Gets the "root" Layer of the \ref MenuLayer, which is the parent for the sub-
//! layers used for its implementation.
//! @param menu_layer Pointer to the MenuLayer for which to get the "root" Layer
//! @return The "root" Layer of the \ref MenuLayer.
Layer* menu_layer_get_layer(const MenuLayer *menu_layer);

//! Gets the ScrollLayer of the \ref MenuLayer, which is the layer responsible for
//! the scrolling of the \ref MenuLayer.
//! @param menu_layer Pointer to the \ref MenuLayer for which to get the ScrollLayer
//! @return The ScrollLayer of the \ref MenuLayer.
ScrollLayer* menu_layer_get_scroll_layer(const MenuLayer *menu_layer);

//! Sets the callbacks for the MenuLayer.
//! @param menu_layer Pointer to the \ref MenuLayer for which to set the callbacks
//! and callback context.
//! @param callback_context The new callback context. This is passed into each
//! of the callbacks and can be set to point to application provided data.
//! @param callbacks The new callbacks for the \ref MenuLayer. The storage for this
//! data structure must be long lived. Therefore, it cannot be stack-allocated.
//! @see MenuLayerCallbacks
void menu_layer_set_callbacks(MenuLayer *menu_layer, void *callback_context,
                                       MenuLayerCallbacks callbacks);

//! Convenience function to set the \ref ClickConfigProvider callback on the
//! given window to the \ref MenuLayer internal click config provider. This internal
//! click configuration provider, will set up the default UP & DOWN
//! scrolling / menu item selection behavior.
//! This function calls \ref scroll_layer_set_click_config_onto_window to
//! accomplish this.
//!
//! Click and long click events for the SELECT button can be handled by
//! installing the appropriate callbacks using \ref menu_layer_set_callbacks().
//! This is a deviation from the usual click configuration provider pattern.
//! @param menu_layer The \ref MenuLayer that needs to receive click events.
//! @param window The window for which to set the click configuration.
//! @see \ref Clicks
//! @see \ref window_set_click_config_provider_with_context()
//! @see \ref scroll_layer_set_click_config_onto_window()
void menu_layer_set_click_config_onto_window(MenuLayer *menu_layer,
                                             struct Window *window);

//! Values to specify how a (selected) row should be aligned relative to the
//! visible area of the \ref MenuLayer.
typedef enum {
  //! Don't align or update the scroll offset of the \ref MenuLayer.
  MenuRowAlignNone,

  //! Scroll the contents of the \ref MenuLayer in such way that the selected row
  //! is centered relative to the visible area.
  MenuRowAlignCenter,

  //! Scroll the contents of the \ref MenuLayer in such way that the selected row
  //! is at the top of the visible area.
  MenuRowAlignTop,

  //! Scroll the contents of the \ref MenuLayer in such way that the selected row
  //! is at the bottom of the visible area.
  MenuRowAlignBottom,
} MenuRowAlign;

//! Selects the next or previous item, relative to the current selection.
//! @param menu_layer The \ref MenuLayer for which to select the next item
//! @param up Supply `false` to select the next item in the list (downwards),
//! or `true` to select the previous item in the list (upwards).
//! @param scroll_align The alignment of the new selection
//! @param animated Supply `true` to animate changing the selection, or `false`
//! to change the selection instantly.
//! @note If there is no next/previous item, this function is a no-op.
void menu_layer_set_selected_next(MenuLayer *menu_layer,
                                  bool up,
                                  MenuRowAlign scroll_align,
                                  bool animated);

//! Selects the item with given \ref MenuIndex.
//! @param menu_layer The \ref MenuLayer for which to change the selection
//! @param index The index of the item to select
//! @param scroll_align The alignment of the new selection
//! @param animated Supply `true` to animate changing the selection, or `false`
//! to change the selection instantly.
//! @note If the section and/or row index exceeds the avaible number of sections
//! or resp. rows, the exceeding index/indices will be capped, effectively
//! selecting the last section and/or row, resp.
void menu_layer_set_selected_index(MenuLayer *menu_layer,
                                   MenuIndex index, MenuRowAlign scroll_align,
                                   bool animated);

//! Gets the MenuIndex of the currently selected menu item.
//! @param menu_layer The \ref MenuLayer for which to get the current selected index.
//! @see menu_cell_layer_is_highlighted
//! @note This function should not be used to determine whether a cell should be
//! highlighted or not. See \ref menu_cell_layer_is_highlighted for more
//! information.
MenuIndex menu_layer_get_selected_index(const MenuLayer *menu_layer);

//! Reloads the data of the menu. This causes the menu to re-request the menu
//! item data, by calling the relevant callbacks.
//! The current selection and scroll position will not be changed. See the
//! note with \ref menu_layer_set_selected_index() for the behavior if the
//! old selection is no longer valid.
//! @param menu_layer The \ref MenuLayer for which to reload the data.
void menu_layer_reload_data(MenuLayer *menu_layer);

//! Returns whether or not the given cell layer is highlighted.
//! Using this for determining highlight behaviour is preferable to using
//! \ref menu_layer_get_selected_index. Row drawing callbacks may be invoked multiple
//! times with a different highlight status on the same cell in order to handle partially
//! highlighted cells during animation.
//! @param cell_layer The \ref Layer for the cell to check highlight status.
//! @return true if the given cell layer is highlighted in the menu.
bool menu_cell_layer_is_highlighted(const Layer *cell_layer);

//! Set the default colors to be used for cells when it is in a normal state (not highlighted).
//! The GContext's text and fill colors will be set appropriately prior to calling the `.draw_row`
//! callback.
//! If this function is not explicitly called on a \ref MenuLayer, it will default to white
//! background with black foreground.
//! @param menu_layer The \ref MenuLayer for which to set the colors.
//! @param background The color to be used for the background of the cell.
//! @param foreground The color to be used for the foreground and text of the cell.
//! @see \ref menu_layer_set_highlight_colors
void menu_layer_set_normal_colors(MenuLayer *menu_layer, GColor background, GColor foreground);

//! Set the default colors to be used for cells when it is in a highlighted state.
//! The GContext's text and fill colors will be set appropriately prior to calling the `.draw_row`
//! callback.
//! If this function is not explicitly called on a \ref MenuLayer, it will default to black
//! background with white foreground.
//! @param menu_layer The \ref MenuLayer for which to set the colors.
//! @param background The color to be used for the background of the cell.
//! @param foreground The color to be used for the foreground and text of the cell.
//! @see \ref menu_layer_set_normal_colors
void menu_layer_set_highlight_colors(MenuLayer *menu_layer, GColor background, GColor foreground);

//! This enables or disables padding at the bottom of the \ref MenuLayer.
//! Padding at the bottom of the layer keeps the bottom item from being at the very bottom of the
//! screen.
//! Padding is turned on by default for all MenuLayers.
//! The color of the padded area will be the background color set using
//! \ref menu_layer_set_normal_colors(). To color the padding a different color, use
//! \ref MenuLayerDrawBackgroundCallback.
//! @param menu_layer The menu layer for which to enable or disable the padding.
//! @param enable True = enable padding, False = disable padding.
void menu_layer_pad_bottom_enable(MenuLayer *menu_layer, bool enable);

//! True, if the \ref MenuLayer generally scrolls such that the selected row is in the center.
//! @see \ref menu_layer_set_center_focused
bool menu_layer_get_center_focused(MenuLayer *menu_layer);

//! Controls if the \ref MenuLayer generally scrolls such that the selected row is in the center.
//! For platforms with a round display (PBL_ROUND) the default is true,
//! otherwise false is the default
//! @param menu_layer The menu layer for which to enable or disable the behavior.
//! @param center_focused true = enable the mode, false = disable it.
//! @see \ref menu_layer_get_center_focused
void menu_layer_set_center_focused(MenuLayer *menu_layer, bool center_focused);

//! Returns whether or not the specified cell index is currently selected.
//! @param menu_layer The \ref MenuLayer to use when determining if the index is selected.
//! @param index The \ref MenuIndex of the cell to check for selection.
//! @note This function should not be used to determine whether a cell is highlighted or not.
//! See \ref menu_cell_layer_is_highlighted for more information.
bool menu_layer_is_index_selected(const MenuLayer *menu_layer, MenuIndex *index);

#define MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT ((const int16_t) 68)

#define MENU_CELL_ROUND_FOCUSED_TALL_CELL_HEIGHT ((const int16_t) 84)

#define MENU_CELL_ROUND_UNFOCUSED_SHORT_CELL_HEIGHT ((const int16_t) 24)

#define MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT ((const int16_t) 32)

//! @} // group MenuLayer

//! @addtogroup SimpleMenuLayer
//! \brief Wrapper around \ref MenuLayer, that uses static data to display a
//! list menu.
//!
//! ![](simple_menu_layer.png)
//! @{

struct SimpleMenuLayer;
typedef struct SimpleMenuLayer SimpleMenuLayer;

//! Function signature for the callback to handle the event that a user hits
//! the SELECT button.
//! @param index The row index of the item
//! @param context The callback context
typedef void (*SimpleMenuLayerSelectCallback)(int index, void *context);

//! Data structure containing the information of a menu item.
typedef struct {
  //! The title of the menu item. Required.
  const char *title;
  //! The subtitle of the menu item. Optional, leave `NULL` if unused.
  const char *subtitle;
  //! The icon of the menu item. Optional, leave `NULL` if unused.
  GBitmap *icon;
  //! The callback that needs to be called upon a click on the SELECT button.
  //! Optional, leave `NULL` if unused.
  SimpleMenuLayerSelectCallback callback;
} SimpleMenuItem;

//! Data structure containing the information of a menu section.
typedef struct {
  //! Title of the section. Optional, leave `NULL` if unused.
  const char *title;
  //! Array of items in the section.
  const SimpleMenuItem *items;
  //! Number of items in the `.items` array.
  uint32_t num_items;
} SimpleMenuSection;

//! Creates a new SimpleMenuLayer on the heap and initializes it.
//! It also sets the internal click configuration provider onto given window.
//! @param frame The frame at which to initialize the menu
//! @param window The window onto which to set the click configuration provider
//! @param sections Array with sections that need to be displayed in the menu
//! @param num_sections The number of sections in the `sections` array.
//! @param callback_context Pointer to application specific data, that is passed
//! into the callbacks.
//! @note The `sections` array is not deep-copied and can therefore not be stack
//! allocated, but needs to be backed by long-lived storage.
//! @note This function does not add the menu's layer to the window.
//! @return A pointer to the SimpleMenuLayer. `NULL` if the SimpleMenuLayer could not
//! be created
SimpleMenuLayer* simple_menu_layer_create(GRect frame, Window *window,
    const SimpleMenuSection *sections, int32_t num_sections, void *callback_context);

//! Destroys a SimpleMenuLayer previously created by simple_menu_layer_create.
void simple_menu_layer_destroy(SimpleMenuLayer* menu_layer);

//! Gets the "root" Layer of the simple menu layer, which is the parent for the
//! sub-layers used for its implementation.
//! @param simple_menu Pointer to the SimpleMenuLayer for which to get the
//! "root" Layer
//! @return The "root" Layer of the menu layer.
Layer* simple_menu_layer_get_layer(const SimpleMenuLayer *simple_menu);

//! Gets the row index of the currently selection menu item.
//! @param simple_menu The SimpleMenuLayer for which to get the current
//! selected row index.
int simple_menu_layer_get_selected_index(const SimpleMenuLayer *simple_menu);

//! Selects the item in the first section at given row index.
//! @param simple_menu The SimpleMenuLayer for which to change the selection
//! @param index The row index of the item to select
//! @param animated Supply `true` to animate changing the selection, or `false`
//! to change the selection instantly.
void simple_menu_layer_set_selected_index(SimpleMenuLayer *simple_menu, int32_t index, bool animated);

//! @param simple_menu The \ref SimpleMenuLayer to get the \ref MenuLayer from.
//! @return The \ref MenuLayer.
MenuLayer *simple_menu_layer_get_menu_layer(SimpleMenuLayer *simple_menu);

//! @} // group SimpleMenuLayer

//! @addtogroup ActionBarLayer
//! \brief Vertical, bar-shaped control widget on the right edge of the window
//!
//! ![](action_bar_layer.png)
//! ActionBarLayer is a Layer that displays a bar on the right edge of the
//! window. The bar can contain up to 3 icons, each corresponding with one of
//! the buttons on the right side of the watch. The purpose of each icon is
//! to provide a hint (feed-forward) to what action a click on the respective
//! button will cause.
//!
//! The action bar is useful when there are a few (up to 3) main actions that
//! are desirable to be able to take quickly, literally with one press of a
//! button.
//!
//! <h3>More actions</h3>
//! If there are more than 3 actions the user might want to take:
//! * Try assigning the top and bottom icons of the action bar to the two most
//! immediate actions and use the middle icon to push a Window with a MenuLayer
//! with less immediate actions.
//! * Secondary actions that are not vital, can be "hidden" under a long click.
//! Try to group similar actions to one button. For example, in a Music app,
//! a single click on the top button is tied to the action to jump to the
//! previous track. Holding that same button means seek backwards.
//!
//! <h3>Directionality mapping</h3>
//! When the top and bottom buttons are used to control navigating through
//! a (potentially virtual, non-visible) list of items, follow this guideline:
//! * Tie the top button to the action that goes to the _previous_ item in the
//! list, for example "jump to previous track" in a Music app.
//! * Tie the bottom button to the action that goes to the _next_ item in the
//! list, for example "jump to next track" in a Music app.
//!
//! <h3>Geometry</h3>
//! * The action bar is 30 pixels wide. Use the \ref ACTION_BAR_WIDTH define.
//! * Icons should not be wider than 28 pixels, or taller than 18 pixels.
//! It is recommended to use a size of around 15 x 15 pixels for the "visual core" of the icon,
//! and extending or contracting where needed.
//! <h3>Example Code</h3>
//! The code example below shows how to do the initial setup of the action bar
//! in a window's `.load` handler.
//! Configuring the button actions is similar to the process when using
//! \ref window_set_click_config_provider(). See \ref Clicks for more
//! information.
//!
//! \code{.c}
//! ActionBarLayer *action_bar;
//!
//! // The implementation of my_next_click_handler and my_previous_click_handler
//! // is omitted for the sake of brevity. See the Clicks reference docs.
//!
//! void click_config_provider(void *context) {
//!   window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) my_next_click_handler);
//!   window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) my_previous_click_handler);
//! }
//!
//! void window_load(Window *window) {
//!   ...
//!   // Initialize the action bar:
//!   action_bar = action_bar_layer_create();
//!   // Associate the action bar with the window:
//!   action_bar_layer_add_to_window(action_bar, window);
//!   // Set the click config provider:
//!   action_bar_layer_set_click_config_provider(action_bar,
//!                                              click_config_provider);
//!
//!   // Set the icons:
//!   // The loading of the icons is omitted for brevity... See gbitmap_create_with_resource()
//!   action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, my_icon_previous, true);
//!   action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, my_icon_next, true);
//! }
//! \endcode
//! @{

//! The width of the action bar in pixels.
#define ACTION_BAR_WIDTH PBL_IF_RECT_ELSE(30, 40)

//! The maximum number of action bar items.
#define NUM_ACTION_BAR_ITEMS 3

typedef enum {
  ActionBarLayerIconPressAnimationNone = 0,
  ActionBarLayerIconPressAnimationMoveLeft,
  ActionBarLayerIconPressAnimationMoveUp,
  ActionBarLayerIconPressAnimationMoveRight,
  ActionBarLayerIconPressAnimationMoveDown,
} ActionBarLayerIconPressAnimation;

struct ActionBarLayer;
typedef struct ActionBarLayer ActionBarLayer;

//! Creates a new ActionBarLayer on the heap and initalizes it with the default values.
//! * Background color: \ref GColorBlack
//! * No click configuration provider (`NULL`)
//! * No icons
//! * Not added to / associated with any window, thus not catching any button input yet.
//! @return A pointer to the ActionBarLayer. `NULL` if the ActionBarLayer could not
//! be created
ActionBarLayer* action_bar_layer_create(void);

//! Destroys a ActionBarLayer previously created by action_bar_layer_create
void action_bar_layer_destroy(ActionBarLayer *action_bar_layer);

//! Gets the "root" Layer of the action bar layer, which is the parent for the sub-
//! layers used for its implementation.
//! @param action_bar_layer Pointer to the ActionBarLayer for which to get the "root" Layer
//! @return The "root" Layer of the action bar layer.
Layer* action_bar_layer_get_layer(ActionBarLayer *action_bar_layer);

//! Sets the context parameter, which will be passed in to \ref ClickHandler
//! callbacks and the \ref ClickConfigProvider callback of the action bar.
//! @note By default, a pointer to the action bar itself is passed in, if the
//! context has not been set or if it has been set to `NULL`.
//! @param action_bar The action bar for which to assign the new context
//! @param context The new context
//! @see action_bar_layer_set_click_config_provider()
//! @see \ref Clicks
void action_bar_layer_set_context(ActionBarLayer *action_bar, void *context);

//! Sets the click configuration provider callback of the action bar.
//! In this callback your application can associate handlers to the different
//! types of click events for each of the buttons, see \ref Clicks.
//! @note If the action bar had already been added to a window and the window
//! is currently on-screen, the click configuration provider will be called
//! before this function returns. Otherwise, it will be called by the system
//! when the window becomes on-screen.
//! @note The `.raw` handlers cannot be used without breaking the automatic
//! highlighting of the segment of the action bar that for which a button is
//! @see action_bar_layer_set_icon()
//! @param action_bar The action bar for which to assign a new click
//! configuration provider
//! @param click_config_provider The new click configuration provider
void action_bar_layer_set_click_config_provider(ActionBarLayer *action_bar, ClickConfigProvider click_config_provider);

//! Sets an action bar icon onto one of the 3 slots as identified by `button_id`.
//! Only \ref BUTTON_ID_UP, \ref BUTTON_ID_SELECT and \ref BUTTON_ID_DOWN can be
//! used. The transition will not be animated.
//! Whenever an icon is set, the click configuration provider will be
//! called, to give the application the opportunity to reconfigure the button
//! interaction.
//! @param action_bar The action bar for which to set the new icon
//! @param button_id The identifier of the button for which to set the icon
//! @param icon Pointer to the \ref GBitmap icon
//! @see action_bar_layer_set_icon_animated()
//! @see action_bar_layer_set_icon_press_animation()
//! @see action_bar_layer_set_click_config_provider()
void action_bar_layer_set_icon(ActionBarLayer *action_bar, ButtonId button_id, const GBitmap *icon);

//! Convenience function to clear out an existing icon.
//! All it does is call `action_bar_layer_set_icon(action_bar, button_id, NULL)`
//! @param action_bar The action bar for which to clear an icon
//! @param button_id The identifier of the button for which to clear the icon
//! @see action_bar_layer_set_icon()
void action_bar_layer_clear_icon(ActionBarLayer *action_bar, ButtonId button_id);

//! Adds the action bar's layer on top of the window's root layer. It also
//! adjusts the layout of the action bar to match the geometry of the window it
//! gets added to.
//! Lastly, it calls \ref window_set_click_config_provider_with_context() on
//! the window to set it up to work with the internal callback and raw click
//! handlers of the action bar, to enable the highlighting of the section of the
//! action bar when the user presses a button.
//! @note After this call, do not use
//! \ref window_set_click_config_provider_with_context() with the window that
//! the action bar has been added to (this would de-associate the action bar's
//! click config provider and context). Instead use
//! \ref action_bar_layer_set_click_config_provider() and
//! \ref action_bar_layer_set_context() to register the click configuration
//! provider to configure the buttons actions.
//! @note It is advised to call this is in the window's `.load` or `.appear`
//! handler. Make sure to call \ref action_bar_layer_remove_from_window() in the
//! window's `.unload` or `.disappear` handler.
//! @note Adding additional layers to the window's root layer after this calll
//! can occlude the action bar.
//! @param action_bar The action bar to associate with the window
//! @param window The window with which the action bar is to be associated
void action_bar_layer_add_to_window(ActionBarLayer *action_bar, struct Window *window);

//! Removes the action bar from the window and unconfigures the window's
//! click configuration provider. `NULL` is set as the window's new click config
//! provider and also as its callback context. If it has not been added to a
//! window before, this function is a no-op.
//! @param action_bar The action bar to de-associate from its current window
void action_bar_layer_remove_from_window(ActionBarLayer *action_bar);

//! Sets the background color of the action bar. Defaults to \ref GColorBlack.
//! The action bar's layer is automatically marked dirty.
//! @param action_bar The action bar of which to set the background color
//! @param background_color The new background color
void action_bar_layer_set_background_color(ActionBarLayer *action_bar, GColor background_color);

//! Sets an action bar icon onto one of the 3 slots as identified by `button_id`.
//! Only \ref BUTTON_ID_UP, \ref BUTTON_ID_SELECT and \ref BUTTON_ID_DOWN can be
//! used. Optionally, if `animated` is `true`, the transition will be animated.
//! Whenever an icon is set, the click configuration provider will be called,
//! to give the application the opportunity to reconfigure the button interaction.
//! @param action_bar The action bar for which to set the new icon
//! @param button_id The identifier of the button for which to set the icon
//! @param icon Pointer to the \ref GBitmap icon
//! @param animated True = animate the transition, False = do not animate the transition
//! @see action_bar_layer_set_icon()
//! @see action_bar_layer_set_icon_press_animation()
//! @see action_bar_layer_set_click_config_provider()
void action_bar_layer_set_icon_animated(ActionBarLayer *action_bar, ButtonId button_id,
                                        const GBitmap *icon, bool animated);

//! Sets the animation to use while a button is pressed on an ActionBarLayer.
//! By default we use ActionBarLayerIconPressAnimationMoveLeft
//! @param action_bar The action bar for which to set the press animation
//! @param button_id The button for which to set the press animation
//! @param animation The animation to use.
//! @see action_bar_layer_set_icon_animated()
//! @see action_bar_layer_set_click_config_provider()
void action_bar_layer_set_icon_press_animation(ActionBarLayer *action_bar, ButtonId button_id,
                                               ActionBarLayerIconPressAnimation animation);

//! @} // group ActionBarLayer

//! @addtogroup StatusBarLayer
//! \brief Layer that serves as a configurable status bar.
//! @{

struct StatusBarLayer;
typedef struct StatusBarLayer StatusBarLayer;

//! Values that are used to indicate the different status bar separator modes.
typedef enum {
  //! The default mode. No separator will be shown.
  StatusBarLayerSeparatorModeNone = 0,
  //! A dotted separator at the bottom of the status bar.
  StatusBarLayerSeparatorModeDotted = 1,
} StatusBarLayerSeparatorMode;

//! The fixed height of the status bar, including separator height
#define STATUS_BAR_LAYER_HEIGHT PBL_IF_RECT_ELSE(16, 24)

//! Creates a new StatusBarLayer on the heap and initializes it with the default values.
//!
//! * Text color: \ref GColorBlack
//! * Background color: \ref GColorWhite
//! * Frame: `GRect(0, 0, screen_width, STATUS_BAR_LAYER_HEIGHT)`
//! The status bar is automatically marked dirty after this operation.
//! You can call \ref layer_set_frame() to create a StatusBarLayer of a different width.
//!
//! \code{.c}
//! // Change the status bar width to make space for the action bar
//! int16_t width = layer_get_bounds(root_layer).size.w - ACTION_BAR_WIDTH;
//! GRect frame = GRect(0, 0, width, STATUS_BAR_LAYER_HEIGHT);
//! layer_set_frame(status_bar_layer_get_layer(status_bar), frame);
//! layer_add_child(root_layer, status_bar_layer_get_layer(status_bar));
//! \endcode
//! @return A pointer to the StatusBarLayer, which will be allocated to the heap,
//! `NULL` if the StatusBarLayer could not be created
StatusBarLayer *status_bar_layer_create(void);

//! Destroys a StatusBarLayer previously created by status_bar_layer_create.
//! @param status_bar_layer The StatusBarLayer to destroy
void status_bar_layer_destroy(StatusBarLayer *status_bar_layer);

//! Gets the "root" Layer of the status bar, which is the parent for the sub-
//! layers used for its implementation.
//! @param status_bar_layer Pointer to the StatusBarLayer for which to get the "root" Layer
//! @return The "root" Layer of the status bar.
//! @note The result is always equal to `(Layer *) status_bar_layer`.
Layer *status_bar_layer_get_layer(StatusBarLayer *status_bar_layer);

//! Gets background color of StatusBarLayer
//! @param status_bar_layer The StatusBarLayer of which to get the color
//! @return GColor of background color property
GColor status_bar_layer_get_background_color(const StatusBarLayer *status_bar_layer);

//! Gets foreground color of StatusBarLayer
//! @param status_bar_layer The StatusBarLayer of which to get the color
//! @return GColor of foreground color property
GColor status_bar_layer_get_foreground_color(const StatusBarLayer *status_bar_layer);

//! Sets the background and foreground colors of StatusBarLayer
//! @param status_bar_layer The StatusBarLayer of which to set the colors
//! @param background The new \ref GColor to set for background
//! @param foreground The new \ref GColor to set for text and other foreground elements
void status_bar_layer_set_colors(StatusBarLayer *status_bar_layer, GColor background,
                                 GColor foreground);

//! Sets the mode of the StatusBarLayer separator, to help divide it from content
//! @param status_bar_layer The StatusBarLayer of which to set the separator mode
//! @param mode Determines the separator mode
void status_bar_layer_set_separator_mode(StatusBarLayer *status_bar_layer,
                                         StatusBarLayerSeparatorMode mode);

//! @} // group StatusBarLayer

//! @addtogroup BitmapLayer
//! \brief Layer that displays a bitmap image.
//!
//! ![](bitmap_layer.png)
//! BitmapLayer is a Layer subtype that draws a GBitmap within its frame. It uses an alignment property
//! to specify how to position the bitmap image within its frame. Optionally, when the
//! background color is not GColorClear, it draws a solid background color behind the
//! bitmap image, filling areas of the frame that are not covered by the bitmap image.
//! Lastly, using the compositing mode property of the BitmapLayer, determines the way the
//! bitmap image is drawn on top of what is underneath it (either the background color, or
//! the layers beneath it).
//!
//! <h3>Inside the Implementation</h3>
//! The implementation of BitmapLayer is fairly straightforward and relies heavily on the
//! functionality as exposed by the core drawing functions (see \ref Drawing).
//! \ref BitmapLayer's drawing callback uses \ref graphics_draw_bitmap_in_rect()
//! to perform the actual drawing of the \ref GBitmap. It uses \ref grect_align() to perform
//! the layout of the image and it uses \ref graphics_fill_rect() to draw the background plane.
//! @{

struct BitmapLayer;
typedef struct BitmapLayer BitmapLayer;

//! Creates a new bitmap layer on the heap and initalizes it the default values.
//!
//! * Bitmap: `NULL` (none)
//! * Background color: \ref GColorClear
//! * Compositing mode: \ref GCompOpAssign
//! * Clips: `true`
//! @return A pointer to the BitmapLayer. `NULL` if the BitmapLayer could not
//! be created
BitmapLayer* bitmap_layer_create(GRect frame);

//! Destroys a window previously created by bitmap_layer_create
void bitmap_layer_destroy(BitmapLayer* bitmap_layer);

//! Gets the "root" Layer of the bitmap layer, which is the parent for the sub-
//! layers used for its implementation.
//! @param bitmap_layer Pointer to the BitmapLayer for which to get the "root" Layer
//! @return The "root" Layer of the bitmap layer.
Layer* bitmap_layer_get_layer(const BitmapLayer *bitmap_layer);

//! Gets the pointer to the bitmap image that the BitmapLayer is using.
//!
//! @param bitmap_layer The BitmapLayer for which to get the bitmap image
//! @return A pointer to the bitmap image that the BitmapLayer is using
const GBitmap* bitmap_layer_get_bitmap(BitmapLayer *bitmap_layer);

//! Sets the bitmap onto the BitmapLayer. The bitmap is set by reference (no deep
//! copy), thus the caller of this function has to make sure the bitmap is kept
//! in memory.
//!
//! The bitmap layer is automatically marked dirty after this operation.
//! @param bitmap_layer The BitmapLayer for which to set the bitmap image
//! @param bitmap The new \ref GBitmap to set onto the BitmapLayer
void bitmap_layer_set_bitmap(BitmapLayer *bitmap_layer, const GBitmap *bitmap);

//! Sets the alignment of the image to draw with in frame of the BitmapLayer.
//! The aligment parameter specifies which edges of the bitmap should overlap
//! with the frame of the BitmapLayer.
//! If the bitmap is smaller than the frame of the BitmapLayer, the background
//! is filled with the background color.
//!
//! The bitmap layer is automatically marked dirty after this operation.
//! @param bitmap_layer The BitmapLayer for which to set the aligment
//! @param alignment The new alignment for the image inside the BitmapLayer
void bitmap_layer_set_alignment(BitmapLayer *bitmap_layer, GAlign alignment);

//! Sets the background color of bounding box that will be drawn behind the image
//! of the BitmapLayer.
//!
//! The bitmap layer is automatically marked dirty after this operation.
//! @param bitmap_layer The BitmapLayer for which to set the background color
//! @param color The new \ref GColor to set the background to
void bitmap_layer_set_background_color(BitmapLayer *bitmap_layer, GColor color);

//! Sets the compositing mode of how the bitmap image is composited onto the
//! BitmapLayer's background plane, or how it is composited onto what has been
//! drawn beneath the BitmapLayer.
//!
//! The compositing mode only affects the drawing of the bitmap and not the
//! drawing of the background color.
//!
//! For Aplite, there is no notion of "transparency" in the graphics system. However, the effect of
//! transparency can be created by masking and using compositing modes.
//!
//! For Basalt, when drawing \ref GBitmap images, \ref GCompOpSet will be required to apply any
//! transparency.
//!
//! The bitmap layer is automatically marked dirty after this operation.
//! @param bitmap_layer The BitmapLayer for which to set the compositing mode
//! @param mode The compositing mode to set
//! @see See \ref GCompOp for visual examples of the different compositing modes.
void bitmap_layer_set_compositing_mode(BitmapLayer *bitmap_layer, GCompOp mode);

//! @} // group BitmapLayer

//! @addtogroup RotBitmapLayer
//! \brief Layer that displays a rotated bitmap image.
//!
//! A RotBitmapLayer is like a \ref BitmapLayer but has the ability to be rotated (by default, around its center). The amount of rotation
//! is specified using \ref rot_bitmap_layer_set_angle() or \ref rot_bitmap_layer_increment_angle(). The rotation argument
//! to those functions is specified as an amount of clockwise rotation, where the value 0x10000 represents a full 360 degree
//! rotation and 0 represent no rotation, and it scales linearly between those values, just like \ref sin_lookup.
//!
//! The center of rotation in the source bitmap is always placed at the center of the RotBitmapLayer and the size of the RotBitmapLayer
//! is automatically calculated so that the entire Bitmap can fit in at all rotation angles.
//!
//! For example, if the image is 10px wide and high, the RotBitmapLayer will be 14px wide ( sqrt(10^2+10^2) ).
//!
//! By default, the center of rotation in the source bitmap is the center of the bitmap but you can call \ref rot_bitmap_set_src_ic() to change the
//! center of rotation.
//!
//! @note RotBitmapLayer has performance limitations that can degrade user
//! experience (see \ref graphics_draw_rotated_bitmap). Use sparingly.
//! @{

struct RotBitmapLayer;
typedef struct RotBitmapLayer RotBitmapLayer;

//! Creates a new RotBitmapLayer on the heap and initializes it with the default values:
//!  * Angle: 0
//!  * Compositing mode: \ref GCompOpAssign
//!  * Corner clip color: \ref GColorClear
//!
//! @param bitmap The bitmap to display in this RotBitmapLayer
//! @return A pointer to the RotBitmapLayer. `NULL` if the RotBitmapLayer could not
//! be created
RotBitmapLayer* rot_bitmap_layer_create(GBitmap *bitmap);

//! Destroys a RotBitmapLayer and frees all associated memory.
//! @note It is the developer responsibility to free the \ref GBitmap.
//! @param bitmap The RotBitmapLayer to destroy.
void rot_bitmap_layer_destroy(RotBitmapLayer *bitmap);

//! Defines what color to use in areas that are not covered by the source bitmap.
//! By default this is \ref GColorClear.
//! @param bitmap The RotBitmapLayer on which to change the corner clip color
//! @param color The corner clip color
void rot_bitmap_layer_set_corner_clip_color(RotBitmapLayer *bitmap, GColor color);

//! Sets the rotation angle of this RotBitmapLayer
//! @param bitmap The RotBitmapLayer on which to change the rotation
//! @param angle Rotation is an integer between 0 (no rotation) and 0x10000 (360 degree rotation). @see sin_lookup()
void rot_bitmap_layer_set_angle(RotBitmapLayer *bitmap, int32_t angle);

//! Change the rotation angle of this RotBitmapLayer
//! @param bitmap The RotBitmapLayer on which to change the rotation
//! @param angle_change The rotation angle change
void rot_bitmap_layer_increment_angle(RotBitmapLayer *bitmap, int32_t angle_change);

//! Defines the only point that will not be affected by the rotation in the source bitmap.
//!
//! For example, if you pass GPoint(0, 0), the image will rotate around the top-left corner.
//!
//! This point is always projected at the center of the RotBitmapLayer. Calling this function
//! automatically adjusts the width and height of the RotBitmapLayer so that
//! the entire bitmap can fit inside the layer at all rotation angles.
//!
//! @param bitmap The RotBitmapLayer on which to change the rotation
//! @param ic The only point in the original image that will not be affected by the rotation.
void rot_bitmap_set_src_ic(RotBitmapLayer *bitmap, GPoint ic);

//! Sets the compositing mode of how the bitmap image is composited onto what has been drawn beneath the
//! RotBitmapLayer.
//! By default this is \ref GCompOpAssign.
//! The RotBitmapLayer is automatically marked dirty after this operation.
//! @param bitmap The RotBitmapLayer on which to change the rotation
//! @param mode The compositing mode to set
//! @see \ref GCompOp for visual examples of the different compositing modes.
void rot_bitmap_set_compositing_mode(RotBitmapLayer *bitmap, GCompOp mode);

//! @} // group RotBitmapLayer

//! @} // group Layer

//! @addtogroup Window
//! @{

//! @addtogroup NumberWindow
//! \brief A ready-made Window prompting the user to pick a number
//!
//! ![](number_window.png)
//! @{

struct NumberWindow;
typedef struct NumberWindow NumberWindow;

//! Function signature for NumberWindow callbacks.
typedef void (*NumberWindowCallback)(struct NumberWindow *number_window, void *context);

//! Data structure containing all the callbacks for a NumberWindow.
typedef struct {
  //! Callback that gets called as the value is incremented.
  //! Optional, leave `NULL` if unused.
  NumberWindowCallback incremented;
  //! Callback that gets called as the value is decremented.
  //! Optional, leave `NULL` if unused.
  NumberWindowCallback decremented;
  //! Callback that gets called as the value is confirmed, in other words the
  //! SELECT button is clicked.
  //! Optional, leave `NULL` if unused.
  NumberWindowCallback selected;
} NumberWindowCallbacks;

//! Creates a new NumberWindow on the heap and initalizes it with the default values.
//!
//! @param label The title or prompt to display in the NumberWindow. Must be long-lived and cannot be stack-allocated.
//! @param callbacks The callbacks
//! @param callback_context Pointer to application specific data that is passed
//! @note The number window is not pushed to the window stack. Use \ref window_stack_push() to do this.
//! @return A pointer to the NumberWindow. `NULL` if the NumberWindow could not
//! be created
NumberWindow* number_window_create(const char *label, NumberWindowCallbacks callbacks, void *callback_context);

//! Destroys a NumberWindow previously created by number_window_create.
void number_window_destroy(NumberWindow* number_window);

//! Sets the text of the title or prompt label.
//! @param numberwindow Pointer to the NumberWindow for which to set the label
//! text
//! @param label The new label text. Must be long-lived and cannot be
//! stack-allocated.
void number_window_set_label(NumberWindow *numberwindow, const char *label);

//! Sets the maximum value this field can hold
//! @param numberwindow Pointer to the NumberWindow for which to set the maximum
//! value
//! @param max The maximum value
void number_window_set_max(NumberWindow *numberwindow, int32_t max);

//! Sets the minimum value this field can hold
//! @param numberwindow Pointer to the NumberWindow for which to set the minimum
//! value
//! @param min The minimum value
void number_window_set_min(NumberWindow *numberwindow, int32_t min);

//! Sets the current value of the field
//! @param numberwindow Pointer to the NumberWindow for which to set the current
//! value
//! @param value The new current value
void number_window_set_value(NumberWindow *numberwindow, int32_t value);

//! Sets the amount by which to increment/decrement by on a button click
//! @param numberwindow Pointer to the NumberWindow for which to set the step
//! increment
//! @param step The new step increment
void number_window_set_step_size(NumberWindow *numberwindow, int32_t step);

//! Gets the current value
//! @param numberwindow Pointer to the NumberWindow for which to get the current
//! value
//! @return The current value
int32_t number_window_get_value(const NumberWindow *numberwindow);

//! Gets the "root" Window of the number window
//! @param numberwindow Pointer to the NumberWindow for which to get the "root" Window
//! @return The "root" Window of the number window.
Window *number_window_get_window(NumberWindow *numberwindow);

//! @} // group NumberWindow

//! @addtogroup ActionMenu
//! @{

struct ActionMenuItem;
typedef struct ActionMenuItem ActionMenuItem;

struct ActionMenuLevel;
typedef struct ActionMenuLevel ActionMenuLevel;

typedef enum {
  ActionMenuAlignTop = 0,
  ActionMenuAlignCenter
} ActionMenuAlign;

typedef struct ActionMenu ActionMenu;

//! Callback executed after the ActionMenu has closed, so memory may be freed.
//! @param root_level the root level passed to the ActionMenu
//! @param performed_action the ActionMenuItem for the action that was performed,
//! NULL if the ActionMenu is closing without an action being selected by the user
//! @param context the context passed to the ActionMenu
typedef void (*ActionMenuDidCloseCb)(ActionMenu *menu,
                                     const ActionMenuItem *performed_action,
                                     void *context);

//! enum value that controls whether menu items are displayed in a grid
//! (similarly to the emoji replies) or in a single column (reminiscent of \ref MenuLayer)
typedef enum {
  ActionMenuLevelDisplayModeWide, //!< Each item gets its own row
  ActionMenuLevelDisplayModeThin, //!< Grid view: multiple items per row
} ActionMenuLevelDisplayMode;

//! Callback executed when a given action is selected
//! @param action_menu the action menu currently on screen
//! @param action the action that was triggered
//! @param context the context passed to the action menu
//! @note the action menu is closed immediately after an action is performed,
//! unless it is frozen in the ActionMenuPerformActionCb
typedef void (*ActionMenuPerformActionCb)(ActionMenu *action_menu,
                                          const ActionMenuItem *action,
                                          void *context);

//! Callback invoked for each item in an action menu hierarchy.
//! @param item the current action menu item
//! @param a caller-provided context callback
typedef void (*ActionMenuEachItemCb)(const ActionMenuItem *item, void *context);

//! Configuration struct for the ActionMenu
typedef struct {
  const ActionMenuLevel *root_level; //!< the root level of the ActionMenu
  void *context; //!< a context pointer which will be accessbile when actions are performed
  struct {
    GColor background; //!< the color of the left column of the ActionMenu
    GColor foreground; //!< the color of the individual "crumbs" that indicate menu depth
  } colors;
  ActionMenuDidCloseCb will_close; //!< Called immediately before the ActionMenu closes
  ActionMenuDidCloseCb did_close; //!< a callback used to cleanup memory after the menu has closed
  ActionMenuAlign align;
} ActionMenuConfig;

//! Getter for the label of a given \ref ActionMenuItem
//! @param item the \ref ActionMenuItem of interest
//! @return a pointer to the string label. NULL if invalid.
char *action_menu_item_get_label(const ActionMenuItem *item);

//! Getter for the action_data pointer of a given \ref ActionMenuitem.
//! @see action_menu_level_add_action
//! @param item the \ref ActionMenuItem of interest
//! @return a pointer to the data. NULL if invalid.
void *action_menu_item_get_action_data(const ActionMenuItem *item);

//! Create a new action menu level with storage allocated for a given number of items
//! @param max_items the max number of items that will be displayed at that level
//! @note levels are freed alongside the whole hierarchy so no destroy API is provided.
//! @note by default, levels are using ActionMenuLevelDisplayModeWide.
//! Use \ref action_menu_level_set_display_mode to change it.
//! @see action_menu_hierarchy_destroy
ActionMenuLevel *action_menu_level_create(uint16_t max_items);

//! Set the action menu display mode
//! @param level The ActionMenuLevel whose display mode you want to change
//! @param display_mode The display mode for the action menu (3 vs. 1 item per row)
void action_menu_level_set_display_mode(ActionMenuLevel *level,
                                        ActionMenuLevelDisplayMode display_mode);

//! Add an action to an ActionLevel
//! @param level the level to add the action to
//! @param label the text to display for the action in the menu
//! @param cb the callback that will be triggered when this action is actuated
//! @param action_data data to pass to the callback for this action
//! @return a reference to the new \ref ActionMenuItem on success, NULL if the level is full
ActionMenuItem *action_menu_level_add_action(ActionMenuLevel *level,
                                             const char *label,
                                             ActionMenuPerformActionCb cb,
                                             void *action_data);

//! Add a child to this ActionMenuLevel
//! @param level the parent level
//! @param child the child level
//! @param label the text to display in the action menu for this level
//! @return a reference to the new \ref ActionMenuItem on success, NULL if the level is full
ActionMenuItem *action_menu_level_add_child(ActionMenuLevel *level,
                                            ActionMenuLevel *child,
                                            const char *label);

//! Destroy a hierarchy of ActionMenuLevels
//! @param root the root level in the hierarchy
//! @param each_cb a callback to call on every \ref ActionMenuItem in every level
//! @param context a context pointer to pass to each_cb on invocation
//! @note Typical implementations will cleanup memory allocated for the item label/data
//!       associated with each item in the callback
//! @note Hierarchy is traversed in post-order.
//!       In other words, all children items are freed before their parent is freed.
void action_menu_hierarchy_destroy(const ActionMenuLevel *root,
                                   ActionMenuEachItemCb each_cb,
                                   void *context);

//! Get the context pointer this ActionMenu was created with
//! @param action_menu A pointer to an ActionMenu
//! @return the context pointer initially provided in the \ref ActionMenuConfig.
//! NULL if none exists.
void *action_menu_get_context(ActionMenu *action_menu);

//! Get the root level of an ActionMenu
//! @param action_menu the ActionMenu you want to know about
//! @return a pointer to the root ActionMenuLevel for the given ActionMenu, NULL if invalid
ActionMenuLevel *action_menu_get_root_level(ActionMenu *action_menu);

//! Open a new ActionMenu.
//! The ActionMenu acts much like a window. It fills the whole screen and handles clicks.
//! @param config the configuration info for this new ActionMenu
//! @return the new ActionMenu
ActionMenu *action_menu_open(ActionMenuConfig *config);

//! Freeze the ActionMenu. The ActionMenu will no longer respond to user input.
//! @note this API should be used when waiting for asynchronous operation.
//! @param action_menu the ActionMenu
void action_menu_freeze(ActionMenu *action_menu);

//! Unfreeze the ActionMenu previously frozen with \ref action_menu_freeze
//! @param action_menu the ActionMenu to unfreeze
void action_menu_unfreeze(ActionMenu *action_menu);

//! Set the result window for an ActionMenu. The result window will be
//! shown when the ActionMenu closes
//! @param action_menu the ActionMenu
//! @param result_window the window to insert, pass NULL to remove the current result window
//! @note repeated call will result in only the last call to be applied, i.e. only
//! one result window is ever set
void action_menu_set_result_window(ActionMenu *action_menu, Window *result_window);

//! Close the ActionMenu, whether it is frozen or not.
//! @note this API can be used on a frozen ActionMenu once the data required to
//! build the result window has been received and the result window has been set
//! @param action_menu the ActionMenu to close
//! @param animated whether or not show a close animation
void action_menu_close(ActionMenu *action_menu, bool animated);

//! @} // group ActionMenu

//! @} // group Window

//! @addtogroup Vibes
//! \brief Controlling the vibration motor
//!
//! The Vibes API provides calls that let you control Pebble’s vibration motor.
//!
//! The vibration motor can be used as a visceral mechanism for giving immediate feedback to the user.
//! You can use it to highlight important moments in games, or to draw the attention of the user.
//! However, you should use the vibration feature sparingly, because sustained use will rapidly deplete Pebble’s battery,
//! and vibrating Pebble too much and too often can become annoying for users.
//! @note When using these calls, if there is an ongoing vibration,
//! calling any of the functions to emit (another) vibration will have no effect.
//! @{

/** Data structure describing a vibration pattern.
 A pattern consists of at least 1 vibe-on duration, optionally followed by
 alternating vibe-off + vibe-on durations. Each segment may have a different duration.

 Example code:
 \code{.c}
 // Vibe pattern: ON for 200ms, OFF for 100ms, ON for 400ms:
static const uint32_t const segments[] = { 200, 100, 400 };
VibePattern pat = {
  .durations = segments,
  .num_segments = ARRAY_LENGTH(segments),
};
vibes_enqueue_custom_pattern(pat);
\endcode
 @see vibes_enqueue_custom_pattern
 */
typedef struct {
  /**
   Pointer to an array of segment durations, measured in milli-seconds.
   The maximum allowed duration is 10000ms.
   */
  const uint32_t *durations;
  /**
   The length of the array of durations.
   */
  uint32_t num_segments;
} VibePattern;

//! Cancel any in-flight vibe patterns; this is a no-op if there is no
//! on-going vibe.
void vibes_cancel(void);

//! Makes the watch emit one short vibration.
void vibes_short_pulse(void);

//! Makes the watch emit one long vibration.
void vibes_long_pulse(void);

//! Makes the watch emit two brief vibrations.
//!
void vibes_double_pulse(void);

//! Makes the watch emit a ‘custom’ vibration pattern.
//! @param pattern An arbitrary vibration pattern
//! @see VibePattern
void vibes_enqueue_custom_pattern(VibePattern pattern);

//! @} // group Vibes

//! @addtogroup Light Light
//! \brief Controlling Pebble's backlight
//!
//! The Light API provides you with functions to turn on Pebble’s backlight or
//! put it back into automatic control. You can trigger the backlight and schedule a timer
//! to automatically disable the backlight after a short delay, which is the preferred
//! method of interacting with the backlight.
//! @{

//! Trigger the backlight and schedule a timer to automatically disable the backlight
//! after a short delay. This is the preferred method of interacting with the backlight.
void light_enable_interaction(void);

//! Turn the watch's backlight on or put it back into automatic control.
//! Developers should take care when calling this function, keeping Pebble's backlight on for long periods of time
//! will rapidly deplete the battery.
//! @param enable Turn the backlight on if `true`, otherwise `false` to put it back into automatic control.
void light_enable(bool enable);

//! @} // group Light

//! @addtogroup Preferences
//!
//! \brief Values recommended by the system
//!
//! @{

//! Get the recommended amount of milliseconds a result window should be visible before it should
//! automatically close.
//! @note It is the application developer's responsibility to automatically close a result window.
//! @return The recommended result window timeout duration in milliseconds
#define preferred_result_display_duration() (1000)

//! PreferredContentSize represents the display scale of all the app's UI components. The enum
//! contains all sizes that all platforms as a whole are capable of displaying, but each individual
//! platform may not be able to display all sizes.
//! @note As of version 4.1, platforms other than Emery cannot display extra large and Emery itself
//! cannot display small.
typedef enum PreferredContentSize {
  PreferredContentSizeSmall,
  PreferredContentSizeMedium,
  PreferredContentSizeLarge,
  PreferredContentSizeExtraLarge,
  NumPreferredContentSizes,
} PreferredContentSize;

//! Returns the user's preferred content size representing the scale of all the app's UI components
//! should use for display.
//! @return The user's \ref PreferredContentSize setting.
#define preferred_content_size() (PreferredContentSizeMedium)

//! Users can toggle Quiet Time manually or on schedule. Watchfaces and apps should respect this
//! choice and avoid disturbing actions such as vibration if quiet time is active.
//! @return True, if Quiet Time is currently active.
#define quiet_time_is_active() (false)

//! @} // group Preferences

//! @} // group UI

//! @addtogroup Profiling
//! @{

//! @} // group Profiling

//! @addtogroup StandardC Standard C
//! @{

//! @addtogroup StandardTime Time
//! \brief Standard system time functions
//!
//! This module contains standard time functions and formatters for printing.
//! Note that Pebble now supports both local time and UTC time
//! (including timezones and daylight savings time).
//! Most of these functions are part of the C standard library which is documented at
//! https://sourceware.org/newlib/libc.html#Timefns
//! @{

#define TZ_LEN 6

#define SECONDS_PER_MINUTE (60)

#define MINUTES_PER_HOUR (60)

#define SECONDS_PER_HOUR (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)

#define HOURS_PER_DAY (24)

#define MINUTES_PER_DAY (HOURS_PER_DAY * MINUTES_PER_HOUR)

#define SECONDS_PER_DAY (MINUTES_PER_DAY * SECONDS_PER_MINUTE)

//! structure containing broken-down time for expressing calendar time
//! (ie. Year, Month, Day of Month, Hour of Day) and timezone information
struct tm {
  int tm_sec;     /*!< Seconds. [0-60] (1 leap second) */
  int tm_min;     /*!< Minutes. [0-59] */
  int tm_hour;    /*!< Hours.  [0-23] */
  int tm_mday;    /*!< Day. [1-31] */
  int tm_mon;     /*!< Month. [0-11] */
  int tm_year;    /*!< Years since 1900 */
  int tm_wday;    /*!< Day of week. [0-6] */
  int tm_yday;    /*!< Days in year.[0-365] */
  int tm_isdst;   /*!< DST. [-1/0/1] */

  int tm_gmtoff;  /*!< Seconds east of UTC */
  char tm_zone[TZ_LEN]; /*!< Timezone abbreviation */
};

//! Format the time value at tm according to fmt and place the result in a buffer s of size max
//! @param s A preallocation char array of size max
//! @param maxsize the size of the array s
//! @param format a formatting string
//! @param tm_p A pointer to a struct tm containing a broken out time value
//! @return The number of bytes placed in the array s, not including the null byte,
//!   0 if the value does not fit.
int strftime(char* s, size_t maxsize, const char* format, const struct tm* tm_p);

//! convert the time value pointed at by clock to a struct tm which contains the time
//! adjusted for the local timezone
//! @param timep A pointer to an object of type time_t that contains a time value
//! @return A pointer to a struct tm containing the broken out time value adjusted
//!   for the local timezone
struct tm *localtime(const time_t *timep);

//! convert the time value pointed at by clock to a struct tm
//!   which contains the time expressed in Coordinated Universal Time (UTC)
//! @param timep A pointer to an object of type time_t that contains a time value
//! @return A pointer to a struct tm containing Coordinated Universal Time (UTC)
struct tm *gmtime(const time_t *timep);

//! convert the broken-down time structure to a timestamp
//!   expressed in Coordinated Universal Time (UTC)
//! @param tb A pointer to an object of type tm that contains broken-down time
//! @return The number of seconds since epoch, January 1st 1970
time_t mktime(struct tm *tb);

//! Obtain the number of seconds since epoch.
//! Note that the epoch is not adjusted for Timezones and Daylight Savings.
//! @param tloc Optionally points to an address of a time_t variable to store the time in.
//!     If you only want to use the return value, you may pass NULL into tloc instead
//! @return The number of seconds since epoch, January 1st 1970
time_t time(time_t *tloc);

//! Obtain the number of seconds elapsed between beginning and end represented as a double.
//! @param end A time_t variable representing some number of seconds since epoch, January 1st 1970
//! @param beginning A time_t variable representing some number of seconds since epoch,
//!     January 1st 1970. Note that end should be greater than beginning, but this is not enforced.
//! @return The number of seconds elapsed between beginning and end.
//! @note Pebble uses software floating point emulation.  Including this function which returns a
//!     double will significantly increase the size of your binary.  We recommend directly
//!     subtracting both timestamps to calculate a time difference.
//!     \code{.c}
//!     int difference = ts1 - ts2;
//!     \endcode
double difftime(time_t end, time_t beginning);

//! Obtain the number of seconds and milliseconds part since the epoch.
//!   This is a non-standard C function provided for convenience.
//! @param tloc Optionally points to an address of a time_t variable to store the time in.
//!   You may pass NULL into tloc if you don't need a time_t variable to be set
//!   with the seconds since the epoch
//! @param out_ms Optionally points to an address of a uint16_t variable to store the
//!   number of milliseconds since the last second in.
//!   If you only want to use the return value, you may pass NULL into out_ms instead
//! @return The number of milliseconds since the last second
uint16_t time_ms(time_t *t_utc, uint16_t *out_ms);

//! Return the UTC time that corresponds to the start of today (midnight).
//! @return the UTC time corresponding to the start of today (midnight)
time_t time_start_of_today(void);

//! @} // group StandardTime

//! @} // group StandardC

