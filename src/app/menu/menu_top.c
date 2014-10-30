/**
 * @file    menu_top.c
 * @brief   Top level of the menu.
 *
 * @date    10/10/2014
 * @author  Harry Rostovtsev
 * @email   harry_rostovtsev@datacard.com
 * Copyright (C) 2014 Datacard. All rights reserved.
 *
 * @addtogroup groupMenu
 * @{
 */

/* Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "menu_top.h"
#include "qp_port.h"                                        /* for QP support */
#include "project_includes.h"

#include "debug_menu.h"
#include "dbg_out_cntrl.h"
#include "dbg_mod_cntrl.h"
#include "systest_menu.h"
#include "systest_i2c.h"

/* Compile-time called macros ------------------------------------------------*/
Q_DEFINE_THIS_FILE                  /* For QSPY to know the name of this file */
DBG_DEFINE_THIS_MODULE( DBG_MODL_DBG );/* For debug system to ID this module */

/* Private typedefs ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables and Local objects ---------------------------------------*/

/**
 * @brief   Storage for the root of the menu.  MenuMgr has a pointer to this.
 */
treeNode_t menu;

/**
 * @brief   Text of the main menu.  Displayed when the menu gets printed.
 */
char *const menu_TitleTxt = "Coupler Board Menu";

/**
 * @brief   Selector key for the top of the menu.
 * This is the letter sequence that the user inputs to jump back to the top of
 * the menu.
 */
char *const menu_SelectKey = "T";

/* Private function prototypes -----------------------------------------------*/


/**
 * @brief   Prints out the help text for the entire menu system.
 * @param [in] msgSrc: MsgSrc var that specifies where to print out to.
 * @return  None
 */
static void MENU_printHelp( MsgSrc msgSrc );

/**
 * @brief   Prints out the entire expanded menu tree
 * This is just a wrapper around the recursive function that prints the entire
 * menu tree with some pretty-print code added in to make it obvious where the
 * previous menu was printed and where the new one was.
 *
 * @param [in] node: treeNode_t* pointer to the root of the menu.
 * @param [in] msgSrc: MsgSrc var that specifies where to print out to.
 * @return  None
 */
static void MENU_printEntireExpandedMenu( treeNode_t* node, MsgSrc msgSrc );

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
treeNode_t* MENU_init( void )
{
   /* 1. This has to get called first.  It initializes the top level of the menu */
   MENU_initTopLevel( &menu, menu_TitleTxt, menu_SelectKey );

   /* 2. At this point, submenus and their items can be added.  The nodes, text,
    * actions, etc have to be defined in their respective c files and the vars
    * declared "extern" so they can be accessed from here. The menus and menu
    * items will appear in the order in which they are added here. */

   /* Add the DEBUG menu */
   MENU_addSubMenu(
         &menuDbg,                                      /**< Menu being added */
         &menu,                           /**< Parent of the menu being added */
         menuDbg_TitleTxt,                   /**< Menu being added title text */
         menuDbg_SelectKey,               /**< Menu being added selection key */
         NULL                         /**< Action taken when menu is selected */
   );

      /* Add a Debug Output Control sub-menu under the DEBUG menu */
      MENU_addSubMenu(
            &menuDbgOutCntrl,                              /**< Menu being added */
            &menuDbg,                        /**< Parent of the menu being added */
            menuDbgOutCntrl_TitleTxt,           /**< Menu being added title text */
            menuDbgOutCntrl_SelectKey,       /**< Menu being added selection key */
            NULL                         /**< Action taken when menu is selected */
      );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgOutCntrlItem_toggleSerialDebug,   /**< Menu item being added */
               &menuDbgOutCntrl,                   /**< Parent of the menu item being added */
               menuDbgOutCntrlItem_toggleSerialDebugTxt,  /**< Menu item title text */
               menuDbgOutCntrlItem_toggleSerialDebugSelectKey, /**< Menu item selection key */
               MENU_toggleSerialDebugAction /**< Action taken when menu item is selected */
         );

         MENU_addMenuItem(
               &menuDbgOutCntrlItem_toggleEthDebug,      /**< Menu item being added */
               &menuDbgOutCntrl,                   /**< Parent of the menu item being added */
               menuDbgOutCntrlItem_toggleEthDebugTxt,     /**< Menu item title text */
               menuDbgOutCntrlItem_toggleEthDebugSelectKey, /**< Menu item selection key */
               MENU_toggleEthDebugAction  /**< Action taken when menu item is selected */
         );

      /* Add a Debug Module Control sub-menu under the DEBUG menu */
      MENU_addSubMenu(
            &menuDbgModCntrl,                              /**< Menu being added */
            &menuDbg,                        /**< Parent of the menu being added */
            menuDbgModCntrl_TitleTxt,           /**< Menu being added title text */
            menuDbgModCntrl_SelectKey,       /**< Menu being added selection key */
            NULL                         /**< Action taken when menu is selected */
      );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModGen,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModGenTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModGenSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModGeneralAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModSer,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModSerTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModSerSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModSerialAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModTime,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModTimeTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModTimeSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModTimeAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModEth,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModEthTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModEthSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModEthAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModI2C,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModI2CTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModI2CSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModI2CAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModNOR,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModNORTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModNORSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModNORAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModSDR,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModSDRTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModSDRSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModSDRAMAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModDBG,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModDBGTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModDBGSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModDBGAction /**< Action taken when menu item is selected */
         );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuDbgModCntrlItem_toggleModCOMM,   /**< Menu item being added */
               &menuDbgModCntrl,                   /**< Parent of the menu item being added */
               menuDbgModCntrlItem_toggleModCOMMTxt,  /**< Menu item title text */
               menuDbgModCntrlItem_toggleModCOMMSelectKey, /**< Menu item selection key */
               MENU_toggleDbgModCOMMAction /**< Action taken when menu item is selected */
         );

   /* Add the SYSTEST menu */
   MENU_addSubMenu(
         &menuSysTest,                                  /**< Menu being added */
         &menu,                           /**< Parent of the menu being added */
         menuSysTest_TitleTxt,               /**< Menu being added title text */
         menuSysTest_SelectKey,           /**< Menu being added selection key */
         NULL                         /**< Action taken when menu is selected */
   );

      /* Add a Debug Output Control sub-menu under the DEBUG menu */
      MENU_addSubMenu(
            &menuSysTest_I2C,                           /**< Menu being added */
            &menuSysTest,                 /**< Parent of the menu being added */
            menuSysTest_I2C_TitleTxt,        /**< Menu being added title text */
            menuSysTest_I2C_SelectKey,    /**< Menu being added selection key */
            NULL                      /**< Action taken when menu is selected */
      );

         /* Add menu items for this menu */
         MENU_addMenuItem(
               &menuItem_runI2CEEPROMReadTest,     /**< Menu item being added */
               &menuSysTest_I2C,     /**< Parent of the menu item being added */
               menuSysTest_runI2CEEPROMReadTest_Txt,  /**< Menu item title text */
               menuSysTest_runI2CEEPROMReadTest_SelectKey, /**< Menu item selection key */
               MENU_i2cEEPROMReadTestAction /**< Action taken when menu item is selected */
         );



   /* Uncomment to print the entire tree with node addresses (for debugging only) */
