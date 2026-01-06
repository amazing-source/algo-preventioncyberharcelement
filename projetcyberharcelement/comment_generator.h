#ifndef COMMENT_GENERATOR_H
#define COMMENT_GENERATOR_H

#define MAX_COMMENTS 50
#define MAX_COMMENT_LENGTH 512

typedef struct {
    int id;
    char texte[MAX_COMMENT_LENGTH];
} Comment;

typedef struct {
    Comment comments[MAX_COMMENTS];
    int count;
} CommentList;

int generate_comments(const char *theme, int count, CommentList *list);

#endif
