enum type {
	/*
	|| - LOG_OR
	&& - LOG_AND
	| - CONV
	& - MUTE
	; - SEMICOLON
	>> - WR_APP
	> - WR
	< - RD
	argv - ARGV
	*/
	LOG_OR,
	LOG_AND,
	CONV,
	MUTE,
	SEMICOLON,
	WR_APP,
	WR,
	RD,
	ARGV,
	LEFT_BR,
	RIGHT_BR,
	EOI
};

enum {
	MAXLEN = 255
};

struct node {
	enum type op_type;
	char ** argv;
	int argc;
	char * path;
	struct node * left, * right; 
};