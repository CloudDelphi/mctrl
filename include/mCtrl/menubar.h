/*
 * Copyright (c) 2012 Martin Mitas
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MCTRL_MENUBAR_H
#define MCTRL_MENUBAR_H

#include <mCtrl/defs.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file
 * @brief Menu bar control (@c MC_WC_MENUBAR).
 *
 * The @c MC_WC_MENUBAR is implentation of a control generally known as
 * Internet Explorer-Style Menu Bar. It is a control which can host a menu
 * (here represented by a menu handle, @c HMENU), but which generally works
 * as a toolbar.
 *
 * The standard menus take whole width of the window for their menubars,
 * and there can only be used one menu in a top-level windows. Child windows
 * cann ot have a menu at all (well, we are not talking about popup menus).
 *
 * The @c MC_WC_MENUBAR offers solution to these problems. It has been designed
 * with especially following use cases in mind:
 *
 * - Embedding the menu into a standard ReBar control from @c COMCTL32.DLL.
 *
 * - Positioning the contol on other position or with different size then what
 *   is normally enforced for normale menu of a window or dialog.
 *
 * - Possibility to create this control in child windows, or having multiple
 *   menubars in a single window.
 *
 *
 * @section sec_mb_subclass Subclassed Toolbar
 *
 * Actually the @c MC_WC_MENUBAR is implemented as a subclass of the standard
 * toolbar (from @c COMCTL32.DLL) control, so you can use its style, and also
 * some toolbar messages.
 *
 * Of course there are also differences: The menubar control automatically sets
 * some toolbar styles when created, as it sees fit for its purpose. Application
 * still can reset it with @c SetWindowLong and @c GWL_STYLE.
 *
 * Furthermore the menubar control does not support toolbar messages which add,
 * modify or remove toolbar buttons. The control just manages them
 * automatically to reflect the installed menu.
 *
 * I.e. sending any of these toolbar messages to the control always fails:
 * - @c TB_ADDBITMAP
 * - @c TB_ADDSTRING
 * - @c TB_ADDBUTTONS
 * - @c TB_BUTTONSTRUCTSIZE
 * - @c TB_CHANGEBITMAP
 * - @c TB_CUSTOMIZE
 * - @c TB_DELETEBUTTON
 * - @c TB_ENABLEBUTTON
 * - @c TB_HIDEBUTTON
 * - @c TB_INDETERMINATE
 * - @c TB_INSERTBUTTON
 * - @c TB_LOADIMAGES
 * - @c TB_MARKBUTTON
 * - @c TB_MOVEBUTTON
 * - @c TB_PRESSBUTTON
 * - @c TB_REPLACEBITMAP
 * - @c TB_SAVERESTORE
 * - @c TB_SETANCHORHIGHLIGHT
 * - @c TB_SETBITMAPSIZE
 * - @c TB_SETBOUNDINGSIZE
 * - @c TB_SETCMDID
 * - @c TB_SETDISABLEDIMAGELIST
 * - @c TB_SETHOTIMAGELIST
 * - @c TB_SETIMAGELIST
 * - @c TB_SETINSERTMARK
 * - @c TB_SETPRESSEDIMAGELIST
 * - @c TB_SETSTATE
 *
 *
 * @section sec_mb_create Installing a Menu
 *
 * To install a menu in the menubar, you may set parameter @c lpParam of
 * @c CreateWindow or @c CreateWindowEx to the handle of the menu (@c HMENU).
 * Or, after the menubar is created, you may install a menu with the
 * message @c MC_MBM_SETMENU.
 *
 * Either way the application is responsible to keep the menu handle valid
 * as long as the menubar exists (or until other menu is installed in the
 * menubar).
 *
 * Note however that changes to the menu are not automatically rflected in the
 * menubar. If application programatically changes top-level items of the menu
 * (for example add new popups, disable some of them etc.), it then has to
 * send @c MC_MBM_REFRESH to reflect the changes.
 *
 *
 * @section sec_mb_notifications Notifications
 *
 * The control sends notifications of both, the toolbar and menu.
 *
 * To handle the actions corresponding to the menu items, application
 * uses the notification @c WM_COMMAND as with a normal menu. It can also
 * take use of @c WM_MENUSELECT and @c WM_INITMENU.
 *
 * Toolbar notifications are sent through @c WM_NOTIFY. For example,
 * @c TBN_DROPDOWN or @c TBN_HOTITEMCHANGE are sent as any other notifications
 * normal toolbar fires.
 *
 * All the notifications are sent by default to a window which was parent of
 * the menubar when creating the menubar. One exception is if the parent is
 * a ReBar control: Because it will often be the case and the ReBar control
 * cannot handle the notifications propperly, they are then sent to the
 * grand-father of the menubar (i.e. parent of the ReBar).
 *
 * Application can also explicitly set the target window of the notifications
 * with the standard toolbar message @c TB_SETPARENT.
 *
 *
 * @section sec_mb_hotkeys Hot Keys
 *
 * To work as intended, the control requires some cooperation with the
 * application. The message loop in the application should call
 * the function @c mcIsMenubarMessage to handle hot keys of the menu items
 * and allow to activate the menu with the key @c F10.
 *
 * Hence code of the message loop in applications using the menubar control
 * should be similar to the example below:
 *
 * @code
 * MSG msg;
 * while(GetMessage(&msg, NULL, 0, 0)) {
 *     if(TranslateAccelerator(hWnd, hAccel, &msg))
 *         continue;
 *     if(mcIsMenubarMessage(hwndMenubar, &msg))
 *         continue;
 *     if(IsDialogMessage(hWnd, &msg))
 *         continue;
 *
 *     TranslateMessage(&msg);
 *     DispatchMessage(&msg);
 * }
 * @endcode
 *
 *
 * @section std_msgs Standard Messages
 *
 * These standard messages are handled by the control:
 * - @c CCM_SETNOTIFYWINDOW
 *
 * These standards notifications are sent by the control:
 * - @c NM_OUTOFMEMORY
 */


