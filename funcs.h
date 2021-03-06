extern WINDOW *CreateWindow(WINDOW *old_win, int lines, int cols, int y, int x);
extern int GetString(WINDOW *win, char *str, int max_len);
extern void RemoveWindow(WINDOW *old_win, WINDOW *new_win);
extern void EndFIFO(void);
extern void EndScreen(void);
extern void SeedRnd(void);
extern void DeltaInits(void);
extern WINDOW *InitScreen(void);
extern void Welcome(WINDOW *wel_w);
extern void BlockSignals(void);
extern int GetFIFOName(WINDOW *old_w, int *pipe_w, int *pipe_r, char *you, char *hom, coords_t *us, coords_t *them);
extern void CompInit(void);
extern void GetReady(WINDOW *win, int p_read, int p_write, int game_count, char *hom);
extern void CompGameInit(void);
extern void DrawPositions(WINDOW *board_w, int board[Y_MAX][X_MAX], coords_t *us, coords_t *them);
extern int GetMoves(WINDOW *board_w, int board[Y_MAX][X_MAX], int pipe_r, int pipe_w, coords_t *us, coords_t *them);
extern void ComputeMove(int board[Y_MAX][X_MAX], coords_t *us, coords_t *them);
extern void MillisecondSleep(int ms);

extern int ComputerOpponent;
extern int IComputer;
extern int StopIO;
extern int ChildPid;
extern int SlowCPU;
extern int DisplayPlay;
