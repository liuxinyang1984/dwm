/* See LICENSE file for copyright and license details. */

/* appearance */
static const unsigned int borderpx  = 5;        /* border pixel of windows */
static const unsigned int gappx     = 5;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft = 1;    /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor */
static const int showsystray        = 1;        /* 0 means no systray */
static const int swallowfloating    = 0;        /* 1 means swallow floating windows by default */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
/* 0 = 跟字体；>0 固定栏高(px)。字号 12 时约等于原 size=16 栏高 */
static const unsigned int barheight = 22;
static const char *fonts[]          = { "Maple Mono NL NF:size=12" };
static const char dmenufont[]       = "Maple Mono NL NF:size=12";
/* Gruvbox Dark — 与 st / neovim 对齐 */
static const char col_gray1[]       = "#282828"; /* bg */
static const char col_gray2[]       = "#3c3836"; /* border norm */
static const char col_gray3[]       = "#a89984"; /* fg muted */
static const char col_gray4[]       = "#ebdbb2"; /* fg */
static const char col_cyan[]        = "#458588"; /* dmenu sel bg */
static const char col_sel_fg[]      = "#fabd2f"; /* 选中 tag/标题字：Gruvbox 亮黄 */
static const char col_border[]      = "#458588"; /* 选中窗口边框 */
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_sel_fg, col_gray1, col_border },
};

/* tagging */
/* 栏最左 Arch 图标（Nerd Font nf-linux-archlinux）；空串 "" 关闭 */
static const char *archicon = "";
static const char *tags[] = { "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ "Gimp",    NULL,     NULL,           0,         1,          0,           0,        -1 },
	{ "Firefox", NULL,     NULL,           1 << 8,    0,          0,          -1,        -1 },
	{ "St",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 120;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[T]",      tile },    /* first entry is default */
	{ "[F]",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
/* MacBook / 纯 X11：Super；与 Hyprland 同机时勿同时抢 Mod4 */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
/* -nb/-nf 列表；-sb/-sf 选中（黄）。输入行色在 dmenu SchemePrompt，不受这两对覆盖 */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-c", "-l", "5", "-bw", "5", "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", "#d79921", "-sf", col_gray1, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "st", "-t", scratchpadname, "-g", "120x34", NULL };
static const char *xkillcmd[] = { "xkill", NULL };
/* 剪贴板历史：对齐本机 Hypr Super+V（cliphist+wofi）→ X11 clipmenu+dmenu */
static const char *clipmenucmd[] = { "dmenu-clip", NULL };

#include <X11/XF86keysym.h>
#include "movestack.c"
static const Key keys[] = {
	/* modifier                     key        function        argument */
	/* 多媒体键（各笔记本通用；Mac 在 fnmode=2 时需按 Fn） */
	{ 0, XF86XK_AudioMute,          spawn, SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle") },
	{ 0, XF86XK_AudioLowerVolume,   spawn, SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-") },
	{ 0, XF86XK_AudioRaiseVolume,   spawn, SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+") },
	{ 0, XF86XK_MonBrightnessDown,  spawn, SHCMD("brightnessctl set 5%-") },
	{ 0, XF86XK_MonBrightnessUp,    spawn, SHCMD("brightnessctl set 5%+") },
	/* Apple 键盘背光（机型相关；需要时取消注释并重编 dwm）
	{ 0, XF86XK_KbdBrightnessDown,  spawn, SHCMD("brightnessctl -d smc::kbd_backlight set 5%-") },
	{ 0, XF86XK_KbdBrightnessUp,    spawn, SHCMD("brightnessctl -d smc::kbd_backlight set 5%+") },
	*/
	{ MODKEY,                       XK_space,  spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_v,      spawn,          {.v = clipmenucmd } }, /* 剪贴板历史 */
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_h,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_l,      focusstack,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	/* 固定 Super+Alt，不依赖 MODKEY（避免 MODKEY=Alt 时叠不成组合键） */
	{ Mod4Mask|Mod1Mask,            XK_h,      setmfact,       {.f = -0.05} },
	{ Mod4Mask|Mod1Mask,            XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY|ControlMask,           XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_q,      spawn,          {.v = xkillcmd } },
	{ MODKEY,                       XK_t,      togglefloating, {0} },
	{ MODKEY,                       XK_m,      togglelayout,   {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_m,      togglefullscreen, {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefakefullscreen, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_minus,  setgaps,        {.i = -1 } },
	{ MODKEY,                       XK_equal,  setgaps,        {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_equal,  setgaps,        {.i = 0 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },
	{ MODKEY|ControlMask|ShiftMask, XK_q,      quit,           {1} },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

