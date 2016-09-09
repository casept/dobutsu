#include <stddef.h>

/* fields on the board:
 *
 * B A 9
 * 8 7 6
 * 5 4 3
 * 2 1 0
 *
 * everything else means piece is on hand.
 */

/* an encoded game position */
typedef unsigned pos_code;
typedef unsigned short board;

/* all squares on the board */
#define ALL_SQUARES 07777

#define MAX_POS (pos_code)(256 * 24 * 56 * 56 * 56U)
#define IN_HAND 12

/* an unpacked game position */
struct position {
	unsigned char l, L, c, C, e, E, g, G;
	unsigned char op;
};

/* for struct position.op */
enum {
	co = 1 << 0,	/* owner 1st chick */
	Co = 1 << 1,	/* owner 2nd chick */
	cp = 1 << 2,	/* promotion 1st chick */
	Cp = 1 << 3,	/* promotion 2nd chick */
	eo = 1 << 4,	/* owner 1st elephant */
	Eo = 1 << 5,	/* owner 2nd elephant */
	go = 1 << 6,	/* owner 1st giraffe */
	Go = 1 << 7	/* owner 2nd giraffe */
};

/*
 * A move description.  piece refers to one of the numbers below.
 * Note that this is always a move for Sente, Gote's moves are never
 * considered as we flip the board between moves.
 */
struct move {
	unsigned char piece, to;
};

/*
 * piece numbers for struct moved defined using offsetof so we can
 * perform pointer magic to set up struct position.
 */
enum {
	PIECE_l = offsetof(struct position, l),
	PIECE_L = offsetof(struct position, L),
	PIECE_c = offsetof(struct position, c), /* don't care about promotion */
	PIECE_C = offsetof(struct position, C), /* dito */
	PIECE_e = offsetof(struct position, e),
	PIECE_E = offsetof(struct position, E),
	PIECE_g = offsetof(struct position, g),
	PIECE_G = offsetof(struct position, G),
	MAX_PIECE = PIECE_G
};

/* said pointer magic */
#define PIDX(pos, piece) ((unsigned char*)(pos))[piece]

/*
 * The maximum number of moves returned by enumerate_moves().  This
 * number (68) occurs e.g. in position 4851 when the Sente lion has free
 * movement and all other pieces are in Sente's hand to be dropped on
 * the 10 free squares on the board:
 *
 * +---+
 * |  l|
 * |   |
 * | L | GGEECC
 * +---+
 */
#define MAX_MOVES (10 * 6 + 8)

/* from poscode.c */
extern pos_code encode_pos(const struct position*);
extern pos_code encode_pos_check(const struct position*);
extern void decode_pos(struct position*, pos_code);
extern int check_pos(const struct position*);

/* return values for decode_pos, encode_pos and various other functions */
enum {
	POS_OK      = 0,  /* used internally, not actually returned */
	POS_INVALID = -1, /* invalid position */
	POS_SENTE   = -2, /* gote lion is mated or sente lion is on second row */
	POS_IOERROR = -3, /* an IO error occured */
	POS_DRAW    = -4, /* the game is a draw */
};

#define MOVE_LENGTH 5
#define POS_LENGTH 25

/* from display.c */
extern int display_pos(const struct position*);
extern int show_pos(const struct position*);
extern void pos_notation(char *, int, const struct position*);
extern int describe_move(const struct position*, struct move);
extern void move_notation(char *, const struct position*, struct move);

/* from notation.c */
extern int parse_position(struct position*, const char*);
extern int parse_move(struct move *, const struct position*, const char*);

enum to_move {
	TURN_GOTE = 0,		/* it's gote's turn */
	TURN_SENTE = 1,		/* it's sente's turn */
	TURN_INVALID = -1,	/* position string is invalid */
};

/* from tables.c */
extern const board Rmoves[13], rmoves[13], Llmoves[13], Ggmoves[13], Eemoves[13], Cmoves[13], cmoves[13];
extern const unsigned char vert_mirror[13], turned_board[13];

/* from postabs.c, generated by gentabs */
extern const unsigned char lion_decoding[24], pos1_decoding[24 * 11], pos2_decoding[11 * 10 / 2 + 1], flip_op[256];
extern const signed char lion_encoding[7 * 8], pos1_encoding[24 * 13], pos2_encoding[11 * 11];

/* from moves.c */
extern unsigned generate_most_moves(struct move*, const struct position*);
extern unsigned generate_all_moves(struct move*, const struct position*);
extern unsigned generate_all_moves_for(int, struct move*, const struct position*);

extern void turn_position(struct position*);
extern void turn_move(struct move*);
extern void apply_move(struct position*, struct move);
extern int apply_move_for(int, struct position*, struct move);

/* from gamedb.c */
typedef struct gamedb GAMEDB;
extern GAMEDB *open_gamedb(const char*);
extern int distance_to_mate(GAMEDB*, const struct position*, int);
extern void close_gamedb(GAMEDB*);
