#ifndef HARASSMENT_DETECTOR_H
#define HARASSMENT_DETECTOR_H

#define MAX_EXPLANATION_LENGTH 512

typedef struct {
    int score;
    char categorie[64];
    char explication[MAX_EXPLANATION_LENGTH];
} AnalysisResult;

int analyze_comment(const char *comment, AnalysisResult *result);

#endif