//   KTREE_printTree(&menu, 0);

   return( &menu ); /* return a pointer to the top level of the menu tree */
}

/******************************************************************************/
treeNode_t* MENU_parse(
      treeNode_t* node,
      const char* pBuffer,
      uint16_t bufferLen,
      MsgSrc msgSrc
)
{
   treeNode_t *newNode = node;

   /* Sanity check. If the passed in node is null, set to the top of the menu
    * and inform user. This really shouldn't happen and if it does, it is a bug
    * in the menu handling application. */
   if ( NULL == node ) {
      WRN_printf("Passed in a null node.  Setting to top of menu\n");
      newNode = &menu;
      MENU_printMenuExpandedAtCurrNode( newNode, msgSrc );
      return( newNode );
   }

   DBG_printf("BufferLen %d\n", bufferLen);

   /* Parse the universal commands first */
   if ( 0 == strncmp((const char *)pBuffer, "?", 1 ) && 1 == bufferLen-1 ) {
      MENU_printHelp( msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "T", 1 ) && 1 == bufferLen-1 ) {
      newNode = &menu;
      MENU_printMenuExpandedAtCurrNode( newNode, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "P", 1 ) && 1 == bufferLen-1 ) {
      MENU_printMenuExpandedAtCurrNode(newNode, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "A", 1 ) && 1 == bufferLen-1 ) {
      /* Have to pass in the root of the menu to make sure to get the whole menu
       * and not forget where we are currently at. */
      MENU_printEntireExpandedMenu( &menu, msgSrc );
   } else if ( 0 == strncmp((const char *)pBuffer, "U", 1 ) && 1 == bufferLen-1 ) {
      if ( NULL != node->fakeParentNode ) {
         newNode = node->fakeParentNode;
      } else {
         LOG_printf("Already at the top of the menu\n");
      }
      MENU_printMenuExpandedAtCurrNode(newNode, msgSrc );
   } else {
      newNode = MENU_parseCurrLevelMenuItems(node, pBuffer, bufferLen, msgSrc);
   }
   return( newNode );
}

/******************************************************************************/
static void MENU_printHelp( MsgSrc msgSrc )
{
   MENU_printf(msgSrc, "******************************************************************************\n");
   MENU_printf(msgSrc, "*****                           Menu Help                                *****\n");
   MENU_printf(msgSrc, "***** Type the following commands:                                       *****\n");
   MENU_printf(msgSrc, "***** '?': menu help                                                     *****\n");
   MENU_printf(msgSrc, "***** 'P': print currently selected menu                                 *****\n");
   MENU_printf(msgSrc, "***** 'A': print entire expanded menu tree                               *****\n");
   MENU_printf(msgSrc, "***** 'T': go to the top of the menu                                     *****\n");
   MENU_printf(msgSrc, "***** 'U': go up one level from where you're currently at                *****\n");
   MENU_printf(msgSrc, "***** any identifier to select the submenu or menu item on your level    *****\n");
   MENU_printf(msgSrc, "******************************************************************************\n");
}

/******************************************************************************/
static void MENU_printEntireExpandedMenu( treeNode_t* node, MsgSrc msgSrc )
{
   MENU_printf(msgSrc, "******************************************************************************\n");
   MENU_printMenuTree(&menu, msgSrc );
   MENU_printf(msgSrc, "******************************************************************************\n\n");
}
/**
 * @}
 * end addtogroup groupMenu
 */

/******** Copyright (C) 2014 Datacard. All rights reserved *****END OF FILE****/
