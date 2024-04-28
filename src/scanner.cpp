#include "scanner.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void Token_Print(const char * source_code) {
	FILE* fp = fopen(source_code, "r");
	if (!fp) {
		perror("fopen failed");
		exit(1);
	}

	list_head* token_list = NULL;
	if(get_tokens(fp, &token_list)){
		fprintf(stderr,"get tokens failed\n");
		exit(1);
	}

	FILE* fp2 = fopen("token.txt", "w");
	if (!fp2) {
		perror("fopen failed");
		exit(1);
	}

	print_token_list(fp2, token_list);

	fclose(fp);
	fclose(fp2);
}

token_t* alloc_token() {
	token_t* t = (token_t*)malloc(sizeof(token_t));
	t->line = -1;

	t->sem_len = 0;
	t->sem_buf_size = 1;
	t->sem = (char*)malloc(1);
	t->lex_type = _NULL_TYPE;
	INIT_LIST_HEAD(&t->list);

	return t;
}


void release_token(token_t* token) {
	if (token->sem) {
		free(token->sem);
		token->sem = NULL;
		token->sem_len = 0;
	}

	free(token);
}

void token_sem_append_char(token_t* token, char ch) {
	if (token->sem_len + 1 > token->sem_buf_size) {
		token->sem_buf_size <<= 1;		// * 2
		token->sem = (char*)realloc(token->sem, token->sem_buf_size);
	}

	token->sem[token->sem_len++] = ch;
}


void token_sem_append_string(token_t* token, const char* s, int s_len) {

	if (token->sem_len + s_len > token->sem_buf_size) {
		token->sem_buf_size <<= 1;		// * 2
		token->sem = (char*)realloc(token->sem, token->sem_buf_size);
	}

	memcpy(&token->sem[token->sem_len], s, s_len);
	token->sem_len += s_len;
}


const char* snl_reserved_words[] = {
	NULL,
	"program",
	"procedure",
	"type",
	"var",
	"if",
	"then",
	"else",
	"fi",
	"while",
	"do",
	"endwh",
	"begin",
	"end",
	"read",
	"write",
	"array",
	"of",
	"record",
	"return",
	"integer",
	"char",
};


//���ַ��ָ���
const char* separator[] = {
	"=",
	"<",
	"+",
	"-",
	"*",
	"/",
	"(",
	")",
	".",
	";",
	"[",
	"]",
	","
};


//
int is_reserved_word(const char* word) {
	for (int i = 1;
		i < (sizeof(snl_reserved_words) / sizeof(snl_reserved_words[0]));
		i++) {
		if (!memcmp(word, snl_reserved_words[i], strlen(snl_reserved_words[i])))
			return i;
	}
	return 0;
}

int is_single_char_separator(char ch) {
	for (int i = 0;
		i < (sizeof(separator) / sizeof(separator[0]));
		i++) {
		if (separator[i][0] == ch) {
			return i;
		}
	}
	return -1;
}

int get_char(FILE* fp, int* line) {
	int ch = fgetc(fp);
	if (ch == '\n') {
		++(*line);
	}
	return ch;
}


void unget_char(FILE* fp, int ch, int* line) {
	if (ch == EOF) {
		return;
	}

	ungetc(ch, fp);
	if (ch == '\n') {
		--(*line);
	}
}

