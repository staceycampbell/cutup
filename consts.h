#define ASCII_BS 8
#define ASCII_DEL 127

#ifndef LFNMAX
#define NAME_MAX 7
#else
#define NAME_MAX ((LFNMAX) / 2)
#endif

#ifndef LPNMAX
#define FN_MAX 256
#else
#define FN_MAX LPNMAX
#endif

#define Y_MAX 21
#define X_MAX 78

#define S_FREE       0x000
#define S_ONE        0x001
#define S_OTHER      0x002
#define S_BOTH       0x003
#define S_BORDER     0x004
#define S_CRASH      0x080
#define S_TEMP       0x100

#define S_NO_COLLISION  0x0000
#define S_WE_COLLIDE   0x0001
#define S_THEY_COLLIDE 0x0002

#define S_NORTH 0
#define S_SOUTH 1
#define S_EAST 2
#define S_WEST 3
#define S_SAME 4
#define S_BOGUS 5
#define S_DELTA_MAX (S_BOGUS + 1)
#define S_DIR_MAX (S_WEST + 1)

#define S_LEFT_CH      'h'
#define S_LEFT_ALT_CH  'a'
#define S_RIGHT_CH     'l'
#define S_RIGHT_ALT_CH 's'
#define S_UP_CH        'k'
#define S_UP_ALT_CH    'w'
#define S_DOWN_CH      'j'
#define S_DOWN_ALT_CH  'z'

#define S_US_CHAR '@'
#define S_THEM_CHAR '#'
#define S_BANG_CHAR '*'

#define SCORE_MAX 3

#define C_RIGHT 0
#define C_LEFT 1
#define C_FORWARD 2
#define C_REVERSE 3

#define C_MAX (C_REVERSE + 1)
#define LR_MAX (C_LEFT + 1)

#define COUNT_LIMIT 200
