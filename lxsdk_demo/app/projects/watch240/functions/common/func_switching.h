/*
 * @brief: 
 * @LastEditors: ljf
 * @LastEditTime: 2025-07-21 19:08:45
 * @FilePath: \test\lxsdk_demo\app\projects\watch240\functions\common\func_switching.h
 * @Date: 2025-07-09 15:48:00
 */
#ifndef _SWITCHING_H
#define _SWITCHING_H

#define FUNC_SWITCH_AUTO                0x8000                  //自动完成切换

enum FUNC_SWITCH_MODE {
    FUNC_SWITCH_DIRECT,                                         //无切换/直接切换
    FUNC_SWITCH_CANCEL,                                         //取消切换

    //淡入淡出
    FUNC_SWITCH_FADE,       // 淡入效果  
    FUNC_SWITCH_FADE_OUT,   // 淡出效果
    
    //左右切换（水平方向） 
    FUNC_SWITCH_LR,                    // 左右切换基础值  
    FUNC_SWITCH_LR_LEFT,               // 向左平移切换  
    FUNC_SWITCH_LR_RIGHT,              // 向右平移切换    
    FUNC_SWITCH_LR_ZOOM_LEFT,          // 向左缩放切换  
    FUNC_SWITCH_LR_ZOOM_RIGHT,         // 向右缩放切换

    //上下切换（垂直方向）
    FUNC_SWITCH_UD,                    // 上下切换基础值  
    FUNC_SWITCH_UD_UP,                 // 向上平移切换  
    FUNC_SWITCH_UD_DOWN,               // 向下平移切换  
    FUNC_SWITCH_UD_ZOOM_UP,            // 向上缩放切换  
    FUNC_SWITCH_UD_ZOOM_DOWN,          // 向下缩放切换

    //菜单切换效果
    FUNC_SWITCH_MENU,                  // 菜单切换基础值  
    FUNC_SWITCH_MENU_DROPDOWN_UP,      // 下拉菜单向上收起  
    FUNC_SWITCH_MENU_DROPDOWN_DOWN,    // 下拉菜单向下展开  
    FUNC_SWITCH_MENU_PULLUP_UP,        // 上拉菜单向上展开  
    FUNC_SWITCH_MENU_PULLUP_DOWN,      // 上拉菜单向下收起  
    FUNC_SWITCH_MENU_SIDE_POP,         // 侧边栏弹出  
    FUNC_SWITCH_MENU_SIDE_BACK,        // 侧边栏收回

    //进出从图标缩放切换
    FUNC_SWITCH_ZOOM,                  // 缩放切换基础值  
    FUNC_SWITCH_ZOOM_ENTER,            // 从图标放大进入应用  
    FUNC_SWITCH_ZOOM_EXIT,             // 缩小到图标退出应用

    //进出淡入切换
    FUNC_SWITCH_ZOOM_FADE,             // 缩放淡入基础值  
    FUNC_SWITCH_ZOOM_FADE_ENTER,       // 缩放淡入进入应用  
    FUNC_SWITCH_ZOOM_FADE_EXIT,        // 缩放淡出退出应用
};

bool func_switching(u16 switch_mode, void *param);              //转场动画

#endif