int  get_next_token(FILE* code, token_t** token) {

	int err = 0;
	int ch = 0;
	static int cur_line = 1;

	//skip space.
	do {
		ch = get_char(code, &cur_line);
	} while (isspace(ch));

	if (ch == EOF) {
		*token = NULL;
		return 0;
	}


	//read token.
	*token = alloc_token();

match_id:
	if (isalpha(ch)) {										//match id
		token_sem_append_char(*token, ch);
		(*token)->lex_type = _ID;
		(*token)->line = cur_line;

		while (1) {
			ch = get_char(code, &cur_line);

			if (ch == EOF) {
				goto done;
			}

			if (!isalnum(ch)) {
				unget_char(code, ch, &cur_line);
				break;
			}

			token_sem_append_char(*token, ch);
		}
		//is reserver word ?

		int reserved = is_reserved_word((*token)->sem);
		if (reserved) {
			(*token)->lex_type = (_lex_type_t)reserved;
		}

		goto done;
	}

match_number:
	if (isdigit(ch)) {								//match number
		token_sem_append_char(*token, ch);
		//

		(*token)->lex_type = _INTC;
		(*token)->line = cur_line;

		while (1) {
			ch = get_char(code, &cur_line);

			if (ch == EOF) {
				goto done;
			}

			if (!isdigit(ch)) {
				unget_char(code, ch, &cur_line);
				goto done;
			}

			token_sem_append_char(*token, ch);
		}
	}

match_assign:
	if (ch == ':') {								//match assign.
		int next_ch = get_char(code, &cur_line);
		if (next_ch != '=') {
			unget_char(code, next_ch, &cur_line);
			goto match_comment;
		}

		token_sem_append_string(*token, ":=", 2);
		//

		(*token)->lex_type = _ASSIGN;
		(*token)->line = cur_line;
		goto done;
	}

match_comment:
	if (ch == '{') {								//match comment.
		(*token)->lex_type = _COMMENT;
		(*token)->line = cur_line;

		while (1) {
			ch = get_char(code, &cur_line);
			if (ch == EOF || ch == '}') {
				goto done;
			}

			token_sem_append_char(*token, ch);
		}
	}

match_char:
	if (ch == '\'') {								//match char.
		char ascii[0x20];
		int next_ch = get_char(code, &cur_line);
	/*	if (!isalnum(next_ch)) {
			unget_char(code, next_ch, &cur_line);
			goto match_underange;
		}*/
		//�����ַ���ascii��.
		sprintf(ascii, "%d", next_ch);
		token_sem_append_string(*token, ascii, strlen(ascii));
		
		(*token)->lex_type = _CHARC;
		(*token)->line = cur_line;

		ch = get_char(code, &cur_line);
		if (ch != '\'') {
			err = -1;				//read char failed.
			goto done;
		}

		goto done;
	}

match_underange:
	if (ch == '.') {
		int next_ch = get_char(code, &cur_line);
		if (next_ch != '.') {
			unget_char(code, next_ch, &cur_line);
			goto match_single_separator;
		}

		token_sem_append_string(*token, "..", 2);
		(*token)->lex_type = _UNDERANGE;
		(*token)->line = cur_line;
		goto done;
	}

match_single_separator:

	if (is_single_char_separator(ch) >= 0) {
		(*token)->lex_type = (lex_type_t)(_EQ + is_single_char_separator(ch));
		(*token)->line = cur_line;
		token_sem_append_char(*token, ch);
		goto done;
	}

match_error:
	err = -1;

done:
	if (err) {
		release_token(*token);
		*token = NULL;
	}
	return err;
}




int get_tokens(FILE* fp, list_head** token_list) {
	list_head* tokens = (list_head*)malloc(sizeof(list_head));
	token_t* token = NULL;
	int err = 0;

	if (tokens == NULL) {
		return -1;
	}

	INIT_LIST_HEAD(tokens);

	while (1) {
		err = get_next_token(fp, &token);
		if (err) {
			//get token failed,release tokens.
			while (!list_empty(tokens)) {
				list_head* next = tokens->next;
				list_del(next);

				token = container_of(next, token_t, list);
				release_token(token);
			}
			return err;
		}

		//read EOF.
		if (token == NULL) {
			break;
		}

		list_add_tail(&token->list, tokens);
	}
	*token_list = tokens;
	return err;
}

void print_token(FILE* fp, token_t* token) {
	const char* sem = sem_info[token->lex_type];
	int sem_length = 0;

	if (!sem) {
		sem = token->sem;
		sem_length = token->sem_len;
	}
	else {
		sem_length = strlen(sem);
	}


	fprintf(fp, "%03d %s ", token->line, lex_types[token->lex_type]);
	fwrite(sem, 1, sem_length, fp);
	fputc('\n', fp);
	fflush(fp);
}

void print_token_list(FILE* fp, list_head* token_list)
{
	for (list_head* t = token_list->next;
		t != token_list;
		t = t->next) {

		token_t* token = container_of(t, token_t, list);

		print_token(fp, token);
	}
}