/**
 * @brief Registers window class of the control.
 * @return @c TRUE on success, @c FALSE on failure.
 * @sa @ref sec_init
 */
BOOL MCTRL_API mcMenubar_Initialize(void);

/**
 * @brief Unregisters window class of the control.
 * @sa @ref sec_init
 */
void MCTRL_API mcMenubar_Terminate(void);

/**
 * @brief Determines whether a message is intended for the specified
 * menubar control and, if it is, processes the message.
 * @param hwndMenubar The menubar control.
 * @param lpMsg The message.
 * @return @c TRUE, if the message has been processed; @c FALSE otherwise.
 */
BOOL MCTRL_API mcIsMenubarMessage(HWND hwndMenubar, LPMSG lpMsg);


/**
 * @name Window Class
 */
/*@{*/
/** @brief Window class name (unicode variant). */
#define MC_WC_MENUBARW       L"mCtrl.menubar"
/** @brief Window class name (ANSI variant). */
#define MC_WC_MENUBARA        "mCtrl.menubar"
/*@}*/


/**
 * @name Control Messages
 */
/*@{*/

/**
 * @brief Install a menu into the menubar.
 *
 * @param wParam Reserved, set to zero.
 * @param[in] lParam (@c HMENU) The menu to install.
 * @return @c TRUE on success, @c FALSE otherwise.
 */
#define MC_MBM_SETMENU            (WM_USER + 1000)

/**
 * @brief Updates the menubar to reflect changes in the installed menu.
 *
 * Application has to send this messages after it changes the top-level menu
 * items (e.g. adds or deleted a submenu, enables or disables a submenu etc.).
 *
 * @param wParam Reserved, set to zero.
 * @param lParam Reserved, set to zero.
 * @return @c TRUE on success, @c FALSE otherwise.
 */
#define MC_MBM_REFRESH            (WM_USER + 1001)

/*@}*/


/**
 * @name Unicode Resolution
 */
/*@{*/

/** @brief Unicode-resolution alias. @sa MC_WC_MENUBARW MC_WC_MENUBARA */
#define MC_WC_MENUBAR         MCTRL_NAME_AW(MC_WC_MENUBAR)

/*@}*/


#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif  /* MCTRL_MENUBAR_H */
