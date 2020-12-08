#ifndef MAP_DATA_H
#define MAP_DATA_H

#define MAP_TEST_ROW_CNT 20
#define MAP_TEST_COL_CNT 30

static const int MAP_TEST_FLOOR[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,
0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,
1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,
1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
0,0,0,0,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,
0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0,
0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,0,0,0
};

static const int MAP_TEST_WALL[] = {
0,0,0,0,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,51,60,0,0,0,0,0,0,0,0,
0,0,0,0,48,53,53,53,53,53,53,53,53,53,17,53,53,53,53,53,52,58,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,50,50,50,50,50,50,50,0,0,0,0,58,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,48,52,52,52,52,52,52,58,0,0,0,58,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,58,0,0,0,0,0,0,58,0,0,0,58,0,0,0,0,0,0,0,0,
61,51,51,0,58,0,0,0,0,0,58,0,0,0,0,0,0,58,0,0,0,46,50,50,41,50,50,60,0,0,
48,17,49,51,58,0,0,0,0,0,58,0,0,0,0,0,0,58,0,0,0,52,52,52,35,52,52,58,0,0,
58,0,55,52,56,0,0,0,0,0,46,50,60,0,0,0,0,58,0,0,0,0,0,0,0,0,0,58,0,0,
58,0,0,51,0,0,0,0,0,0,52,52,46,51,51,51,51,58,0,0,0,0,0,0,0,0,0,58,0,0,
46,51,47,53,58,0,0,0,0,0,0,0,52,52,38,52,52,56,0,0,0,0,0,0,0,0,0,58,0,0,
53,53,53,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,46,51,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,53,49,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,59,27,27,27,27,0,27,27,27,47,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,52,53,23,49,0,48,53,53,53,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,0,0,51,47,0,20,27,0,0,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,0,0,48,53,0,16,49,0,0,0,
0,0,0,0,46,27,27,27,27,60,0,0,0,0,0,0,0,0,0,57,0,0,58,0,0,0,57,0,0,0,
0,0,0,0,23,23,23,23,23,46,27,27,27,27,27,27,27,27,27,47,0,0,46,51,51,51,47,0,0,0,
0,0,0,0,0,0,0,0,0,23,23,23,23,23,23,23,23,23,23,23,0,0,53,53,53,53,53,0,0,0
};

static const int MAP_TEST_FRONT[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,51,51,51,51,51,51,51,60,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,48,52,52,53,44,52,45,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
58,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
58,0,59,51,60,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
46,50,47,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
22,22,22,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,59,50,50,50,50,0,50,50,50,47,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,52,52,52,49,0,48,53,53,53,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,58,0,0,0,0,0,0,0,0,0,0,0,0,0,0,57,0,0,58,0,0,0,57,0,0,0,
0,0,0,0,46,50,50,50,50,60,0,0,0,0,0,0,0,0,0,57,0,0,58,0,0,0,57,0,0,0,
0,0,0,0,52,52,52,52,52,46,50,50,50,50,50,50,50,50,50,47,0,0,46,50,50,50,47,0,0,0,
0,0,0,0,0,0,0,0,0,52,52,52,52,52,52,52,52,52,52,52,0,0,52,52,52,52,52,0,0,0
};

#endif //MAP_DATA_H